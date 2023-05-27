#include "pch.h"
#include "../Be_descriptors.hpp"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../../ECS/ECS.h"
#include "../Be_swap_chain.hpp"
#include "../../../Components/ParticleSystem.h"
#include "particle_render_system.hpp"
#include "../../RendererSystem/RendererSystem.h"

#pragma warning (push)
#pragma warning (disable:26812)
#pragma warning (disable:4458)
#pragma warning (disable:4189)
#pragma warning (disable:4244)
#pragma warning (disable:4189)

namespace BE
{
	ParticleRenderSystem::ParticleRenderSystem()
	{}
	ParticleRenderSystem::~ParticleRenderSystem()
	{
	}
	void ParticleRenderSystem::setupSystem(BeDevice* device, VkRenderPass pass, VkDescriptorSetLayout globalSetLayout)
	{
		beDevice = device;
		createPipelineLayout(globalSetLayout);
		createPipeline(pass);
		createBlankTextureImage();
		Initialize();
	}

	void ParticleRenderSystem::createBlankTextureImage()
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = 1;
		imageInfo.extent.height = 1;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		beDevice->createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			blankImage,
			blankImageMemories);

		// Create sampler
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(beDevice->getPhysicalDevice(), &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		vkCreateSampler(beDevice->device(), &samplerInfo, nullptr, &blankImageSampler);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = blankImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkBuffer stagingBuffers{ VK_NULL_HANDLE };
		VkDeviceMemory stagingBufferMemories{ VK_NULL_HANDLE };
		BE::RendererSystem::GetInstance()->getDevice()->createBuffer(imageInfo.extent.width * imageInfo.extent.height * 8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffers, stagingBufferMemories);

		transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffers, blankImage, static_cast<uint32_t>(imageInfo.extent.width), static_cast<uint32_t>(imageInfo.extent.height));
		transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkCreateImageView(beDevice->device(), &viewInfo, nullptr, &blankImageView);
		blankImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		blankImageInfo.imageView = blankImageView;
		blankImageInfo.sampler = blankImageSampler;

		vkDestroyBuffer(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBuffers, nullptr);
		vkFreeMemory(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBufferMemories, nullptr);
	}

	void ParticleRenderSystem::copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer newCommandBuffer = BE::RendererSystem::GetInstance()->getDevice()->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width,height,1 };

		vkCmdCopyBufferToImage(newCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		BE::RendererSystem::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
	}

	void ParticleRenderSystem::transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer newCommandBuffer = BE::RendererSystem::GetInstance()->getDevice()->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			newCommandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		BE::RendererSystem::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
	}

	void ParticleRenderSystem::renderGameObjects(FrameInfo& frameInfo, int frameIndex, BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, const CameraLocal& camera)
	{
		bePipeline->bind(frameInfo.commandBuffer);

		std::vector<EntityID> entities = ECS->GetTracker<ParticleSystemTracker>()->GetTrackedEntities();

		//Not too sure about this.
		//AssetID textureToUse = AssetMgr->GetAssetData<Texture>(textureToUse);

		//Only render is there's texture to render
		//if (textureToUse)
		// 
		// For each entity possessing a particle system...
		for (EntityID& entity : entities)
		{
			// We initialize once the particle system and the Descriptor Set
			// We might want to change the initialized parameter for a "updated" one, so that we can update the Descriptor Set if we change the texture
			if (ECS->GetComponent<ParticleSystem>(entity).initialized == false)
			{
				ECS->GetComponent<ParticleSystem>(entity).InitializeForRendering(setLayout, pool, blankImageInfo, BEID(0));
				ECS->GetComponent<ParticleSystem>(entity).updateDescriptorSet(*beDevice);
			}

		/* if (ECS->GetComponent<ParticleSystem>(entity).getHasTexturesChanged())
			{
				ECS->GetComponent<ParticleSystem>(entity).addTexturesToUpdateList(setLayout, blankImageInfo, textureToUse);
				ECS->GetComponent<ParticleSystem>(entity).setHasTexturesChanged(false);
				ECS->GetComponent<ParticleSystem>(entity).updateDescriptorSet(*beDevice);
			}*/


			const float deltaTime = 0.01f;
			auto count = ECS->GetComponent<ParticleSystem>(entity).getParticleCount();
			// We update the Particle System (Particle update & population)
			ECS->GetComponent<ParticleSystem>(entity).UpdateParticleSystem(deltaTime,count);

			const glm::mat4& M = camera.cc.getInverseView();

			const auto CameraRight = M[0];
			const auto CameraUp = M[1];

			// For each particle in the particle system...
			ParticleSystem& System = ECS->GetComponent<ParticleSystem>(entity);
			if (count > 0)
			{
				ParticlePushConstant push{};

				push.pos = System.getPos();
				push.color = System.getColor();
				push.size  = System.particleSize;
				push.modelMatrix = camera.cc.getProjection() * camera.cc.getView();
				
				// We push the constants and draw the particle
				vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ParticlePushConstant), &push);
				vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(ECS->GetComponent<ParticleSystem>(entity).getDescriptorSets()[frameIndex]), 0, nullptr);
				bindParticle(System, frameInfo.commandBuffer, CameraRight, CameraUp);
				drawParticle(System, frameInfo.commandBuffer);
			}
		}
	}

	void ParticleRenderSystem::Shutdown()
	{
		// Destroy index buffer
		indexBuffer.destroy();

		// Destroy vertex buffers
		for (auto& VB : vertexBuffers)
		{
			VB.destroy();
		}
		if (beDevice->device() == nullptr) {}
		else{
		vkDestroyImage(beDevice->device(), blankImage, nullptr);
		vkDestroyImageView(beDevice->device(), blankImageView, nullptr);
		vkFreeMemory(beDevice->device(), blankImageMemories, nullptr);
		vkDestroySampler(beDevice->device(), blankImageSampler, nullptr);
		vkDestroyPipelineLayout(beDevice->device(), pipelineLayout, nullptr);
		}

		bePipeline->destroyGraphicsPipeline();
	}

	void ParticleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ParticlePushConstant);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(beDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void ParticleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BePipeline::defaultPipelineConfigInfo2(pipelineConfig);
		BePipeline::enableTransparentTextures(pipelineConfig);
		pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		bePipeline = std::make_unique<BePipeline>(
			beDevice,
			"../Assets/Shaders/Particle_vert.spv",
			"../Assets/Shaders/Particle_frag.spv",
			pipelineConfig);
	}

	void ParticleRenderSystem::bindParticle(ParticleSystem const& System, VkCommandBuffer commandBuffer, const glm::vec4& CameraRight, const glm::vec4& CameraUp )
	{
		auto pVerParticle = reinterpret_cast<ParticleVertex*>(vertexBuffers[curIndex].getMappedMemory());

		// Convert the particleSystems into vertices
		int iVert = 0;
		for (auto& particle : System.GetParticles())
		{
			const auto CR = CameraRight * particle.size * 0.5f;
			const auto CU = CameraUp * particle.size * 0.5f;

			auto c_uint = 0;
			c_uint |= static_cast<std::uint32_t>(particle.color.x * 255) << 0;
			c_uint |= static_cast<std::uint32_t>(particle.color.y * 255) << 8;
			c_uint |= static_cast<std::uint32_t>(particle.color.z * 255) << 16;
			c_uint |= static_cast<std::uint32_t>(particle.color.w * 255) << 24;

			pVerParticle[iVert].m_Position = particle.pos - CR - CU;
			pVerParticle[iVert].m_Color = c_uint;
			pVerParticle[iVert].m_UV = glm::vec2(0, 0);
			iVert++;

			pVerParticle[iVert].m_Position = particle.pos + CR - CU;
			pVerParticle[iVert].m_Color = c_uint;
			pVerParticle[iVert].m_UV = glm::vec2(0, 1);
			iVert++;

			pVerParticle[iVert].m_Position = particle.pos + CR + CU;
			pVerParticle[iVert].m_Color = c_uint;
			pVerParticle[iVert].m_UV = glm::vec2(1, 1);
			iVert++;

			pVerParticle[iVert].m_Position = particle.pos - CR + CU;
			pVerParticle[iVert].m_Color = c_uint;
			pVerParticle[iVert].m_UV = glm::vec2(1, 0);
			iVert++;
		}

		// Do the proper binding
		VkBuffer     buffers[] = { vertexBuffers[curIndex].getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);

		curIndex = 1 - curIndex;
	}

	void ParticleRenderSystem::drawParticle(ParticleSystem const& particle, VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(BE::ParticleSystem::max_particles_v * 6), particle.getParticleCount(), 0, 0, 0);
	}

	void ParticleRenderSystem::Initialize()
	{
		// Create Index Buffer
		{
			BeBuffer stagingIndexBuffer
			{ BE::RendererSystem::GetInstance()->getDevice()
			, sizeof(std::uint16_t)
			, static_cast<uint32_t>(BE::ParticleSystem::max_particles_v *6)
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			};

			int iIndexBase = 0;
			int iBaseVertex = 0;
			stagingIndexBuffer.map();
			{
				std::uint16_t* pIndexBase = reinterpret_cast<std::uint16_t*>(stagingIndexBuffer.getMappedMemory());

				pIndexBase[iIndexBase ++] = iBaseVertex + 0;
				pIndexBase[iIndexBase ++] = iBaseVertex + 2;
				pIndexBase[iIndexBase ++] = iBaseVertex + 1;
				pIndexBase[iIndexBase ++] = iBaseVertex + 0;
				pIndexBase[iIndexBase ++] = iBaseVertex + 3;
				pIndexBase[iIndexBase ++] = iBaseVertex + 2;

				iBaseVertex += 4;
			}
			stagingIndexBuffer.unmap();

			indexBuffer.Create
			(BE::RendererSystem::GetInstance()->getDevice()
				, sizeof(std::uint16_t)
				, static_cast<uint32_t>(BE::ParticleSystem::max_particles_v *6)
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			BE::RendererSystem::GetInstance()->getDevice()->copyBuffer(stagingIndexBuffer.getBuffer(), indexBuffer.getBuffer(), static_cast<uint32_t>(BE::ParticleSystem::max_particles_v *6 * sizeof(std::uint16_t)));
		}

		// Initialize vertex buffer
		for (auto& VB : vertexBuffers)
		{
			VB.Create
			( BE::RendererSystem::GetInstance()->getDevice()
				, sizeof(ParticleVertex)
				, static_cast<uint32_t>(BE::ParticleSystem::max_particles_v *4)
				, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			VB.map();
		}
	}

	//Sample Functions
	void ParticleSystem::createAndDestroyParticleSystem(float lifetime, glm::vec4 pos, glm::vec4 color, glm::vec4 vel, float particleSize)
	{
		// Create the entity and add the particle system component
		BE::EntityID entityID = ECS->CreateEntity();
		ECS->AddComponent<BE::ParticleSystem>(entityID);

		// Set particle system properties
		BE::ParticleSystem& particleSystem = ECS->GetComponent<BE::ParticleSystem>(entityID);
		particleSystem.setPos(pos);
		particleSystem.setColor(color);
		particleSystem.setVel(vel);
		particleSystem.setParticleSize(particleSize);
		particleSystem.setLifeTime(lifetime);

		// Wait for the given lifetime
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(lifetime * 1000)));

		// Destroy the entity
		ECS->DestroyEntity(entityID);
	}

	//To use the this function
	/*
		BE::EntityID entityID = BE::ECS->CreateEntity();
		BE::ECS->AddComponent<BE::ParticleSystem>(entityID);

		// Set properties
		BE::ParticleSystem& particleSystem = BE::ECS->GetComponent<BE::ParticleSystem>(entityID);
		particleSystem.setPos(glm::vec4{ 0.f, 0.f, 0.f, 1.f });
		particleSystem.setColor(glm::vec4{ 1.f, 1.f, 1.f, 1.f });
		particleSystem.setVel(glm::vec4{ 0.f, 0.f, 0.f, 0.f });
		particleSystem.setParticleSize(5.f);
		particleSystem.setLifeTime(10.f);

		// Emit particles
		particleSystem.Emit(glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f }, 10.f, 90.f, 100);

		// Destroy entity after lifetime
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 10000)
		{
			particleSystem.UpdateParticleSystem(0.01f);
		}
		BE::ECS->DestroyEntity(entityID);

	*/

	void ParticleSystem::Emit(glm::vec3 const& position, glm::vec3 const& direction, float speed, float spread, int count)
	{
		float angleStep = glm::radians(spread) / count;
		glm::vec3 dir = glm::normalize(direction);
		glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3{ 0.f, 1.f, 0.f }));
		glm::vec3 up = glm::normalize(glm::cross(right, dir));

		for (int i = 0; i < count; ++i)
		{
			float angle = angleStep * i - glm::radians(spread) * 0.5f;
			glm::vec3 spreadDir = glm::normalize(dir + right * std::cos(angle) + up * std::sin(angle));
			glm::vec3 vel = spreadDir * speed;
			particles.emplace_back(glm::vec4(position, 1.0f), color, glm::vec4(vel, 0.f), particleSize, lifetime);
		}
	}



	


}