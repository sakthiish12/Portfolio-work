#pragma once
#pragma warning (push)
#pragma warning (disable: 4505)
#include "../Be_device.hpp"
#include "../Be_frame_info.hpp"
#include "../Be_pipeline.hpp"
#include "../../../AssetManager/AssetManager.h"
#include "../Be_descriptors.hpp"
#include "../../Graphics/Font/Font.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace BE {
	struct TextPushConstantData
	{
		alignas (16) glm::mat4 projection{};
		alignas (16) glm::mat4 view{};
		alignas(16) glm::vec4 colorIntensityFactor{ 1.0f,1.0f,1.0f,1.0f };
		alignas (16)	glm::vec4 colorAndOpacity{1.0f,1.0f,1.0f,1.0f};
		alignas (4) int textType{}; //0 for screen, 1 for world
		alignas (4) float brightnessFactor{ 1.0f };
	};

	class TextRenderSystem
	{
	private:
		float offscreenPassWidth{}, offscreenPassHeight{};
		BeDevice* beDevice;
		std::unique_ptr<BePipeline> bePipeline;
		VkPipelineLayout pipelineLayout; //fill
		TextPushConstantData pushConstantData;

		void createPipelineLayouts(VkDescriptorSetLayout globalSetLayout);
		void createPipelines(VkRenderPass renderPass);
	public:

		TextRenderSystem();
		~TextRenderSystem();
		void setupSystem(
			BeDevice* vulkanDevice,
			VkRenderPass pass, VkDescriptorSetLayout globalSetLayout,std::unique_ptr<BeDescriptorPool>& pool, float width, float height);
		void renderText(FrameInfo& frameInfo,GlobalUbo& ubo);
		void Shutdown();

		//for Font
		VkImage blankImage{};
		VkDeviceMemory blankImageMemory{};
		VkSampler blankImageSampler{};
		VkImageView blankImageView{};
		VkDescriptorImageInfo blankImageInfo{};
		VkDescriptorSet blankImageDescriptorSet{};
		void createBlankTextureImage(VkDescriptorSetLayout globalSetLayout,
			std::unique_ptr<BeDescriptorPool>& pool);

		VkBuffer writeToBuffer{};
		VkDeviceMemory writeToMemory{};
		glm::vec4* mapped{};
		void createBufferToWriteTo();
		void mapBufferToWriteTo();
		void unmapBufferWritingTo();
		void startWritingToBuffer();

		float ConvertToClipSpace(float x,int i); //-1 to 1 from like 0 to 1280 or smth

		std::vector<VkDescriptorSet> texturesToUse; //This is per font
		VkDescriptorSet prevSet{};
		int numberOfLetters{};
	};
}
#pragma warning (pop)