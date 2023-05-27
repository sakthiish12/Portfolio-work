/******************************************************************************/
/*!
\file		Menu_Windows.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Main Menu options

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR

namespace BE
{
	class Menu_Window : public ImGuiWindow
	{
	private:
		static Menu_Window* p_menu;
	public:
		bool showConsoleWindow;
		bool showFPSWindow;
		bool showResourcesWindow;
		bool showEHierarWindow;
		bool showUHierarWindow;
		bool showEInspectWindow;
		bool showUInspectWindow;
		bool showSceneWindow;
		bool showEditorGraphicsWindow;
		bool showPlayerGraphicsWindow;
		bool showAssetsWindow;
		bool showEntityTagWindow;

		std::filesystem::path m_currentDir;
		static std::string filePath;

		Menu_Window();
		~Menu_Window();

		static inline Menu_Window* Get() { return p_menu; }

		Menu_Window* init();
		void update() override;
		void Shutdown() override;
	};
};

#endif
#endif