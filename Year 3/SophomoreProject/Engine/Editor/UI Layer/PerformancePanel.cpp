// /******************************************************************************/
// /*!
// \file         PerformancePanel.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         8th Oct, 2021
// \brief        Drawing collision boundary using graphic debugging
// \lines        77 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include <chrono>
#include <filesystem>//file system 
#include "PerformancePanel.h"
#include "../../StopWatches/EngineTime.h"
//#include "../../EditorStateManager/gameStateManager.cpp"
//#include"../../EditorStateManager/EditorStates/gameState.h"
#include "../../ECS/ECS.h"
//#include "../../ECS/ECS.h"

void UiLayer::LoadPerFormancePanel(bool* p_open) {
	UNREFERENCED_PARAMETER(p_open);
	//float dt;
	//dt = static_cast<float>(gameStateManager::fixedDeltaTime);

	//dt=GetElapsedTime();
	ImGui::PushID("Metrics");
	ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiCond_Once);
	ImGui::Begin("Metrics##Window");
	{

		{
			// FPS Graph
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Frame##TabItem"))
			{
				ImGui::Unindent();

				static float nextUpdateTime = 0;
				static int updateRate = 60; // Default 60hz
				const float fps = 1.0f / Time::DeltaTime();

				// Apply cooldown
				if (Time::DeltaTime() > nextUpdateTime)
				{
					m_DeltaTimeList.push_back(fps);
					nextUpdateTime = Time::DeltaTime() + (1.0f / static_cast<float>(updateRate));
				}

				if (m_DeltaTimeList.size() > 50)
					m_DeltaTimeList.erase(m_DeltaTimeList.begin());

				const std::string overlay = std::to_string((int)m_DeltaTimeList.back());

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); ImGui::Spacing();
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / m_DeltaTimeList.back(), m_DeltaTimeList.back());
				ImGui::Separator();

				ImGui::Text("FPS Graph");
				ImGui::Spacing();

				//ImGui::SliderInt("Update Rate", &updateRate, 1, 60);

				//static float scaleMinMax[2]{ 1, 165 };
				//ImGui::SliderFloat2("Plot Min/Max", scaleMinMax, 1, 300);
				//int lastElement = m_DeltaTimeList.back();
					
				/*	ImGui::PlotLines("##FPS",
						m_DeltaTimeList.data(), static_cast<int>(m_DeltaTimeList.size()),
						0, overlay.c_str(), m_DeltaTimeList[0], m_DeltaTimeList[1], ImVec2(ImGui::GetWindowWidth(), 100));*/
				
				ImGui::Indent(); ImGui::Spacing(); ImGui::Separator();
				ImGui::TreePop(); ImGui::Spacing();
			}

			// ECS Graph
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("ECS##TabItem"))
			{
				ImGui::Unindent();

				ImGui::Text("Entities Count: %u", ecs.getEntityNames().size());
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("System CPU (%%)");
				ImGui::Spacing();
				ImGui::Spacing();

				static float nextUpdateTime = 0;
				static int updateRate = 60; // Default 60hz
				//ImGui::SliderInt("Update Rate", &updateRate, 1, 60);

				// Apply cooldown
				if (Time::UnscaledElapsedTime() > nextUpdateTime)
				{
					m_Metrics = g_imguiTestState.GetMetrics(); // Update metrics
					nextUpdateTime = Time::UnscaledElapsedTime() + (1.0f / static_cast<float>(updateRate)); // Apply cooldown					
				}
				
				// Total up the time
				float totalTime = 0;
				for (const auto& [name, timeContainer] : m_Metrics.systemExecutionTimings.GetLappedCategories())
				{
					// Average the tick times
					float averageTickTime = 0;
					for (const auto& timeTaken : timeContainer)
						averageTickTime += timeTaken;

					totalTime += averageTickTime;
				}

				// Display progress bars
				for (const auto& [name, timeContainer] : m_Metrics.systemExecutionTimings.GetLappedCategories())
				{
					// Average the tick times
					float averageTickTime = 0;
					for (const auto& timeTaken : timeContainer)
						averageTickTime += timeTaken;

					ImGui::Text("%s", name.c_str());
					ImGui::ProgressBar(averageTickTime / totalTime);
				}

				ImGui::Indent(); ImGui::Spacing(); ImGui::Separator();
				ImGui::TreePop(); ImGui::Spacing();
			}

		}
	}
	ImGui::End();
	ImGui::PopID();
}

