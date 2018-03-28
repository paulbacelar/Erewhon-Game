// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <Shared/Protocol/CompressedInteger.hpp>

namespace ewn
{
	template<typename T>
	CompressedSigned<T>::CompressedSigned(T value) :
	m_value(value)
	{
	}

	template<typename T>
	CompressedSigned<T>::operator T() const
	{
		return m_value;
	}

	template<typename T>
	CompressedSigned<T>& CompressedSigned<T>::operator=(T value)
	{
		m_value = value;
		return *this;
	}


	template<typename T>
	CompressedUnsigned<T>::CompressedUnsigned(T value) :
	m_value(value)
	{
	}

	template<typename T>
	CompressedUnsigned<T>::operator T() const
	{
		return m_value;
	}

	template<typename T>
	CompressedUnsigned<T>& CompressedUnsigned<T>::operator=(T value)
	{
		m_value = value;
		return *this;
	}
}

namespace Nz
{
	template<typename T>
	bool Serialize(SerializationContext& context, ewn::CompressedSigned<T> value, TypeTag<ewn::CompressedSigned<T>>)
	{
		using UnsignedT = std::make_unsigned_t<T>;

		T signedValue = value;
		UnsignedT unsignedValue = reinterpret_cast<UnsignedT>(signedValue);

		// ZigZag encoding:
		// https://developers.google.com/protocol-buffers/docs/encoding
		unsignedValue = (unsignedValue << 1) ^ (unsignedValue >> (CHAR_BIT * sizeof(UnsignedT) - 1));

		return Serialize(context, ewn::CompressedUnsigned<UnsignedT>(unsignedValue));
	}

	template<typename T>
	bool Serialize(SerializationContext& context, ewn::CompressedUnsigned<T> value, TypeTag<ewn::CompressedUnsigned<T>>)
	{
		T integerValue = value;
		bool remaining;
		do 
		{
			Nz::UInt8 byteValue = integerValue & 0x7F;
			integerValue >>= 7;

			remaining = (integerValue > 0);
			if (remaining)
				byteValue |= 0x80;

			if (!Serialize(context, byteValue))
				return false;
		}
		while (remaining);

		return true;
	}

	template<typename T>
	bool Unserialize(SerializationContext& context, ewn::CompressedSigned<T>* value, TypeTag<ewn::CompressedSigned<T>>)
	{
		using UnsignedT = std::make_unsigned_t<T>;

		ewn::CompressedUnsigned<UnsignedT> compressedValue;
		if (!Unserialize(context, &compressedValue))
			return false;

		// ZigZag decoding:
		// https://developers.google.com/protocol-buffers/docs/encoding
		UnsignedT unsignedValue = compressedValue;
		unsignedValue = (unsignedValue >> 1) ^ (-(unsignedValue & 1));

		*value = reinterpret_cast<T>(unsignedValue);
		return true;
	}

	template<typename T>
	bool Unserialize(SerializationContext& context, ewn::CompressedUnsigned<T>* value, TypeTag<ewn::CompressedUnsigned<T>>)
	{
		T integerValue = 0;
		bool remaining;
		std::size_t i = 0;

		do
		{
			Nz::UInt8 byteValue;
			if (!Unserialize(context, &byteValue))
				return false;

			remaining = (byteValue & 0x80);
			if (remaining)
				byteValue &= ~Nz::UInt8(0x80);

			integerValue |= T(byteValue) << 7 * (i++);
		}
		while (remaining);

		*value = integerValue;

		return true;
	}
}
