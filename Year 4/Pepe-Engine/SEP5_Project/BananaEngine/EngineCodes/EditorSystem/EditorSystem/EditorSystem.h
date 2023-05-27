/******************************************************************************/
/*!
\file		EditorSystem.h
\par        Project: 
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Editor Manager, calling all ImGui windows

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef EDITOR_SYSTEM_H
#define EDITOR_SYSTEM_H

#include <BananaEngine.h>
#ifdef USE_EDITOR

#include "../ImGuiWindow/ImGuiWindow.h"
#include "../Menu_Window/Menu_Window.h"
#include "../Console_Window/Console_Window.h"
#include "../Hierarchy_Window/Hierarchy_Window.h"
#include "../Inspector_Window/Inspector_Window.h"
#include "../Tool_Bar/Tool_Bar.h"
#include "../UndoSystem/UndoSystem.h"
#include "../EntityTag_Window/EntityTag_Window.h"
#include "../Resources_Window/Resources_Window.h"
#include "../Assets_Window/Assets_Window.h"
#include "../Graphics_Window/Graphics_Window.h"
#include "../FPS_Window/FPS_Window.h"
#include "../../Systems/ISystem.h"
#include "../UI_Hierarchy_Window/UI_Hierarchy_Window.h"
#include "../UI_Inspector_Window/UI_Inspector_Window.h"

namespace BE
{
	class EditorSystem : public BE::Singleton<EditorSystem>, public ISystem
	{
		friend class BE::Singleton<EditorSystem>;

	private:
		Menu_Window* menu;
		Console_Window* console;
		Hierarchy_Window* hierarchy;
		Inspector_Window* inspector;
		Tool_Bar* tool_bar;
		EntityTag_Window* entityTag;
		Resources_Window* resources;
		Assets_Window* assets;
		Graphics_Window* graphics;
		FPS_Window* fps;
		UI_Hierarchy_Window* UIhierarchy;
		UI_Inspector_Window* UIinspector;

		std::vector<ImGuiWindow*> windows;

		void ShowDockSpace(bool* p_open);
		void updateWindows();
	public:
		EditorSystem();
		EditorSystem(BE::BeWindow& window, BE::BeDevice& device, VkRenderPass renderPass, uint32_t imageCount);
		~EditorSystem();

		//void Init(BE::BeWindow& window, BE::BeDevice& device, VkRenderPass renderPass, uint32_t imageCount);
		void newFrame();
		void Example();

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		CommandManager* command;

		//Functions
		//void HandleMessage(PE::IMessage* msg);
		void setupSystem(BE::BeWindow& window, BE::BeDevice& beDevice, VkRenderPass renderPass, uint32_t imageCount);
		void render(VkCommandBuffer commandBuffer);
		void initWindows();
		void addWindow(ImGuiWindow* newWin);

		//used in graphics_windows
		BE::BeDevice* getDevice();
		VkDescriptorPool& getDescriptorPool();
		Graphics_Window* getGraphicsWindow();
		Tool_Bar* getToolBar();
		Hierarchy_Window* getHierarchyWindow();
	private:
		BE::BeDevice* beDevice;

		// We haven't yet covered descriptor pools in the tutorial series
		// so I'm just going to create one for just imgui and store it here for now.
		// maybe its preferred to have a separate descriptor pool for imgui anyway,
		// I haven't looked into imgui best practices at all.
		VkDescriptorPool descriptorPool;
		//Getters
		//inline static EditorSystem* GetInstance() { return pInstance; }
		//inline FPS_Window* Get_fps() { return fps; }

		//Creates an instance
		//static EditorSystem* CreateInstance();

		//Disabling copy constructor/assignment
		//EditorSystem& operator=(const EditorSystem&) = delete;
		//EditorSystem(const EditorSystem&) = delete;
		ISYSTEM_FUNCTIONS;
	};
#define Editorsystem EditorSystem::GetInstance()
};

#endif
#endif