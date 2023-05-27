#include "pch.h"
#include "text_render_system.hpp"
#include "../../RendererSystem/RendererSystem.h"
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048

namespace BE
{
	TextRenderSystem::TextRenderSystem()
	{
	}

	TextRenderSystem::~TextRenderSystem()
	{
	}

	void TextRenderSystem::setupSystem(BeDevice* newbeDevice, 
		VkRenderPass pass, VkDescriptorSetLayout globalSetLayout, 
		std::unique_ptr<BeDescriptorPool>& pool,float width,float height)
	{
		beDevice = newbeDevice;
		offscreenPassWidth = width;
		offscreenPassHeight = height;

		createPipelineLayouts(globalSetLayout);
		createPipelines(pass);
		createBlankTextureImage(globalSetLayout,pool);
		createBufferToWriteTo();

		AssetManager::GetInstance()->initializeFontStuff(static_cast<uint32_t>(width),
			static_cast<uint32_t>(height), globalSetLayout, pool);
	}

	void TextRenderSystem::renderText(FrameInfo& frameInfo, GlobalUbo& ubo)
	{
#ifdef USE_EDITOR
		//Don't draw UI
		if (!UI_Hierarchy_Window::Get()->getDrawUI())
			return;
#endif
		bePipeline->bind(frameInfo.commandBuffer);

		//Add vertices with positioning + uv data into buffer to draw with
		startWritingToBuffer();

		//NEXT
		int entityCounter{}, letterCounter{};

		pushConstantData.projection = ubo.projection;
		pushConstantData.view = ubo.view;

		for (CanvasID cid : UI->GetAllCanvases())
		{
			//Get the canvas pointer
			const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(cid);

			//Skip rendering all UI Elements if the canvas is inactive
			//Skip rendering is UI Element is inactive
			if (!canvas->GetCanvasFlag(UICanvasFlag::isActive))
				continue;

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				//Check what type of UI element is it
				const std::shared_ptr<UIElement>& uiBase = canvas->GetUI(uid);

				//Skip rendering is UI Element is inactive
				if (!uiBase->GetUIElementFlag(UIElementFlags::isActive))
					continue;

				if (uiBase->GetUIType() != UIElementType::fontText)
					continue;

				const std::shared_ptr<UIFontText> ui = canvas->GetUI<UIFontText>(uid);

				if (ui->GetText().length() == 0 || ui->GetFont() == 0)
					continue;

				if (entityCounter == 0)
				{
					VkDeviceSize offsets = 0;
					vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, &writeToBuffer, &offsets);
					vkCmdBindVertexBuffers(frameInfo.commandBuffer, 1, 1, &writeToBuffer, &offsets);
				}

				pushConstantData.colorAndOpacity = glm::vec4{ ui->GetColour(),ui->getOpacity() };
				pushConstantData.brightnessFactor = ui->getBrightnessFactor();
				pushConstantData.colorIntensityFactor = ui->getColorIntensityFactor();
				vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(TextPushConstantData), &pushConstantData);

				//Only update descriptor set if font type/style changed
				if (texturesToUse[entityCounter] != prevSet)
				{
					prevSet = texturesToUse[entityCounter];
					vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &texturesToUse[entityCounter], 0, NULL);
				}
				++entityCounter;

				for (size_t c{}; c < ui->GetText().length(); ++c)
				{
					vkCmdDraw(frameInfo.commandBuffer, 6, 1, static_cast<uint32_t>(letterCounter++ * 6), 0);
				}
			}

			prevSet = VK_NULL_HANDLE;
		}
	}

	void TextRenderSystem::createPipelineLayouts(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(TextPushConstantData);

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

	void TextRenderSystem::createPipelines(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BePipeline::defaultPipelineConfigInfo(pipelineConfig);
		BePipeline::enableForText(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		bePipeline = std::make_unique<BePipeline>(
			beDevice,
			"../Assets/Shaders/text_shader_vert.spv",
			"../Assets/Shaders/text_shader_frag.spv",
			pipelineConfig);
	}

	void TextRenderSystem::Shutdown()
	{
		// Free up all Vulkan resources requested by the text overlay
		AssetManager::GetInstance()->shutdownFontStuff();
		vkDestroyImage(beDevice->device(), blankImage, nullptr);
		vkFreeMemory(beDevice->device(), blankImageMemory, nullptr);
		vkDestroySampler(beDevice->device(), blankImageSampler, nullptr);
		vkDestroyImageView(beDevice->device(), blankImageView, nullptr);

		vkDestroyBuffer(beDevice->device(), writeToBuffer, nullptr);
		vkFreeMemory(beDevice->device(), writeToMemory, nullptr);
		vkDestroyPipelineLayout(beDevice->device(), pipelineLayout, nullptr);
		bePipeline->destroyGraphicsPipeline();
	}

	void TextRenderSystem::createBlankTextureImage(VkDescriptorSetLayout globalSetLayout,
		std::unique_ptr<BeDescriptorPool>& pool)
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
			blankImageMemory);

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

		beDevice->transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		beDevice->copyBufferToImage(stagingBuffers, blankImage, static_cast<uint32_t>(imageInfo.extent.width), static_cast<uint32_t>(imageInfo.extent.height),1);
		beDevice->transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkCreateImageView(beDevice->device(), &viewInfo, nullptr, &blankImageView);
		blankImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		blankImageInfo.imageView = blankImageView;
		blankImageInfo.sampler = blankImageSampler;

		vkDestroyBuffer(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBuffers, nullptr);
		vkFreeMemory(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBufferMemories, nullptr);

		//allocate one descriptor set for each frame
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool =*pool->getDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &globalSetLayout;
		vkAllocateDescriptorSets(beDevice->device(), &allocInfo, &blankImageDescriptorSet);

		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = 0;
		setWrite.dstSet = blankImageDescriptorSet;
		setWrite.descriptorCount = 1;
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setWrite.pImageInfo = &blankImageInfo;
		vkUpdateDescriptorSets(beDevice->device(), 1, &setWrite, 0, nullptr);
	}

	void TextRenderSystem::createBufferToWriteTo()
	{
		// Vertex buffer
		VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(glm::vec4);

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if (vkCreateBuffer(beDevice->device(), &bufferInfo, nullptr, &writeToBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vertex buffer fpr text render system");
		}

		VkMemoryRequirements memReqs;
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		vkGetBufferMemoryRequirements(beDevice->device(), writeToBuffer, &memReqs);
		allocInfo.allocationSize = memReqs.size;
		allocInfo.memoryTypeIndex = beDevice->findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(beDevice->device(), &allocInfo, nullptr, &writeToMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate memory for text render system");
		}
		if (vkBindBufferMemory(beDevice->device(), writeToBuffer, writeToMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to bind buffer memory for text render system");
		}
	}

	void TextRenderSystem::mapBufferToWriteTo()
	{
		if (vkMapMemory(beDevice->device(), writeToMemory, 0, VK_WHOLE_SIZE, 0, (void**)&mapped) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to map text memory");
		}
	}

	void TextRenderSystem::unmapBufferWritingTo()
	{
		vkUnmapMemory(beDevice->device(), writeToMemory);
		mapped = nullptr;
	}

	void TextRenderSystem::startWritingToBuffer()
	{
		texturesToUse.clear();
		numberOfLetters = 0;

		mapBufferToWriteTo();

		for (CanvasID cid : UI->GetAllCanvases())
		{
			//Get the canvas pointer
			const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(cid);

			//Skip rendering all UI Elements if the canvas is inactive
			//Skip rendering is UI Element is inactive
			if (!canvas->GetCanvasFlag(UICanvasFlag::isActive))
				continue;

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				//Check what type of UI element is it
				const std::shared_ptr<UIElement>& uiBase = canvas->GetUI(uid);

				//Skip rendering is UI Element is inactive
				if (!uiBase->GetUIElementFlag(UIElementFlags::isActive))
					continue;

				//Get position and scale, see which one you need.
				const Vec3f& uiPos1 = uiBase->GetUIDrawPos();		//Between 0 and 1
				const Vec3f& uiScale1 = uiBase->GetUIScale();		//Between 0 and 1

				if (uiBase->GetUIType() == UIElementType::fontText)
				{
					const std::shared_ptr<UIFontText> ui = canvas->GetUI<UIFontText>(uid);

					if (ui->GetFont() == 0 || ui->GetText().length() == 0)
						continue;

					Font& fontToUse = AssetMgr->GetAssetData<Font>(ui->GetFont());

					glm::vec2 position = glm::vec2{ uiPos1.x * offscreenPassWidth,(1.0f- uiPos1.y) * offscreenPassHeight };
					glm::vec2 scale = glm::vec2{ uiScale1.x,uiScale1.y };

					const glm::vec2 textDimensions = fontToUse.MeasureTextDimensions(ui->GetText());

					switch (ui->GetAlignment())
					{
					case UIFontTextAlignment::right:
						position.x -= textDimensions.x * scale.x;
						break;
					case UIFontTextAlignment::center:
						position.x -= textDimensions.x * 0.5f * scale.x;
						break;
					case UIFontTextAlignment::left:
						break;
					}

					//switch (ui->GetUIHorizontalAlignment())
					//{
					//case UIHorizontalAlignment::right:
					//	position.x -= textDimensions.x * scale.x;
					//	break;
					//case UIHorizontalAlignment::middle:
					//	position.x -= textDimensions.x * 0.5f * scale.x;
					//	break;
					//case UIHorizontalAlignment::left:
					//	break;
					//}

					//switch (ui->GetUIVerticalAlignment())
					//{
					//case TextAlignVertical::alignTop:
					//	break;
					//case TextAlignVertical::alignCenter:
					//	position.y -= textDimensions.y * 0.25f * scale;
					//	break;
					//case TextAlignVertical::alignBottom:
					//	position.y -= textDimensions.y * scale;
					//	break;
					//}

					for (size_t letterCounter{}; letterCounter < ui->GetText().length(); ++letterCounter)
					{
						assert(mapped != nullptr);

						const std::shared_ptr<Glyph> glyph = fontToUse.getGlyph(ui->GetText()[letterCounter]);
						const std::shared_ptr<FontTexture> texture = glyph->getTexture();

						if (texture)
						{
							texturesToUse.emplace_back(texture->getDescriptorSet());
							const glm::vec4 textureWindow = texture->getTextureWindow();

							const float x0 = position.x;
							float y0 = position.y;

							if (ui->GetText()[letterCounter] == 'q' || ui->GetText()[letterCounter] == 'y' ||
								ui->GetText()[letterCounter] == 'p' || ui->GetText()[letterCounter] == 'g' || ui->GetText()[letterCounter] == 'j')
							{
								y0 -= texture->getHeight()*0.25f * scale.y;
							}

							//1  - top left  vertex 0 
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0 + texture->getHeight() * scale.y, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.y;
							mapped++;

							//3 - btm right vertex 1
							mapped->x = ConvertToClipSpace(x0 + texture->getWidth() * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.w;
							mapped++;

							//0 - btm left vertex 2
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.w;
							mapped++;

							//3  - btm right vertex 3
							mapped->x = ConvertToClipSpace(x0 + texture->getWidth() * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.w;
							mapped++;

							//1 - top left vertex 4
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0 + texture->getHeight() * scale.y, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.y;
							mapped++;

							//2 - top right vertex 5
							mapped->x = ConvertToClipSpace(x0 + texture->getWidth() * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0 + texture->getHeight() * scale.y, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.y;
							mapped++;
						}
						else
						{
							texturesToUse.emplace_back(blankImageDescriptorSet);
							const glm::vec4 textureWindow = glm::vec4{ 0.0f,0.0f,0.0f,0.0f };

							const float x0 = position.x;
							const float y0 = position.y;

							//1  - top left  vertex 0 
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0 * scale.y, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.y;
							mapped++;

							//3 - btm right vertex 1
							mapped->x = ConvertToClipSpace(x0 * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.w;
							mapped++;

							//0 - btm left vertex 2
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.w;
							mapped++;

							//3  - btm right vertex 3
							mapped->x = ConvertToClipSpace(x0 * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.w;
							mapped++;

							//1 - top left vertex 4
							mapped->x = ConvertToClipSpace(x0, 0);
							mapped->y = ConvertToClipSpace(y0 * scale.y, 1);
							mapped->z = textureWindow.x;
							mapped->w = textureWindow.y;
							mapped++;

							//2 - top right vertex 5
							mapped->x = ConvertToClipSpace(x0 * scale.x, 0);
							mapped->y = ConvertToClipSpace(y0 * scale.y, 1);
							mapped->z = textureWindow.z;
							mapped->w = textureWindow.y;
							mapped++;
						}

						position.x += (static_cast<float>(glyph->getAdvance()) * scale.x);

						if (ui->GetText()[letterCounter] == 'j')
						{
							position.x +=  texture->getWidth() * 0.25f * scale.x;
						}
						++numberOfLetters;
					}
				}
			}
		}

		unmapBufferWritingTo();
	}

	float TextRenderSystem::ConvertToClipSpace(float x,int i) // 0 for width, 1 for height
	{
		if (i == 0)
			return (x / offscreenPassWidth * 2.0f) - 1.0f;
		else
			return (x / offscreenPassHeight * 2.0f) - 1.0f;
	}
}