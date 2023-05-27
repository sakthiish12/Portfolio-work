/******************************************************************************/
/*!
\file		FPS_Window.cpp
\par        Project: Candy Crusade
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		FPS window to show all FPS data

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "FPS_Window.h"

#ifdef USE_EDITOR

#include "../Menu_Window/Menu_Window.h"
#include <BananaEngine.h>

namespace BE
{
	FPS_Window* FPS_Window::p_fps = nullptr;
	float* p_Graphics;
	float* p_Audio;
	float* p_ECS;
	float* p_Animation;
	float* p_Input;
	float* p_Physics;
	float* p_Logic;
	float* p_Scene;
	float* p_UI;

	int counter = 0;
	double getFrame = 0.0;

	FPS_Window::FPS_Window() : showLineGraph{ false }, Graphic_dt{ 0.00f },
								Audio_dt{0.00f}, ECS_dt{ 0.00f },
								Animation_dt{ 0.00f }, Input_dt{ 0.00f }, 
								Physics_dt{ 0.00f }, Logic_dt{ 0.0f },
								total_dt{ 0.00f }, Scene_dt{ 0.00f }, UI_dt{ 0.00f }
	{
	}							   

	FPS_Window::~FPS_Window()
	{
		
	}

	FPS_Window* FPS_Window::init()
	{
		p_fps = new FPS_Window();
		return p_fps;
	}

	void FPS_Window::update()
	{
		Set_var();

		if (Menu_Window::Get()->showFPSWindow)
		{
			if (!begin("FPS", &Menu_Window::Get()->showFPSWindow))
			{
				end();
			}
			else
			{
				DumpData();
				DT_LineGraph();
				PerformanceGraph();
				UpdateVector();
				SystemLineGraph(); //@donavern

				end();
			}
		}
	}

	void FPS_Window::Shutdown()
	{
		frameRate.clear();
		delete p_fps;
	}

	void FPS_Window::Total_DT()
	{
		total_dt = Graphic_dt + Audio_dt + ECS_dt + Animation_dt + Input_dt + Physics_dt + Logic_dt + Scene_dt + UI_dt;
	}

	float FPS_Window::DT_Percentage(float system_dt)
	{
		float Percent = system_dt / total_dt * 100;
		return Percent;
	}

	void FPS_Window::SystemLineGraph()
	{
		ImGui::Checkbox("Show Line Graphs", &showLineGraph);
		ImGui::Text("");

		if (showLineGraph)
		{
			p_Graphics = &Graphic_rate[0];
			ImGui::Text("Graphics: %f MS", Graphic_dt);
			ImGui::PlotLines("", p_Graphics, (int)Graphic_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Audio = &Audio_rate[0];
			ImGui::Text("Audio: %f MS", Audio_dt);
			ImGui::PlotLines("", p_Audio, (int)Audio_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_ECS = &ECS_rate[0];
			ImGui::Text("ECS: %f MS", ECS_dt);
			ImGui::PlotLines("", p_ECS, (int)ECS_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Animation = &Animation_rate[0];
			ImGui::Text("Animation: %f MS", Animation_dt);
			ImGui::PlotLines("", p_Animation, (int)Animation_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Input = &Input_rate[0];
			ImGui::Text("Input: %f MS", Input_dt);
			ImGui::PlotLines("", p_Input, (int)Input_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Physics = &Physics_rate[0];
			ImGui::Text("Physics: %f MS", Physics_dt);
			ImGui::PlotLines("", p_Physics, (int)Physics_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Logic = &Logic_rate[0];
			ImGui::Text("Game Logic: %f MS", Logic_dt);
			ImGui::PlotLines("", p_Logic, (int)Logic_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_Scene = &Scene_rate[0];
			ImGui::Text("Scene Manager: %f MS", Scene_dt);
			ImGui::PlotLines("", p_Scene, (int)Scene_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();

			p_UI = &UI_rate[0];
			ImGui::Text("UI: %f MS", UI_dt);
			ImGui::PlotLines("", p_UI, (int)UI_rate.size(), 0, "", 0.0f, 0.001f, ImVec2(200, 50));
			ImGui::NewLine();
		}
	}

	void FPS_Window::Set_var()
	{
		Graphic_dt = (float)BE::Renderersystem->GetSystemDt();
		Audio_dt = (float)BE::AudioMgr->GetSystemDt();
		ECS_dt = (float)BE::ECS->GetSystemDt();
		Animation_dt = (float)BE::Animationsystem->GetSystemDt();
		Input_dt = (float)BE::Input->GetSystemDt();
		Physics_dt = (float)BE::PhySys->GetSystemDt();
		Logic_dt = (float)BE::GLS->GetSystemDt();
		Scene_dt = (float)BE::SceneMgr->GetSystemDt();
		UI_dt = (float)BE::UI->GetSystemDt();

		Total_DT();

		ss << std::fixed << std::setprecision(4) << BE::Engine->GetCurrFrame();
		ss << "\t" << std::fixed << std::setprecision(4) << BE::Engine->GetDeltaTime();
		ss << "\t" << std::fixed << std::setprecision(8) << Graphic_dt ;
		ss << "\t" << std::fixed << std::setprecision(8) << Audio_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << ECS_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << Animation_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << Input_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << Physics_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << Logic_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << Scene_dt;
		ss << "\t" << std::fixed << std::setprecision(8) << UI_dt << std::endl;
	}

	void FPS_Window::DumpData()
	{
		if (ImGui::Button(ICON_FA_FILE_DOWNLOAD " Dump Profiler Data"))
		{
			std::ofstream out("../Banana_Profiler.log");
			out << "Profiler FPS Data" << std::endl;
			out << "" << std::endl;

			out << "System Index:" << std::endl;
			out << "0: Graphics" << std::endl;
			out << "1: Audio" << std::endl;
			out << "2: ECS" << std::endl;
			out << "3: Animation" << std::endl;
			out << "4: Input" << std::endl;
			out << "5: Physics" << std::endl;
			out << "6: Game Logic" << std::endl;
			out << "7: Scene Manager" << std::endl;
			out << "8: UI" << std::endl;

			out << "" << std::endl;
			out << "All System Speed are in MS" << std::endl;
			out << "Index\t FPS\t 0\t 1\t 2\t 3\t 4\t 5\t 6\t 7\t 8\t" << std::endl;
			out << ss.str() << std::endl;

			out.close();
		}

		ImGui::Separator();
	}

	void FPS_Window::DT_LineGraph()
	{
		if (counter < 50)
		{
			++counter;
		}

		if (counter == 50)
		{
			getFrame = BE::Engine->GetFramerate();
			counter = 0;
		}

		ImGui::Text("FPS: %f", getFrame);

		frameRate.push_back((float)BE::Engine->GetFramerate());

		ImGui::Checkbox("Show DT Line Graph", &showDTLineGraph);
		ImGui::Text("");

		if (showDTLineGraph)
		{
			float* arr = &frameRate[0];

			ImGui::PlotLines("", arr, (int)frameRate.size(), 0, "", 0.0f, 60.0f, ImVec2(200, 50));
			ImGui::Separator();
		}

	}

	void FPS_Window::PerformanceGraph()
	{
		ImGui::Checkbox("Show Performance Graph", &showPerformanceGraph);
		ImGui::Text("");
		if (showPerformanceGraph)
		{
			ImGui::Text("Performance Percentage");

			std::vector<float> Histo_graph = { Graphic_dt, Audio_dt, ECS_dt, Animation_dt, Input_dt, Physics_dt, Logic_dt, Scene_dt, UI_dt };
			float* arr_test = &Histo_graph[0];

			ImGui::PlotHistogram("", arr_test, (int)Histo_graph.size(), 0, "", 0.0f, 0.001f, ImVec2(250, 150)); //@donavern

			ImGui::Text("Index               |\tDT  \t|\tPercent");
			ImGui::Text("0: Graphics      |\t%.8f\t|\t%.2f", Graphic_dt, DT_Percentage(Graphic_dt));
			ImGui::Text("1: Audio            |\t%.8f\t|\t%.2f", Audio_dt, DT_Percentage(Audio_dt));
			ImGui::Text("2: ECS               |\t%.8f\t|\t%.2f", ECS_dt, DT_Percentage(ECS_dt));
			ImGui::Text("3: Animation	 |\t%.8f\t|\t%.2f", Animation_dt, DT_Percentage(Animation_dt));
			ImGui::Text("4: Input            |\t%.8f\t|\t%.2f", Input_dt, DT_Percentage(Input_dt));
			ImGui::Text("5: Physics         |\t%.8f\t|\t%.2f", Physics_dt, DT_Percentage(Physics_dt));
			ImGui::Text("6: Game Logic |\t%.8f\t|\t%.2f", Logic_dt, DT_Percentage(Logic_dt));
			ImGui::Text("7: Scene           |\t%.8f\t|\t%.2f", Scene_dt, DT_Percentage(Scene_dt));
			ImGui::Text("8: UI                 |\t%.8f\t|\t%.2f", UI_dt, DT_Percentage(UI_dt));
			ImGui::NewLine();
			ImGui::Separator();
		}
	}

	void FPS_Window::UpdateVector()
	{
		Graphic_rate.push_back(Graphic_dt);
		Audio_rate.push_back(Audio_dt);
		ECS_rate.push_back(ECS_dt);
		Animation_rate.push_back(Animation_dt);
		Input_rate.push_back(Input_dt);
		Physics_rate.push_back(Physics_dt);
		Logic_rate.push_back(Logic_dt);
		Scene_rate.push_back(Scene_dt);
		UI_rate.push_back(UI_dt);
	}
}
#endif