#pragma once



#include "../Camera/Be_camera.hpp"
#include "../Be_device.hpp"
#include "../Be_frame_info.hpp"
#include "../Be_pipeline.hpp"
#include "../Be_descriptors.hpp"
#include "../../../Components/ParticleSystem.h"
#include "../../../AssetManager/AssetManager.h"
#include <AssetManager/AssetTypes/Be_model.hpp>
#include <vector>
//#include <array>

constexpr float MAX_PARTICLES = 128;

namespace BE {
	struct CameraLocal;
	class ParticleRenderSystem {
		//push constant
		struct ParticlePushConstant {
			alignas(16) glm::mat4 modelMatrix{ 1.f };
			alignas(16) glm::mat4 normalMatrix{ 1.f };
			alignas(16) glm::vec4 pos{ 0.0f, 0.0f, 0.0f, 0.0f };
			alignas(16) glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
			alignas(4)  float size{};
		};

	public:
		

		ParticleRenderSystem();
		~ParticleRenderSystem();

		ParticleRenderSystem(const ParticleRenderSystem&) = delete;
		ParticleRenderSystem& operator=(const ParticleRenderSystem&) = delete;

		void setupSystem(BeDevice* device, VkRenderPass, VkDescriptorSetLayout globalSetLayout);
		void createBlankTextureImage();
		//void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);
		void transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);
		void renderGameObjects(FrameInfo& frameInfo, int frameIndex, BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, const CameraLocal& camera);
		void Shutdown();

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		
		void bindParticle(ParticleSystem const& particle, VkCommandBuffer commandBuffer, const glm::vec4& CameraRight, const glm::vec4& CameraUp);
		void drawParticle(ParticleSystem const& particle, VkCommandBuffer commandBuffer);
		void Initialize();
		void Emit();
	//move to renderersystem

	protected:

		std::vector<ParticleSystem>		particleSystems;

		VkDescriptorImageInfo			blankImageInfo;
		VkImage							blankImage;
		VkImageView						blankImageView;
		VkDeviceMemory					blankImageMemories;
		VkSampler						blankImageSampler;

		BeBuffer                        indexBuffer;
		std::array<BeBuffer, 2>         vertexBuffers ;



		int                             curIndex = 0;

		BeDevice*						beDevice;
		VkPipelineLayout				pipelineLayout;
		std::unique_ptr<BePipeline>		bePipeline;

	};
}  // namespace BE
