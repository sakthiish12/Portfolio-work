#pragma once
#include "pch.h"
#include "TextureAtlas.h"
#include "../Be_device.hpp"
#include "../Be_descriptors.hpp"

namespace BE
{
	TextureAtlas::TextureAtlas()
	{
	}
	TextureAtlas::~TextureAtlas()
	{
	}
	void TextureAtlas::Initialize(BeDevice* beDevice, uint32_t width, uint32_t height, std::unique_ptr<BeDescriptorPool>& pool, VkDescriptorSetLayout setLayout)
	{
		thisBeDevice = beDevice;
		fontAtlasTextureWidth = width;
		fontAtlasTextureHeight = height;

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		thisBeDevice->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, fontAtlasImage, fontAtlasImageMemory);
		thisBeDevice->transitionImageLayout2(fontAtlasImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		thisBeDevice->transitionImageLayout2(fontAtlasImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = fontAtlasImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(beDevice->device(), &viewInfo, nullptr, &fontAtlasImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view");
		}

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(beDevice->device(), &samplerInfo, nullptr, &fontAtlasSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler");
		}

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = *pool->getDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &setLayout;

		if (vkAllocateDescriptorSets(beDevice->device(), &allocInfo, &fontAtlasDescriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate texture descriptor set");
		}

		VkDescriptorImageInfo imageInfo2 = {};
		imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo2.imageView = fontAtlasImageView;
		imageInfo2.sampler = fontAtlasSampler;

		VkWriteDescriptorSet descriptorWrites = {};
		descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites.dstSet = fontAtlasDescriptorSet;
		descriptorWrites.dstBinding = 0;
		descriptorWrites.dstArrayElement = 0;
		descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites.descriptorCount = 1;
		descriptorWrites.pImageInfo = &imageInfo2;

		vkUpdateDescriptorSets(beDevice->device(), 1, &descriptorWrites, 0, nullptr);

		allocator.Initialize(width, height);
	}
	void TextureAtlas::Shutdown()
	{
		//destroy
		vkDestroyImage(thisBeDevice->device(), fontAtlasImage, nullptr);
		vkDestroyImageView(thisBeDevice->device(), fontAtlasImageView, nullptr);
		vkDestroySampler(thisBeDevice->device(), fontAtlasSampler, nullptr);
		vkFreeMemory(thisBeDevice->device(), fontAtlasImageMemory, nullptr);
	}
	std::shared_ptr<AtlasTexture> TextureAtlas::addToTextureAtlas(uint32_t width, uint32_t height, uint8_t* pixels)
	{
		auto area = allocator.Allocate(width, height);
		if (area == Area(0,0,0,0))
		{
			return nullptr;
		}

		Area newArea;
		VkDeviceSize imageSize = width * height * 4;

		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingMemory{};

		thisBeDevice->createBuffer(
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

		void* mapped = nullptr;
		vkMapMemory(thisBeDevice->device(), stagingMemory, 0, imageSize, 0, &mapped);
		memcpy(mapped, pixels, imageSize);
		vkUnmapMemory(thisBeDevice->device(), stagingMemory);

		thisBeDevice->transitionImageLayout2(
			fontAtlasImage,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		thisBeDevice->copyBufferToImageForFont(stagingBuffer, fontAtlasImage, width, height, area.x, area.y);
		thisBeDevice->transitionImageLayout2(
			fontAtlasImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(thisBeDevice->device(), stagingBuffer, nullptr);
		vkFreeMemory(thisBeDevice->device(), stagingMemory, nullptr);

		return std::make_shared<AtlasTexture>(this, area);
	}
	
	VkDescriptorSet TextureAtlas::getDescriptorSet()
	{
		return fontAtlasDescriptorSet;
	}
	
	glm::vec4 TextureAtlas::getTextureWindow()
	{
		return glm::vec4{0.0f,0.0f,1.0f,1.0f};
	}
	
	int TextureAtlas::getWidth()
	{
		return fontAtlasTextureWidth;
	}
	
	int TextureAtlas::getHeight()
	{
		return fontAtlasTextureHeight;
	}
}