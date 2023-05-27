#pragma once
#include <pch.h>
#include "FontTexture.h"
#include "Area.h"

namespace BE
{
	struct Area;
	class TextureAtlas;

	class AtlasTexture : public FontTexture
	{
	public:
		AtlasTexture(TextureAtlas* newOwner, Area newArea);

		VkDescriptorSet getDescriptorSet() override;
		glm::vec4 getTextureWindow() override;
		int getWidth() override;
		int getHeight() override;

	protected:
		TextureAtlas* owner{};
		Area area{};
	};
}