/******************************************************************************/
/*!
\file       Tool_Bar.cpp
\par        Project:
\author     Pham Thao Mien
\date       12 November 2021
\brief      This header file contains the handling of changing of scene state
			when the tool bar functions are being clicked.

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Tool_Bar.h"

#ifdef USE_EDITOR

#include "../EditorSystem/EditorSystem.h"

namespace BE
{
	Tool_Bar* Tool_Bar::p_toolbar = nullptr;

	Tool_Bar::Tool_Bar() : m_scenestate{ SceneState::Edit }, playButton{ false }, pauseButton{ false }
	{
	}
	Tool_Bar::~Tool_Bar() {}

	//void Tool_Bar::SetSceneState(SceneState state)
	//{
	//	m_scenestate = state;
	//	if (m_scenestate == SceneState::Pause)
	//		button = play;
	//	else
	//		button = pause;
	//}

	void Tool_Bar::resetButtons()
	{
		playButton = false;
		pauseButton = false;
	}

	Tool_Bar* Tool_Bar::init()
	{
		p_toolbar = new Tool_Bar();
		return p_toolbar;
	}

	void Tool_Bar::update()
	{
		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration |
										   ImGuiWindowFlags_NoScrollbar |
										   ImGuiWindowFlags_NoScrollWithMouse);

		float size = 40.0f;
		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		auto MidPoint = ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x / 2.0f);
		auto StartCursorPosition = MidPoint - (2 * (50 + ImGui::GetStyle().ItemSpacing.x));
		ImGui::SetCursorScreenPos({ StartCursorPosition, ImGui::GetCursorScreenPos().y });

		if (ImGui::Button(ICON_FA_SAVE, ImVec2(size, size)))
		{
			//ImGui::SetTooltip
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("SAVE");

		ImGui::SameLine();

		if ((ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL)) && ImGui::IsKeyPressed(GLFW_KEY_Z))
		{
			UI_Hierarchy_Window::ui_id = BE::BEID(0);

			EditorSystem::GetInstance()->command->undo();
		}

		if (ImGui::Button(ICON_FA_UNDO, ImVec2(size, size)))
		{
			UI_Hierarchy_Window::ui_id = BE::BEID(0);

			EditorSystem::GetInstance()->command->undo();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("UNDO");

		ImGui::SameLine();

		if (!playButton)
		{
			//Going into play state
			if (ImGui::Button(ICON_FA_PLAY, ImVec2(size, size)))
			{
				UI_Hierarchy_Window::ui_id = BE::BEID(0);

				BE::Hierarchy_Window::id = (EntityID)0;
				BE::Hierarchy_Window::Get_Selected().clear();

				playButton = !playButton;

				if (playButton && !pauseButton)
				{
					m_scenestate = SceneState::Play;
					BE::Engine->Awake();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, false);
				}
				/*else if (!playButton && !pauseButton)
				{
					m_scenestate = SceneState::Edit;
					SceneManager::GetInstance()->RestartScene();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
				}*/
				else if (playButton && pauseButton)
				{
					m_scenestate = SceneState::Pause;
					BE::Engine->Awake();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, false);
				}
				/*else if (!playButton && pauseButton)
				{
					m_scenestate = SceneState::Edit;
					SceneManager::GetInstance()->RestartScene();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
				}*/
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("PLAY");
		}
		else
		{
			if (ImGui::Button(ICON_FA_SQUARE, ImVec2(size, size)))
			{
				UI_Hierarchy_Window::ui_id = BE::BEID(0);

				BE::Hierarchy_Window::id = (EntityID)0;
				BE::Hierarchy_Window::Get_Selected().clear();

				playButton = !playButton;

				/*if (playButton && !pauseButton)
				{
					m_scenestate = SceneState::Play;
					BE::Engine->Awake();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, false);
				}
				else*/ if (!playButton && !pauseButton)
				{
					m_scenestate = SceneState::Edit;
					SceneManager::GetInstance()->RestartScene();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
				}
				/*else if (playButton && pauseButton)
				{
					m_scenestate = SceneState::Pause;
					BE::Engine->Awake();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, false);
				}*/
				else if (!playButton && pauseButton)
				{
					m_scenestate = SceneState::Edit;
					SceneManager::GetInstance()->RestartScene();
					BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
				}
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("STOP");
		}
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(size, size)))
		{
			UI_Hierarchy_Window::ui_id = BE::BEID(0);

			BE::Hierarchy_Window::id = (EntityID)0;
			BE::Hierarchy_Window::Get_Selected().clear();

			pauseButton = !pauseButton;

			if (playButton && pauseButton)
			{
				m_scenestate = SceneState::Pause;
			}
			else if (playButton && !pauseButton)
			{
				m_scenestate = SceneState::Play;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("PAUSE");

		ImGui::End();
	}

	void Tool_Bar::Shutdown()
	{
		delete p_toolbar;
	}
}
#endif