// /******************************************************************************/
// /*!
// \file         ImGuiHelpers.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Nov, 2021
// \brief        Declarations of specialised helpers for ImGui
// \lines        12 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef IMGUIHELPERS_H
#define IMGUIHELPERS_H

#include "../../pch.h"

//Please put all helpers in this namespace
namespace ImGuiHelpers
{
	//Helper to make a tooltip in just one line
	//TODO variadic if got time
	void tooltip(const char* text);

	//Helper to make a special ImGui float dragger
	//Can be inputted to have adaptive sensitivity or not
	//TODO make base sensitivity be variable by editor settings
	void ImGuiDragFloatSpecial(const char* label,
		float* val, float sensitivity = 0.01f, bool adaptiveSensitivity = true,
		float min = 0.0f, float max = 0.0f);

	//Tells ImGui to make the next elements' backgrounds be coloured as such
	//Provided base colour should be somewhat bright
	//FrameBg and FrameBgActive will be half as bright as FrameBgHovered
	//FrameBgHovered will be the same as the provided base colour
	//Remember to call ImGui::PopStyleColor(3) after applying colours on
	//needed elements
	void ImGuiPushFrameStyleColors(ImVec4 const& baseColor);

	//Similar to ImGuiPushFrameStyleColors but for buttons instead
	//Remember to call ImGui::PopStyleColor(3) after applying colours on
	//needed buttons
	void ImGuiPushButtonStyleColors(ImVec4 const& baseColor);
}
#endif