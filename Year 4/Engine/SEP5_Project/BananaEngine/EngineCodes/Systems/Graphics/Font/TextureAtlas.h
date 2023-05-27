#pragma once
#include <pch.h>
#include "FontTexture.h"
#include "AreaAllocator.h"
#include "AtlasTexture.h"

namespace BE
{
	class TextureAtlas : public FontTexture
	{
	public:
		TextureAtlas();
		~TextureAtlas();

		void Initialize(BeDevice* beDevice, uint32_t width, uint32_t height, std::unique_ptr<BeDescriptorPool>& pool, VkDescriptorSetLayout setLayout);
		std::shared_ptr<AtlasTexture> addToTextureAtlas(uint32_t width, uint32_t height, uint8_t* pixels);
		void Shutdown();

		VkDescriptorSet getDescriptorSet() override;
		glm::vec4 getTextureWindow() override;
		int getWidth() override;
		int getHeight() override;

	protected:
		uint32_t fontAtlasTextureWidth{};
		uint32_t fontAtlasTextureHeight{};
		VkImage fontAtlasImage;
		VkImageView fontAtlasImageView{};
		VkSampler fontAtlasSampler{};
		VkDescriptorSet fontAtlasDescriptorSet{};
		AreaAllocator allocator;
		BeDevice* thisBeDevice{};
		VkDeviceMemory fontAtlasImageMemory;
	};
}