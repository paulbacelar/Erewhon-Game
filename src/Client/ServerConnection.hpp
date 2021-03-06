// Copyright (C) 2018 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef EREWHON_CLIENT_SERVERCONNECTION_HPP
#define EREWHON_CLIENT_SERVERCONNECTION_HPP

#include <Shared/Protocol/NetworkStringStore.hpp>
#include <Shared/Protocol/Packets.hpp>
#include <Client/ClientCommandStore.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/String.hpp>

namespace ewn
{
	class ClientApplication;
	class ClientCommandStore;
	class NetworkReactor;

	class ServerConnection
	{
		friend ClientApplication;

		public:
			inline ServerConnection(ClientApplication& application);
			ServerConnection(const ServerConnection&) = delete;
			ServerConnection(ServerConnection&&) = delete;
			~ServerConnection() = default;

			bool Connect(const Nz::String& serverHostname, Nz::UInt32 data = 0);
			inline void Disconnect(Nz::UInt32 data = 0);

			Nz::UInt64 EstimateServerTime() const;

			inline ClientApplication& GetApp();
			inline const ClientApplication& GetApp() const;
			inline const NetworkStringStore& GetNetworkStringStore() const;

			inline bool IsConnected() const;
			
			template<typename T> void SendPacket(const T& packet);

			inline void UpdateServerTimeDelta(Nz::UInt64 deltaTime);

			ServerConnection& operator=(const ServerConnection&) = delete;
			ServerConnection& operator=(ServerConnection&&) = delete;

			NazaraSignal(OnConnected,    ServerConnection* /*server*/, Nz::UInt32 /*data*/);
			NazaraSignal(OnDisconnected, ServerConnection* /*server*/, Nz::UInt32 /*data*/);

			// Packet reception signals
			NazaraSignal(OnArenaPrefabs,           ServerConnection* /*server*/, const Packets::ArenaPrefabs&     /*data*/);
			NazaraSignal(OnArenaSounds,            ServerConnection* /*server*/, const Packets::ArenaSounds&      /*data*/);
			NazaraSignal(OnArenaState,             ServerConnection* /*server*/, const Packets::ArenaState&       /*data*/);
			NazaraSignal(OnBotMessage,             ServerConnection* /*server*/, const Packets::BotMessage&       /*data*/);
			NazaraSignal(OnChatMessage,            ServerConnection* /*server*/, const Packets::ChatMessage&      /*data*/);
			NazaraSignal(OnControlEntity,          ServerConnection* /*server*/, const Packets::ControlEntity&    /*data*/);
			NazaraSignal(OnCreateEntity,           ServerConnection* /*server*/, const Packets::CreateEntity&     /*data*/);
			NazaraSignal(OnDeleteEntity,           ServerConnection* /*server*/, const Packets::DeleteEntity&     /*data*/);
			NazaraSignal(OnIntegrityUpdate,        ServerConnection* /*server*/, const Packets::IntegrityUpdate&  /*data*/);
			NazaraSignal(OnLoginFailure,           ServerConnection* /*server*/, const Packets::LoginFailure&     /*data*/);
			NazaraSignal(OnLoginSuccess,           ServerConnection* /*server*/, const Packets::LoginSuccess&     /*data*/);
			NazaraSignal(OnNetworkStrings,         ServerConnection* /*server*/, const Packets::NetworkStrings&   /*data*/);
			NazaraSignal(OnPlaySound,              ServerConnection* /*server*/, const Packets::PlaySound&        /*data*/);
			NazaraSignal(OnRegisterFailure,        ServerConnection* /*server*/, const Packets::RegisterFailure&  /*data*/);
			NazaraSignal(OnRegisterSuccess,        ServerConnection* /*server*/, const Packets::RegisterSuccess&  /*data*/);
			NazaraSignal(OnSpaceshipInfo,          ServerConnection* /*server*/, const Packets::SpaceshipInfo&    /*data*/);
			NazaraSignal(OnSpaceshipList,          ServerConnection* /*server*/, const Packets::SpaceshipList&    /*data*/);
			NazaraSignal(OnTimeSyncResponse,       ServerConnection* /*server*/, const Packets::TimeSyncResponse& /*data*/);
			NazaraSignal(OnUpdateSpaceshipFailure, ServerConnection* /*server*/, const Packets::UpdateSpaceshipFailure& /*data*/);
			NazaraSignal(OnUpdateSpaceshipSuccess, ServerConnection* /*server*/, const Packets::UpdateSpaceshipSuccess& /*data*/);

		private:
			inline void DispatchIncomingPacket(Nz::NetPacket&& packet);
			inline void NotifyConnected(Nz::UInt32 data);
			inline void NotifyDisconnected(Nz::UInt32 data);

			void UpdateNetworkStrings(ServerConnection* server, const Packets::NetworkStrings& data);

			ClientApplication& m_application;
			ClientCommandStore m_commandStore;
			NetworkStringStore m_stringStore;
			NetworkReactor* m_networkReactor;
			Nz::UInt64 m_deltaTime;
			std::size_t m_peerId;
			bool m_connected;
	};
}

#include <Client/ServerConnection.inl>

#endif // EREWHON_CLIENT_SERVERCONNECTION_HPP
