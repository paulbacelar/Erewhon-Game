// Copyright (C) 2018 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <Shared/Protocol/NetworkStringStore.hpp>

namespace ewn
{
	void NetworkStringStore::FillStore(std::size_t firstId, std::vector<std::string> strings)
	{
		assert(firstId <= m_strings.size());

		// First, remove all strings with an id over firstId, if any
		for (std::size_t i = firstId; i < m_strings.size(); ++i)
			m_stringMap.erase(m_strings[i]);

		m_strings.erase(m_strings.begin() + firstId, m_strings.end());

		m_strings.reserve(m_strings.size() - firstId + strings.size());
		for (std::string& str : strings)
			RegisterString(std::move(str));
	}

	Packets::NetworkStrings NetworkStringStore::BuildPacket(std::size_t firstId) const
	{
		Packets::NetworkStrings packet;
		packet.startId = Nz::UInt32(firstId);
		packet.strings.reserve(m_strings.size() - firstId);
		for (std::size_t i = firstId; i < m_strings.size(); ++i)
			packet.strings.push_back(m_strings[i]);

		return packet;
	}
}
