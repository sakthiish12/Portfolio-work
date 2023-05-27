#pragma once

#include "Camera/Be_camera.hpp"

// lib
//#include <vulkan/vulkan.h>

namespace BE {

#define MAX_LIGHTS 100

struct tempInfo {
	static inline uint32_t viewportSize[2];
	static inline float aspectRatioWoH;
};

struct PointLight {
	alignas(16) glm::vec4 position{};  // ignore w
	alignas(16) glm::vec4 color{};     // w is intensity
	alignas(4) float radius{};
};

struct GlobalUbo {
  alignas(16) glm::mat4 projection{1.f};
  alignas(16) glm::mat4 view{1.f};
  alignas(16) glm::mat4 inverseView{1.f};
  alignas(16) glm::mat4 boneMatrices[30]{};
  alignas(16) glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};  // w is intensity
  PointLight pointLights[MAX_LIGHTS]{};
  alignas(4) int numLights{};
  alignas(16) glm::vec4 cameraWorldPos{};
  alignas(16)	 glm::vec4 mousePos{ 0.0f };
  alignas(16) glm::vec4 vpData{ 0.0f };
  alignas(16) glm::vec4 frameSize{};
  alignas(4) int pressed;
};

struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  VkDescriptorSet globalDescriptorSet;
};
}  // namespace BE
