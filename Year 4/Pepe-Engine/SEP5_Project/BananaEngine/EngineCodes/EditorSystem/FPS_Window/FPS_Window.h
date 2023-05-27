/******************************************************************************/
/*!
\file		FPS_Window.h
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
#ifndef FPS_WINDOW_H
#define FPS_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR
namespace BE
{
	class FPS_Window : public ImGuiWindow
	{
	private:
		static FPS_Window* p_fps;
		bool showLineGraph{};
		bool showDTLineGraph{};
		bool showPerformanceGraph{};

		std::stringstream ss;
		float Graphic_dt;
		float Audio_dt;
		float ECS_dt;
		float Animation_dt;
		float Input_dt;
		float Physics_dt;
		float Logic_dt;
		float Scene_dt;
		float UI_dt;
		float total_dt;
		
	public:
		std::vector<float> frameRate;
		std::vector<float> Graphic_rate;
		std::vector<float> Audio_rate;
		std::vector<float> ECS_rate;
		std::vector<float> Animation_rate;
		std::vector<float> Input_rate;
		std::vector<float> Physics_rate;
		std::vector<float> Logic_rate;
		std::vector<float> Scene_rate;
		std::vector<float> UI_rate;
		
		FPS_Window();
		~FPS_Window();

		static inline FPS_Window* Get() { return p_fps; }
		void Total_DT();								// Calculate total frame rate
		float DT_Percentage(float system_dt);			// Calculate how many percent each system uses
		void DT_LineGraph();							// Overall DT line graph
		void SystemLineGraph();							// Line Graph for each system
		void Set_var();									// Define variables
		void DumpData();								// Dump Profiler FPS Data
		void PerformanceGraph();						// Overall Bar Graph of all systems
		void UpdateVector();							// Update each system's vector

		FPS_Window* init();
		void update() override;
		void Shutdown() override;
	};
};

#endif
#endif