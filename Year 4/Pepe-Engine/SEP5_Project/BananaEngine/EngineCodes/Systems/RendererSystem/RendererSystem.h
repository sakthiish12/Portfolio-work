#pragma once
#ifndef RENDERERSYSTEM_H
#define RENDERERSYSTEM_H
#include <Precompiled/pch.h>
#include <Utilities/Singleton/Singleton.h>
#include <Components/Transform.h>
#include <Components/CameraComponent.h>
#include "../ISystem.h"
#include "../Graphics/Offscreen.h"
#include "../Graphics/Be_descriptors.hpp"
#include "../Graphics/Be_renderer.hpp"
#include "../Graphics/Be_swap_chain.hpp"
#include "../Graphics/Be_buffer.hpp"
#include "../Graphics/Be_bone.h"
#include "../Graphics/Systems/simple_render_system.hpp"
#include "../Graphics/Systems/point_light_system.hpp"
#include "../Graphics/GridSystem/Grid_System.hpp"
#include <EditorSystem/Graphics_Window/EditorCameraController.hpp>
#include <EditorSystem/EditorSystem/EditorSystem.h>
#include "../../Systems/SceneManager/SceneManager.h"
#include "../Graphics/Systems/ui_render_system.hpp"
#include "../Graphics/Systems/text_render_system.hpp"
#include "../Graphics/Systems/particle_render_system.hpp"

namespace BE
{		
	struct CameraLocal
	{
		BE::Transform tf;
		BE::cameraComponent cc;
	};

	//This class goes through all animationComponent and updates the animation, calculates bone transformation and sets it there
	class RendererSystem : public Singleton<RendererSystem>, public ISystem
	{
	public:
		RendererSystem() = default;
		~RendererSystem() = default;

		BeDevice* getDevice();

		CameraLocal GameCamera;
		void setupRenderer(BeWindow&, BeDevice&, BeRenderer&, OffScreen&);
		void startRenderPass();
		void endRenderPass();
		inline bool getCanRender();
		void setCurrentTime(std::chrono::steady_clock::time_point t);
		uint64_t getSelectedID();
		GlobalUbo& getUBO();
		void flipEntitiesOneTime();

		bool& getGlobalWireframeMode();
		void setGlobalWireframeMode(bool b);
		bool& getShowGrid();
	private:
		BeWindow* beWindow{};
		BeDevice* beDevice{};
		BeRenderer* beRenderer{};
		OffScreen* MOffScreen{};

		std::unique_ptr<BE::BeDescriptorPool> globalPool{};
		std::unique_ptr<BE::BeDescriptorPool> texturedPool{};
		std::unique_ptr<BE::BeDescriptorPool> uiPool{};
		std::unique_ptr<BE::BeDescriptorPool> textPool{};
		std::unique_ptr<BE::BeDescriptorPool> ParticlePool{};
		std::unique_ptr<BE::BeDescriptorSetLayout> globalSetLayout{};
		std::unique_ptr<BE::BeDescriptorSetLayout> texturedSetLayout{};
		std::unique_ptr<BE::BeDescriptorSetLayout> uiSetLayout{};
		std::unique_ptr<BE::BeDescriptorSetLayout> textSetLayout{};
		std::unique_ptr<BE::BeDescriptorSetLayout> particleSetLayout{};
		std::vector<std::unique_ptr<BE::BeBuffer>> uboBuffers;
		std::vector<VkDescriptorSet> globalDescriptorSets;

		SimpleRenderSystem simpleRenderSystem;
		PointLightSystem pointLightSystem;
		GridSystem gridSystem;
		UIRenderSystem uiRenderSystem;
		TextRenderSystem textRenderSystem;
		ParticleRenderSystem particleRenderSystem;
#ifdef USE_EDITOR
		EditorCameraController cameraController;
#endif
		//BE::KeyboardMovementController cameraController{};

		VkCommandBuffer renderCommandBuffer{ VK_NULL_HANDLE };
		bool canRender{};
		float frameTime{};
		bool reloadedAssets{};
		bool globalWireframeMode{};
		bool showGrid{true};

		std::chrono::steady_clock::time_point currentTime{}, newTime{};
		GlobalUbo ubo; //Prephysics step
		ISYSTEM_FUNCTIONS;
		friend class Singleton<RendererSystem>;
	public:
		inline GLFWwindow* GetGLFWWindow() const { return beWindow->GetGLFWWindow(); }
		inline BeWindow* GetBEWindow() const { return beWindow; }
	};
#define Renderersystem RendererSystem::GetInstance()
}
#endif