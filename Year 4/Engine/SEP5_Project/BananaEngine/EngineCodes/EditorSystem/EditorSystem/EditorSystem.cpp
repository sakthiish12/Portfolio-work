/******************************************************************************/
/*!
\file		EditorSystem.cpp
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
#include "pch.h"
#include "EditorSystem.h"
#ifdef USE_EDITOR

#include"../UndoSystem/UndoSystem.h"
#include "../../../BananaEngine/EngineCodes/Systems/Graphics/Be_device.hpp"
#include "../../../BananaEngine/EngineCodes/Systems/Window/Window.h"

#pragma warning (push)
#pragma warning (disable:26812)
namespace BE {

	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}
}
namespace BE
{
	EditorSystem::EditorSystem()
	{
	}

	void EditorSystem::Init() {};

	void EditorSystem::Awake() {};

	void EditorSystem::Reload() {};

	EditorSystem::EditorSystem(BE::BeWindow& window, BE::BeDevice& beDevice, VkRenderPass renderPass, uint32_t imageCount) :
		menu{ nullptr }, console{ nullptr }, hierarchy{ nullptr },
		inspector{ nullptr }, tool_bar{ nullptr }, entityTag{ nullptr }, resources{ nullptr }, 
		assets{ nullptr }, graphics{ nullptr }, fps{ nullptr }, beDevice{ &beDevice }, UIhierarchy{ nullptr }, UIinspector{ nullptr }
	{
		command = new CommandManager();
	VkDescriptorPoolSize pool_sizes[] = {
	 {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
	 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
	 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
	 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
	 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
	 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
	 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
	 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
	 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
	 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
	 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		//pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		if (vkCreateDescriptorPool(beDevice.device(), &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up descriptor pool for imgui");
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.IniFilename = "../../../../GameImGuiData.ini";

		io.Fonts->AddFontFromFileTTF("../ImguiStuff/IconFont/ReadexPro-VariableFont_wght.ttf", 14);


		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("../ImguiStuff/IconFont/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		 //ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		// Initialize imgui for vulkan
		
		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = beDevice.getInstance();
		init_info.PhysicalDevice = beDevice.getPhysicalDevice();
		init_info.Device = beDevice.device();
		init_info.QueueFamily = beDevice.getGraphicsQueueFamily();
		init_info.Queue = beDevice.graphicsQueue();

		// pipeline cache is a potential future optimization, ignoring for now
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool;
		// todo, I should probably get around to integrating a memory allocator library such as Vulkan
		// memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
		// in a ton of locations.
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = 2;
		init_info.ImageCount = imageCount;
		init_info.CheckVkResultFn = BE::check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		// upload fonts, this is done by recording and submitting a one time use command buffer
		// which can be done easily bye using some existing helper functions on the lve device object
		auto commandBuffer = beDevice.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		beDevice.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		initWindows();
		LogEditor("Created Editor System");
	}

	EditorSystem::~EditorSystem()
	{
	}
	void EditorSystem::newFrame() 
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorSystem::Example() 
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
		// browse its code to learn more about Dear ImGui!).
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named
		// window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

			ImGui::Text(
				"This is some useful text.");  // Display some text (you can use a format strings too)
			ImGui::Checkbox(
				"Demo Window",
				&show_demo_window);  // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color",
				(float*)&clear_color);  // Edit 3 floats representing a color

			if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true
										  // when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window) {
			ImGui::Begin(
				"Another Window",
				&show_another_window);  // Pass a pointer to our bool variable (the window will have a
										// closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me")) show_another_window = false;
			ImGui::End();
		}
	}

	void EditorSystem::Update(double)
	{
		//bool show = true;

		//// Start the Dear ImGui frame
		//ImGui_ImplVulkan_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//ShowDockSpace(&show);
		//updateWindows();

		//// Rendering
		//ImGui::Render();
		
		//bool show = true;

		//// Start the Dear ImGui frame
		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//ImGuizmo::BeginFrame();

		//ImGuiIO& io = ImGui::GetIO();

		//ShowDockSpace(&show);
		//updateWindows();

		//// Rendering
		//ImGui::Render();

		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	GLFWwindow* backup_current_context = glfwGetCurrentContext();
		//	ImGui::UpdatePlatformWindows();
		//	ImGui::RenderPlatformWindowsDefault();
		//	glfwMakeContextCurrent(backup_current_context);
		//}

		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		////glfwSwapBuffers(SH::GLFW->GetWindowHandler());
		////glfwPollEvents();
		////ImGui::UpdatePlatformWindows();
		////ImGui::RenderPlatformWindowsDefault();

		////SH::ToggleIMGUIPausing togglePause(true);
		////SH::Engine->DispatchMessage(&togglePause);
	}

	void EditorSystem::Shutdown()
	{
		for (auto i = 0; i < windows.size(); i++)
		{
			windows[i]->Shutdown();
		}
		vkDestroyDescriptorPool(beDevice->device(), descriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		delete command;
		LogEditor("Destroyed Editor System");
	}

	/*void EditorSystem::HandleMessage(SH::IMessage* msg)
	{
		(void)msg;
	}*/

	void EditorSystem::ShowDockSpace(bool* p_open)
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("###DockSpace", p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else
		{
			//ShowDockingDisabledMessage();
		}

		ImGui::End();
	}

	void EditorSystem::setupSystem(BE::BeWindow& window, BE::BeDevice& device, VkRenderPass renderPass, uint32_t imageCount)
	{
		beDevice = &device;
		command = new CommandManager();
		VkDescriptorPoolSize pool_sizes[] = {
		 {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		//pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		if (vkCreateDescriptorPool(beDevice->device(), &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up descriptor pool for imgui");
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.IniFilename = "../BananaEngine/GameImGuiData.ini";

		io.Fonts->AddFontFromFileTTF("../BananaEngine/EngineCodes/EditorSystem/ImguiStuff/IconFont/ReadexPro-VariableFont_wght.ttf", 14);

		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("../BananaEngine/EngineCodes/EditorSystem/ImguiStuff/IconFont/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

	   // Setup Platform/Renderer backends
	   // Initialize imgui for vulkan

		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = beDevice->getInstance();
		init_info.PhysicalDevice = beDevice->getPhysicalDevice();
		init_info.Device = beDevice->device();
		init_info.QueueFamily = beDevice->getGraphicsQueueFamily();
		init_info.Queue = beDevice->graphicsQueue();

		// pipeline cache is a potential future optimization, ignoring for now
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool;
		// todo, I should probably get around to integrating a memory allocator library such as Vulkan
		// memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
		// in a ton of locations.
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = 2;
		init_info.ImageCount = imageCount;
		init_info.CheckVkResultFn = BE::check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		// upload fonts, this is done by recording and submitting a one time use command buffer
		// which can be done easily bye using some existing helper functions on the lve device object
		auto commandBuffer = beDevice->beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		beDevice->endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		initWindows();
		LogEditor("Created Editor System");
	}

	void EditorSystem::render(VkCommandBuffer commandBuffer)
	{
		bool show = true;
		ShowDockSpace(&show);
		updateWindows();
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
	}

	void EditorSystem::initWindows()
	{
		menu = Menu_Window::Get()->init();
		addWindow(Menu_Window::Get());

		console = Console_Window::Get()->init();
		addWindow(Console_Window::Get());

		hierarchy = Hierarchy_Window::Get()->init();
		addWindow(Hierarchy_Window::Get());

		inspector = Inspector_Window::Get()->init();
		addWindow(Inspector_Window::Get());

		tool_bar = Tool_Bar::Get()->init();
		addWindow(Tool_Bar::Get());

		entityTag = EntityTag_Window::Get()->init();
		addWindow(EntityTag_Window::Get());

		resources = Resources_Window::Get()->init();
		addWindow(Resources_Window::Get());

		assets = Assets_Window::Get()->init();
		addWindow(Assets_Window::Get());

		graphics = Graphics_Window::Get()->init();
		addWindow(Graphics_Window::Get());

		fps = FPS_Window::Get()->init();
		addWindow(FPS_Window::Get());

		UIhierarchy = UI_Hierarchy_Window::Get()->init();
		addWindow(UI_Hierarchy_Window::Get());

		UIinspector = UI_Inspector_Window::Get()->init();
		addWindow(UI_Inspector_Window::Get());
	}

	void EditorSystem::updateWindows()
	{
		for (auto i = 0; i < windows.size(); i++)
		{
			windows[i]->update();
		}
	}

	void EditorSystem::addWindow(ImGuiWindow* newWin)
	{
		if (newWin)
		{
			windows.push_back(newWin);
		}
	}

	BE::BeDevice* EditorSystem::getDevice()
	{
		return beDevice;
	}

	VkDescriptorPool& EditorSystem::getDescriptorPool()
	{
		return descriptorPool;
	}
	Graphics_Window* EditorSystem::getGraphicsWindow()
	{
		return graphics;
	}
	Tool_Bar* EditorSystem::getToolBar()
	{
		return tool_bar;
	}
	Hierarchy_Window* EditorSystem::getHierarchyWindow()
	{
		return hierarchy;
	}
}
#pragma warning (pop)
#endif