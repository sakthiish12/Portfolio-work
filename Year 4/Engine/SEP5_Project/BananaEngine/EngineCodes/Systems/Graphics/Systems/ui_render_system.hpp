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

class UIRenderSystem {
	struct uiPushConstantData {
		alignas (16)	glm::vec4 posAndScale{0.0f,0.0f,0.0f,0.0f};
		alignas(16)		glm::vec4 color{1.0f,1.0f,1.0f,1.0f};
		alignas (4)		int textureToUse{};
		alignas (4)		float percentage{1.0f};
		alignas (4)		float flickerOpacity{1.0f};
		alignas(4)		float opacity{1.0f};
		alignas(4)		int isVertical{};
		alignas (4)		int uiType{};
		alignas(4)			float brightness{1.0f};
		alignas (4)		float rotationRad{ 0.0f };
		alignas(16)		glm::vec4 colorIntensityFactors{ 1.0f,1.0f,1.0f,1.0f };
	};
 public:
	 UIRenderSystem();
  ~UIRenderSystem();

  UIRenderSystem(const UIRenderSystem&) = delete;
  UIRenderSystem&operator=(const UIRenderSystem&) = delete;

  void setupSystem(BeDevice* device, VkRenderPass, VkDescriptorSetLayout globalSetLayout);
  void renderGameObjects(FrameInfo& frameInfo, int frameIndex, BeDescriptorSetLayout& setLayout,BeDescriptorPool& pool,CameraLocal* camera);
  void Shutdown();
  void setAllUIToRefreshTextures();
 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);
  void createBlankTextureImage();
  void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);
  void transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout);
  void createRectangleShape();
  void bindRectangle(VkCommandBuffer);
  void drawRectangle(VkCommandBuffer);

  std::vector<Vertex> vertices;
  std::unique_ptr<BeBuffer> vertexBuffer;
  std::vector<uint32_t> indices;
  std::unique_ptr<BeBuffer> indexBuffer;

  VkDescriptorImageInfo blankImageInfo;
  VkImage blankImage;
  VkImageView blankImageView;
  VkDeviceMemory blankImageMemories;
  VkSampler blankImageSampler;

  BeDevice *beDevice;

  std::unique_ptr<BePipeline> bePipeline;
  VkPipelineLayout pipelineLayout;
};
}  // namespace BE
