/******************************************************************************/
/*!
\file		UI_Hierachy_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Show all UI objects in the scene

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef UI_HIERARCHY_WINDOW_H
#define UI_HIERARCHY_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR

namespace BE
{
	enum class UI_FileStates
	{
		ogState,
		failState,
		successState,
	};

	class UI_Hierarchy_Window : public ImGuiWindow
	{
	private:
		static UI_Hierarchy_Window* p_ui_hier;

		bool inTextbox = false;

		UI_FileStates loadState = UI_FileStates::ogState;
		UI_FileStates saveState = UI_FileStates::ogState;

		static bool DrawUI;

	public:
		static BE::BEID canvasID;
		static BE::BEID ui_id;
		bool isusing;

		UI_Hierarchy_Window();
		~UI_Hierarchy_Window();

		static inline UI_Hierarchy_Window* Get() { return p_ui_hier; }

		static bool getDrawUI() { return DrawUI; }

		UI_Hierarchy_Window* init();
		void update() override;
		void Shutdown() override;
	};
};

#endif
#endif