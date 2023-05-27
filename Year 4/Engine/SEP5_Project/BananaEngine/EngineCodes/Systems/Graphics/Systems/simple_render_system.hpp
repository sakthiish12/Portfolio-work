#pragma once

#include "../Camera/Be_camera.hpp"
#include "../Be_device.hpp"
#include "../Be_frame_info.hpp"
#include "../Be_pipeline.hpp"
#include "../../../AssetManager/AssetManager.h"
#include "../Be_descriptors.hpp"
#include "../../../EditorSystem/Graphics_Window/EditorCameraController.hpp"

namespace BE {
	struct CameraLocal;
	struct SimplePushConstantData {
		alignas(16) glm::mat4 modelMatrix{ 1.f };
		alignas(16) glm::mat4 normalMatrix{ 1.f };
		alignas(16) glm::vec4 flickerColor{};
		alignas(16) glm::vec4 wireframeColor{};
		alignas(4) float albedoStrength{};
		alignas(4) float aoStrength{};
		alignas(4) float emissiveStrength{};
		alignas(4) float metallicStrength{};
		alignas(4) float normalStrength{};
		alignas(4) float roughnessStrength{};
		alignas(4) float opacity{};
		alignas(4) float brightnessFactor{};
		alignas(4) int hasAnimShouldFlickerWireFrameModeIgnoreLighting{};
		alignas(4) int reflectSpecularCustomAlbedoCustomNormalCustomAO{};
		alignas(4) int customRoughnessCustomEmissiveCustomMetallicCustomHeight{};
		alignas(8) uint64_t id {};
	};
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem();
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void setupSystem(BeDevice* device, VkRenderPass, VkDescriptorSetLayout globalSetLayout);
		void renderGameObjects(FrameInfo& frameInfo, GlobalUbo&, std::vector<std::unique_ptr<BE::BeBuffer>>&, int, BeDescriptorSetLayout& setLayout,
			BeDescriptorPool& pool, CameraLocal* gameCamera, BeWindow* window, bool assetReloaded, bool globalWireframeMode);
		void Shutdown();
		uint64_t getSelectedID();
		float getSelectedDistance();

	private:
		void createPipelineLayouts(VkDescriptorSetLayout globalSetLayout);
		void createPipelines(VkRenderPass renderPass);
		void createBlankTextureImage();
		void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);
		void transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void setBool(int& storage, int index, bool value);
		bool getBool(int& storage, int index);

		SimplePushConstantData push;
		bool wasWireframe{};

		VkDescriptorImageInfo blankImageInfo;
		VkImage blankImage;
		VkImageView blankImageView;
		VkDeviceMemory blankImageMemories;
		VkSampler blankImageSampler;

		BeDevice* beDevice;

		std::unique_ptr<BePipeline> bePipeline;
		VkPipelineLayout pipelineLayout; //fill

		std::unique_ptr<BePipeline> bePipelineWireframe;
		VkPipelineLayout pipelineLayoutWireframe;

		//Mouse pick
			  //Mouse picking system
		struct MousePickingStorageBuffer {
			alignas(16) int64_t id;
		};

		struct Info {
			uint64_t selectedID = 0;
			float distance{};
		};
		std::unique_ptr<BE::BeBuffer> pickBuffer;
		Info pickerInfo{ 0,FLT_MAX };
	};
}  // namespace BE