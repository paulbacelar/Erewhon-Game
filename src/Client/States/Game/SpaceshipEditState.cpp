// Copyright (C) 2018 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <Client/States/Game/SpaceshipEditState.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/StateMachine.hpp>
#include <Shared/Protocol/Packets.hpp>
#include <Client/States/BackgroundState.hpp>
#include <Client/States/ConnectionLostState.hpp>
#include <Client/States/Game/MainMenuState.hpp>
#include <Client/States/Game/TimeSyncState.hpp>
#include <cassert>

namespace ewn
{
	void SpaceshipEditState::Enter(Ndk::StateMachine& /*fsm*/)
	{
		StateData& stateData = GetStateData();

		m_backButton = CreateWidget<Ndk::ButtonWidget>();
		m_backButton->SetPadding(15.f, 15.f, 15.f, 15.f);
		m_backButton->UpdateText(Nz::SimpleTextDrawer::Draw("Back", 24));
		m_backButton->ResizeToContent();
		m_backButton->OnButtonTrigger.Connect([&](const Ndk::ButtonWidget* /*button*/)
		{
			OnBackPressed();
		});

		m_updateButton = CreateWidget<Ndk::ButtonWidget>();
		m_updateButton->SetPadding(15.f, 15.f, 15.f, 15.f);
		m_updateButton->UpdateText(Nz::SimpleTextDrawer::Draw("Update", 24));
		m_updateButton->ResizeToContent();
		m_updateButton->OnButtonTrigger.Connect([&](const Ndk::ButtonWidget* /*button*/)
		{
			OnUpdatePressed();
		});

		m_nameLabel = CreateWidget<Ndk::LabelWidget>();
		m_nameLabel->UpdateText(Nz::SimpleTextDrawer::Draw("Spaceship name:", 24));
		m_nameLabel->ResizeToContent();

		m_nameTextArea = CreateWidget<Ndk::TextAreaWidget>();
		m_nameTextArea->SetContentSize({ 160.f, 30.f });
		m_nameTextArea->SetText(m_spaceshipName);
		m_nameTextArea->EnableBackground(true);
		m_nameTextArea->SetBackgroundColor(Nz::Color::White);
		m_nameTextArea->SetTextColor(Nz::Color::Black);

		m_statusLabel = CreateWidget<Ndk::LabelWidget>();
		m_titleLabel = CreateWidget<Ndk::LabelWidget>();

		m_light = stateData.world3D->CreateEntity();
		m_light->AddComponent<Ndk::LightComponent>(Nz::LightType_Spot);
		auto& lightNode = m_light->AddComponent<Ndk::NodeComponent>();
		lightNode.SetParent(stateData.camera3D);

		m_spaceship = stateData.world3D->CreateEntity();
		m_spaceship->AddComponent<Ndk::GraphicsComponent>();
		auto& spaceshipNode = m_spaceship->AddComponent<Ndk::NodeComponent>();
		spaceshipNode.SetParent(stateData.camera3D);
		spaceshipNode.SetPosition(Nz::Vector3f::Forward() * 2.f);

		LayoutWidgets();
		m_onTargetChangeSizeSlot.Connect(stateData.window->OnRenderTargetSizeChange, [this](const Nz::RenderTarget*) { LayoutWidgets(); });

		m_onUpdateSpaceshipFailureSlot.Connect(stateData.server->OnUpdateSpaceshipFailure, this, &SpaceshipEditState::OnUpdateSpaceshipFailure);
		m_onUpdateSpaceshipSuccessSlot.Connect(stateData.server->OnUpdateSpaceshipSuccess, this, &SpaceshipEditState::OnUpdateSpaceshipSuccess);
		m_onSpaceshipInfoSlot.Connect(stateData.server->OnSpaceshipInfo, this, &SpaceshipEditState::OnSpaceshipInfo);

		QuerySpaceshipInfo();
	}

	void SpaceshipEditState::Leave(Ndk::StateMachine& fsm)
	{
		AbstractState::Leave(fsm);

		m_light.Reset();
		m_spaceship.Reset();

		m_onSpaceshipInfoSlot.Disconnect();
		m_onTargetChangeSizeSlot.Disconnect();
	}

	bool SpaceshipEditState::Update(Ndk::StateMachine& fsm, float elapsedTime)
	{
		StateData& stateData = GetStateData();

		if (!stateData.server->IsConnected())
		{
			fsm.ChangeState(std::make_shared<ConnectionLostState>(stateData));
			return false;
		}

		if (m_nextState)
			fsm.ChangeState(m_nextState);

		m_spaceship->GetComponent<Ndk::NodeComponent>().Rotate(Nz::EulerAnglesf(0.f, 30.f * elapsedTime, 0.f));

		return true;
	}

	void SpaceshipEditState::LayoutWidgets()
	{
		Nz::Vector2f canvasSize = GetStateData().canvas->GetSize();

		m_backButton->SetPosition(20.f, canvasSize.y - m_backButton->GetSize().y - 20.f);

		m_statusLabel->CenterHorizontal();
		m_statusLabel->SetPosition(m_statusLabel->GetPosition().x, canvasSize.y * 0.2f);

		m_titleLabel->CenterHorizontal();
		m_titleLabel->SetPosition(m_titleLabel->GetPosition().x, canvasSize.y * 0.8f - m_titleLabel->GetSize().y / 2.f);

		float totalNameWidth = m_nameLabel->GetSize().x + 5.f + m_nameTextArea->GetSize().x;
		m_nameLabel->SetPosition(canvasSize.x / 2.f - totalNameWidth / 2.f, canvasSize.y * 0.8f - m_titleLabel->GetSize().y / 2.f);
		m_nameTextArea->SetPosition(canvasSize.x / 2.f - totalNameWidth / 2.f + m_nameLabel->GetSize().x, canvasSize.y * 0.8f - m_titleLabel->GetSize().y / 2.f);

		m_updateButton->CenterHorizontal();
		m_updateButton->SetPosition(m_updateButton->GetPosition().x, m_nameTextArea->GetPosition().y + m_nameTextArea->GetSize().y + 20.f);
	}

	void SpaceshipEditState::OnBackPressed()
	{
		m_nextState = m_previousState;
	}

	void SpaceshipEditState::OnUpdateSpaceshipFailure(ServerConnection* server, const Packets::UpdateSpaceshipFailure& updatePacket)
	{
		std::string reason;
		switch (updatePacket.reason)
		{
			case UpdateSpaceshipFailureReason::NotFound:
				reason = "spaceship not found";
				break;

			case UpdateSpaceshipFailureReason::ServerError:
				reason = "server error, please try again later";
				break;

			default:
				reason = "<packet error>";
				break;
		}

		UpdateStatus("Failed to update spaceship: " + reason, Nz::Color::Red);
	}

	void SpaceshipEditState::OnUpdateSpaceshipSuccess(ServerConnection* server, const Packets::UpdateSpaceshipSuccess& updatePacket)
	{
		UpdateStatus("Spaceship successfully updated", Nz::Color::Green);

		m_spaceshipName = m_nameTextArea->GetText().ToStdString();
	}

	void SpaceshipEditState::OnSpaceshipInfo(ServerConnection* server, const Packets::SpaceshipInfo& listPacket)
	{
		const std::string& assetsFolder = server->GetApp().GetConfig().GetStringOption("AssetsFolder");

		m_statusLabel->Show(false);
		//m_titleLabel->Show(true);

		m_titleLabel->UpdateText(Nz::SimpleTextDrawer::Draw("Spaceship " + m_spaceshipName + ":", 24));
		m_titleLabel->ResizeToContent();

		LayoutWidgets();

		auto& entityGfx = m_spaceship->GetComponent<Ndk::GraphicsComponent>();

		entityGfx.Clear();

		Nz::ModelParameters modelParams;
		modelParams.mesh.center = true;
		modelParams.mesh.texCoordScale.Set(1.f, -1.f);

		m_spaceshipModel = Nz::Model::New();
		if (!m_spaceshipModel->LoadFromFile(assetsFolder + '/' + listPacket.hullModelPath, modelParams))
		{
			UpdateStatus("Failed to load model", Nz::Color::Red);
			return;
		}

		float boundingRadius = m_spaceshipModel->GetBoundingVolume().obb.localBox.GetRadius();
		Nz::Matrix4f transformMatrix = Nz::Matrix4f::Scale(Nz::Vector3f::Unit() / boundingRadius);

		entityGfx.Attach(m_spaceshipModel, transformMatrix);
	}

	void SpaceshipEditState::OnUpdatePressed()
	{
		Packets::UpdateSpaceship updateSpaceship;
		updateSpaceship.spaceshipName = m_spaceshipName;
		updateSpaceship.newSpaceshipName = m_nameTextArea->GetText().ToStdString();

		GetStateData().server->SendPacket(updateSpaceship);
	}

	void SpaceshipEditState::QuerySpaceshipInfo()
	{
		m_titleLabel->Show(false);

		UpdateStatus("Loading " + m_spaceshipName + "...");

		Packets::QuerySpaceshipInfo packet;
		packet.spaceshipName = m_spaceshipName;

		GetStateData().server->SendPacket(std::move(packet));
	}

	void SpaceshipEditState::UpdateStatus(const Nz::String& status, const Nz::Color& color)
	{
		m_statusLabel->Show(true);
		m_statusLabel->UpdateText(Nz::SimpleTextDrawer::Draw(status, 24, 0U, color));
		m_statusLabel->ResizeToContent();

		LayoutWidgets();
	}
}
