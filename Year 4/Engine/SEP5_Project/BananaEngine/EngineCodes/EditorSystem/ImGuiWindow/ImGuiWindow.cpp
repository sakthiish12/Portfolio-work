/******************************************************************************/
/*!
\file		ImguiWindow.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Interface for all ImGui windows

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "ImGuiWindow.h"

#ifdef USE_EDITOR

namespace BE
{
	bool ImGuiWindow::begin(std::string name)
	{
		return ImGui::Begin(name.c_str());
	}

	bool ImGuiWindow::begin(std::string name, bool* p_open = (bool*)0)
	{
		return ImGui::Begin(name.c_str(), p_open);
	}

	void ImGuiWindow::end()
	{
		ImGui::End();
	}
}
#endif