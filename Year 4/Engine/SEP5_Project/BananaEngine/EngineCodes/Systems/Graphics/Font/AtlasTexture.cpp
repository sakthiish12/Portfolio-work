#pragma once
#include "pch.h"
#include "AtlasTexture.h"
#include "TextureAtlas.h"

namespace BE
{
	AtlasTexture::AtlasTexture(TextureAtlas* newOwner, Area newArea)
	{
		owner = newOwner;
		area = newArea;
	}
	VkDescriptorSet AtlasTexture::getDescriptorSet()
	{
		return owner->getDescriptorSet();
	}
	glm::vec4 AtlasTexture::getTextureWindow()
	{
		const float x0 = float(area.x) / float(owner->getWidth());
		const float y0 = float(area.y) / float(owner->getHeight());
		const float x1 = x0 + float(area.width) / float(owner->getWidth());
		const float y1 = y0 + float(area.height) / float(owner->getHeight());
		return glm::vec4{ x0, y0, x1, y1 };
	}
	int AtlasTexture::getWidth()
	{
		return area.width;
	}
	int AtlasTexture::getHeight()
	{
		return area.height;
	}
}