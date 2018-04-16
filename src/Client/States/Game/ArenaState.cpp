// Copyright (C) 2018 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <Client/States/Game/ArenaState.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Graphics/ParticleFunctionController.hpp>
#include <Nazara/Graphics/ParticleFunctionRenderer.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Graphics/ParticleStruct.hpp>
#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/CollisionComponent3D.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/StateMachine.hpp>
#include <Client/States/BackgroundState.hpp>
#include <Client/States/ConnectionLostState.hpp>
#include <Client/States/Game/EscapeMenuState.hpp>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

namespace ewn
{
	void ArenaState::Enter(Ndk::StateMachine& /*fsm*/)
	{
		StateData& stateData = GetStateData();

		m_isEnteringMenu = false;

		if (Nz::Texture* background = Nz::TextureLibrary::Get("Background"); background && background->IsValid())
			stateData.world3D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::SkyboxBackground::New(background));
		else
			stateData.world3D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color::Black));

		m_controlledEntity = std::numeric_limits<decltype(m_controlledEntity)>::max();
		m_isDisconnected = !stateData.server->IsConnected();

		m_onControlEntitySlot.Connect(stateData.server->OnControlEntity, this, &ArenaState::OnControlEntity);
		m_onKeyPressedSlot.Connect(stateData.window->GetEventHandler().OnKeyPressed, this, &ArenaState::OnKeyPressed);

		m_chatbox.emplace(stateData.server, *stateData.window, stateData.canvas);
		m_matchEntities.emplace(stateData.app, stateData.server, stateData.world3D);
		m_onEntityCreatedSlot.Connect(m_matchEntities->OnEntityCreated, this, &ArenaState::OnEntityCreated);
		m_onEntityDeletionSlot.Connect(m_matchEntities->OnEntityDelete, this, &ArenaState::OnEntityDelete);

		Packets::JoinArena arenaPacket;
		arenaPacket.arenaIndex = 0;

		stateData.server->SendPacket(arenaPacket);
	}

	void ArenaState::Leave(Ndk::StateMachine& fsm)
	{
		AbstractState::Leave(fsm);

		m_onControlEntitySlot.Disconnect();
		m_onKeyPressedSlot.Disconnect();

		m_spaceshipController.reset();
		m_matchEntities.reset();
	}

	bool ArenaState::Update(Ndk::StateMachine& fsm, float elapsedTime)
	{
		StateData& stateData = GetStateData();

		if (!stateData.server->IsConnected())
		{
			fsm.ResetState(std::make_shared<BackgroundState>(stateData));
			fsm.PushState(std::make_shared<ConnectionLostState>(stateData));
			return false;
		}

		if (m_isEnteringMenu)
		{
			if (fsm.IsTopState(this))
				fsm.PushState(std::make_shared<EscapeMenuState>(stateData));

			m_isEnteringMenu = false;
		}

		m_matchEntities->Update(elapsedTime);
		if (m_spaceshipController)
			m_spaceshipController->Update(elapsedTime);

		auto& cameraNode = stateData.camera3D->GetComponent<Ndk::NodeComponent>();
		Nz::Quaternionf camRot = cameraNode.GetRotation();

		for (std::size_t i = 0; i < m_matchEntities->GetServerEntityCount(); ++i)
		{
			if (!m_matchEntities->IsServerEntityValid(i))
				continue;

			const ServerMatchEntities::ServerEntity& entityData = m_matchEntities->GetServerEntity(i);

			auto& spaceshipNode = entityData.entity->GetComponent<Ndk::NodeComponent>();

			// Update text position
			if (entityData.textEntity)
			{
				auto& textGfx = entityData.textEntity->GetComponent<Ndk::GraphicsComponent>();
				auto& textNode = entityData.textEntity->GetComponent<Ndk::NodeComponent>();
				textNode.SetPosition(spaceshipNode.GetPosition() + camRot * Nz::Vector3f::Up() * 6.f + Nz::Vector3f::Right() * textGfx.GetBoundingVolume().obb.localBox.width / 2.f);
				textNode.SetRotation(cameraNode.GetRotation());
			}
		}

		return true;
	}

	void ArenaState::ControlEntity(std::size_t entityId)
	{
		StateData& stateData = GetStateData();

		if (m_controlledEntity != entityId && m_controlledEntity != std::numeric_limits<std::size_t>::max())
		{
			if (m_matchEntities->IsServerEntityValid(m_controlledEntity))
			{
				auto& oldData = m_matchEntities->GetServerEntity(m_controlledEntity);
				if (oldData.textEntity)
					oldData.textEntity->Enable();
			}

			m_spaceshipController.reset();
		}

		if (entityId != std::numeric_limits<std::size_t>::max() && m_matchEntities->IsServerEntityValid(entityId))
		{
			auto& data = m_matchEntities->GetServerEntity(entityId);

			// Don't show our own name
			if (data.textEntity)
				data.textEntity->Disable();

			m_spaceshipController.emplace(stateData.app, stateData.server, *stateData.window, *stateData.world2D, *m_chatbox, *m_matchEntities, stateData.camera3D, data.entity);
		}

		m_controlledEntity = entityId;
	}

	void ArenaState::OnControlEntity(ServerConnection*, const Packets::ControlEntity& controlPacket)
	{
		ControlEntity((controlPacket.id != 0) ? controlPacket.id : std::numeric_limits<std::size_t>::max());
	}

	void ArenaState::OnEntityCreated(ServerMatchEntities* /*entities*/, ServerMatchEntities::ServerEntity& entityData)
	{
		if (entityData.serverId == m_controlledEntity)
			ControlEntity(m_controlledEntity);
	}

	void ArenaState::OnEntityDelete(ServerMatchEntities* entities, ServerMatchEntities::ServerEntity & entityData)
	{
		if (entityData.serverId == m_controlledEntity)
			ControlEntity(std::numeric_limits<std::size_t>::max());
	}

	void ArenaState::OnKeyPressed(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.code == Nz::Keyboard::F1)
		{
			m_matchEntities->EnableSnapshotHandling(!m_matchEntities->IsSnapshotHandlingEnabled());
			if (m_matchEntities->IsSnapshotHandlingEnabled())
				m_chatbox->PrintMessage("INFO: Sync enabled");
			else
				m_chatbox->PrintMessage("INFO: Sync disabled");
		}
		else if (event.code == Nz::Keyboard::Escape)
			m_isEnteringMenu = true;
	}
}
