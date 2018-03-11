// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef EREWHON_SHARED_NETWORK_PACKETS_HPP
#define EREWHON_SHARED_NETWORK_PACKETS_HPP

#include <Shared/Enums.hpp>
#include <Shared/Protocol/CompressedInteger.hpp>
#include <Shared/Protocol/PacketSerializer.hpp>
#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <array>
#include <variant>
#include <vector>

namespace ewn
{
	enum class PacketType
	{
		ArenaState,
		BotMessage,
		ChatMessage,
		CreateSpaceship,
		ControlEntity,
		CreateEntity,
		DeleteEntity,
		DeleteSpaceship,
		IntegrityUpdate,
		JoinArena,
		Login,
		LoginFailure,
		LoginSuccess,
		PlayerChat,
		PlayerMovement,
		PlayerShoot,
		Register,
		RegisterFailure,
		RegisterSuccess,
		SpawnSpaceship,
		TimeSyncRequest,
		TimeSyncResponse
	};

	template<PacketType PT> struct PacketTag
	{
		static constexpr PacketType Type = PT;
	};

	namespace Packets
	{
#define DeclarePacket(Type) struct Type : PacketTag<PacketType:: Type >

		DeclarePacket(ArenaState)
		{
			struct Entity
			{
				CompressedUnsigned<Nz::UInt32> id;
				Nz::Vector3f angularVelocity;
				Nz::Vector3f linearVelocity;
				Nz::Vector3f position;
				Nz::Quaternionf rotation;
			};

			Nz::UInt16 stateId;
			CompressedUnsigned<Nz::UInt64> serverTime;
			CompressedUnsigned<Nz::UInt64> lastProcessedInputTime;
			std::vector<Entity> entities;
		};

		DeclarePacket(BotMessage)
		{
			BotMessageType messageType;
			std::string errorMessage;
		};

		DeclarePacket(ChatMessage)
		{
			std::string message;
		};

		DeclarePacket(ControlEntity)
		{
			CompressedUnsigned<Nz::UInt32> id;
		};

		DeclarePacket(CreateEntity)
		{
			CompressedUnsigned<Nz::UInt32> id;
			Nz::Vector3f angularVelocity;
			Nz::Vector3f linearVelocity;
			Nz::Vector3f position;
			Nz::Quaternionf rotation;
			Nz::String name;
			Nz::String entityType;
		};

		DeclarePacket(CreateSpaceship)
		{
			std::string spaceshipName;
			std::string code;
		};

		DeclarePacket(DeleteEntity)
		{
			CompressedUnsigned<Nz::UInt32> id;
		};

		DeclarePacket(DeleteSpaceship)
		{
			std::string spaceshipName;
		};

		DeclarePacket(IntegrityUpdate)
		{
			Nz::UInt8 integrityValue;
		};

		DeclarePacket(JoinArena)
		{
		};

		DeclarePacket(Login)
		{
			std::string login;
			std::string passwordHash;
		};

		DeclarePacket(LoginFailure)
		{
			LoginFailureReason reason;
		};

		DeclarePacket(LoginSuccess)
		{
		};

		DeclarePacket(PlayerChat)
		{
			std::string text;
		};

		DeclarePacket(PlayerMovement)
		{
			CompressedUnsigned<Nz::UInt64> inputTime; //< Server time
			Nz::Vector3f direction;
			Nz::Vector3f rotation;
		};

		DeclarePacket(PlayerShoot)
		{
		};

		DeclarePacket(Register)
		{
			std::string login;
			std::string email;
			std::string passwordHash;
		};

		DeclarePacket(RegisterFailure)
		{
			RegisterFailureReason reason;
		};

		DeclarePacket(RegisterSuccess)
		{
		};

		DeclarePacket(SpawnSpaceship)
		{
			std::string spaceshipName;
		};

		DeclarePacket(TimeSyncRequest)
		{
			Nz::UInt8 requestId;
		};

		DeclarePacket(TimeSyncResponse)
		{
			Nz::UInt8 requestId;
			CompressedUnsigned<Nz::UInt64> serverTime;
		};

#undef DeclarePacket

		void Serialize(PacketSerializer& serializer, ArenaState& data);
		void Serialize(PacketSerializer& serializer, BotMessage& data);
		void Serialize(PacketSerializer& serializer, ChatMessage& data);
		void Serialize(PacketSerializer& serializer, ControlEntity& data);
		void Serialize(PacketSerializer& serializer, CreateEntity& data);
		void Serialize(PacketSerializer& serializer, CreateSpaceship& data);
		void Serialize(PacketSerializer& serializer, DeleteEntity& data);
		void Serialize(PacketSerializer& serializer, DeleteSpaceship& data);
		void Serialize(PacketSerializer& serializer, IntegrityUpdate& data);
		void Serialize(PacketSerializer& serializer, JoinArena& data);
		void Serialize(PacketSerializer& serializer, Login& data);
		void Serialize(PacketSerializer& serializer, LoginFailure& data);
		void Serialize(PacketSerializer& serializer, LoginSuccess& data);
		void Serialize(PacketSerializer& serializer, PlayerChat& data);
		void Serialize(PacketSerializer& serializer, PlayerMovement& data);
		void Serialize(PacketSerializer& serializer, PlayerShoot& data);
		void Serialize(PacketSerializer& serializer, Register& data);
		void Serialize(PacketSerializer& serializer, RegisterFailure& data);
		void Serialize(PacketSerializer& serializer, RegisterSuccess& data);
		void Serialize(PacketSerializer& serializer, SpawnSpaceship& data);
		void Serialize(PacketSerializer& serializer, TimeSyncRequest& data);
		void Serialize(PacketSerializer& serializer, TimeSyncResponse& data);
	}
}

#include <Shared/Protocol/Packets.inl>

#endif // EREWHON_SHARED_NETWORK_PACKETS_HPP
