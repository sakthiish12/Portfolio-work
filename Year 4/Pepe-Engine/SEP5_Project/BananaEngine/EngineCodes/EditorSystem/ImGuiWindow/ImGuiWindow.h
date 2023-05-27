/******************************************************************************/
/*!
\file		ImguiWindow.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Interface for all ImGui windows

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef IMGUI_WINDOW_H
#define IMGUI_WINDOW_H

#include <Utilities/Utility.h>
#ifdef USE_EDITOR
namespace BE
{
	struct ImGuiWindow
	{
		ImGuiWindow() {};
		virtual ~ImGuiWindow() {};
		virtual void update() =0;
		virtual bool begin(std::string name);
		virtual bool begin(std::string name, bool* p_open);
		virtual void end();
		virtual void Shutdown() =0;
	};
};
#endif

#endif