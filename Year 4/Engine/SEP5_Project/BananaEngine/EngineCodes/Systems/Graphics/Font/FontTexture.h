#pragma once
#include <pch.h>

namespace BE {
	struct FontTexture {
		virtual VkDescriptorSet getDescriptorSet() = 0;
		virtual glm::vec4 getTextureWindow() = 0;
		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
	};
}