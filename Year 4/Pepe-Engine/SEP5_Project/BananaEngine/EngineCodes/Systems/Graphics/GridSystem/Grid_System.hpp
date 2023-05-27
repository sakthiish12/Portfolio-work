#pragma once
#include <Systems/Graphics/Be_device.hpp>
#include <Systems/Graphics/Be_pipeline.hpp>
#include <Systems/Graphics/Be_frame_info.hpp>

namespace BE
{
	class GridSystem {
	public:
		GridSystem();
		~GridSystem();

		GridSystem(const GridSystem&) = delete;
		GridSystem& operator=(const GridSystem&) = delete;

		void render(FrameInfo& frameInfo);
		void recreatePipeline(VkRenderPass renderPass);
		void setupSystem(BeDevice* device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		void Shutdown();
	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);


		BeDevice* engineDevice;

		std::unique_ptr<BePipeline> bePipeline;
		VkPipelineLayout pipelineLayout;
	};
}