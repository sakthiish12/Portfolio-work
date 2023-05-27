/******************************************************************************/
/*!
\file		Graphics_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Render graphics into ImGui

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"

#ifdef USE_EDITOR

#include "../../../BananaEngine/EngineCodes/Systems/Graphics/Offscreen.h"
namespace BE
{
	enum class GizmoType
	{
		ENTITY_GIZMO = 0,
		UI_GIZMO = 1
	};

	class Graphics_Window : public ImGuiWindow
	{
	private:
		static Graphics_Window* p_graph;

		static int gizmoType;
		static bool picking;
		static GizmoType gizmo_option;
		static int selected_type;
		VkDescriptorSet image{ VK_NULL_HANDLE };

		bool isFocused;
		bool isUsingGizmo;
		glm::vec2 windowPos{};
		glm::vec2 windowSize;
	public:
		Graphics_Window();
		~Graphics_Window();

		static inline Graphics_Window* Get() { return p_graph; }

		Graphics_Window* init();
		void run(BE::OffScreen* MOffScreen);
		void update() override;
		void Shutdown() override;

		static void SetPicking(bool _picking) { picking = _picking; }
		static bool GetPicking() { return picking; }
		void destroyImage(VkDescriptorSet& set);
		VkDescriptorSet& getImage();

		bool IsGraphicsWindowFocused() { return isFocused; }

		bool getIsUsingGizmo(){ return isUsingGizmo; }
		glm::vec2& getWindowPos();
		glm::vec2& getWindowSize();
	};
};

#endif
#endif