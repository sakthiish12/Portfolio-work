// /******************************************************************************/
// /*!
// \file         ImGuiHelpers.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Nov, 2021
// \brief        Definitions of specialised helpers for ImGui
// \lines        51 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../../pch.h"
#include "ImGuiHelpers.h"

namespace ImGuiHelpers
{
	void tooltip(const char* text)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text(text);
			ImGui::EndTooltip();
		}
	}

	void ImGuiDragFloatSpecial(const char* label,
		float* val, float sensitivity, bool adaptiveSensitivity,
		float min, float max)
	{
		//The further the bar has been dragged,
		//the faster the dragging will get.
		static float distanceDragged{ 0.0f };

		//For recording the initial value of a dragger before dragging
		static float initialDragValue{ 0.0f };

		if (adaptiveSensitivity)
			ImGui::DragFloat(label,
				val, abs(distanceDragged * sensitivity) + sensitivity, min, max);
		else
			ImGui::DragFloat(label,
				val, sensitivity, min, max);

		//Start dragging, set initial drag value
		if (ImGui::IsItemActivated())
		{
			initialDragValue = *val;
			distanceDragged = 0.0f;
		}
		//While dragging
		if (ImGui::IsItemActive())
		{
			//Tooltip showing change
			distanceDragged = *val - initialDragValue;
			ImGui::BeginTooltip();
			ImGui::Text("%.3f -> %.3f", initialDragValue, *val);
			ImGui::Text("Delta: %.3f", distanceDragged);
			ImGui::EndTooltip();

			//In case the dragging goes beyond the range, wrap
			if (*val >= max && min != max) *val = min + sensitivity;
			if (*val <= min && min != max) *val = max - sensitivity;
		}
		//Let go, is reset
		if (ImGui::IsItemDeactivated())
		{
			distanceDragged = 0.0f; //Reset
		}
	}

	void ImGuiPushFrameStyleColors(ImVec4 const& baseColor)
	{
		ImVec4 halfColor{ baseColor.x * 0.5f, baseColor.y * 0.5f, baseColor.z * 0.5f, baseColor.w };
		ImGui::PushStyleColor(ImGuiCol_FrameBg, halfColor); //Red
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, baseColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, halfColor);
	}

	void ImGuiPushButtonStyleColors(ImVec4 const& baseColor)
	{
		ImVec4 halfColor{ baseColor.x * 0.5f, baseColor.y * 0.5f, baseColor.z * 0.5f, baseColor.w };
		ImGui::PushStyleColor(ImGuiCol_Button, halfColor); //Red
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, baseColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, halfColor);
	}
}