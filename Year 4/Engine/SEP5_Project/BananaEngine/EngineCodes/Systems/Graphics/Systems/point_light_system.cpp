#include "pch.h"

#include "point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>
#include <glm/ext/matrix_transform.hpp>
#include "../../ECS/ECS.h"

#pragma warning (push)
#pragma warning (disable:26812)
namespace BE {

struct PointLightPushConstants {
  glm::vec4 position{};
  glm::vec4 color{};
  float radius;
  int editorMode{};
};

PointLightSystem::PointLightSystem()
{
}

PointLightSystem::~PointLightSystem() {
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PointLightPushConstants);

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

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

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
  //assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
  AssertCore(pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  BePipeline::defaultPipelineConfigInfo(pipelineConfig);
  BePipeline::enableAlphaBlending(pipelineConfig);
  pipelineConfig.attributeDescriptions.clear();
  pipelineConfig.bindingDescriptions.clear();
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  bePipeline = std::make_unique<BePipeline>(
      beDevice,
      "../Assets/Shaders/point_light_vert.spv",
      "../Assets/Shaders/point_light_frag.spv",
      pipelineConfig);
}

void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) 
{
  auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frameTime, { 0.f, -1.f, 0.f });
  int lightIndex = 0;

  for (EntityID entity : ECS->GetTracker<lightComponentTracker>()->GetTrackedEntities())
  {
    auto& component = ECS->GetComponent<lightComponent>(entity);
    auto& entityInfo = ECS->GetComponent<EntityInfo>(entity);

    if (!entityInfo.HasFlag(EntityFlag::active))
    {
        continue;
    }
    AssertCore(lightIndex < MAX_LIGHTS, "Point lights exceed maximum specified");

    //glm::vec3 temp = glm::vec3(rotateLight * glm::vec4(ECS->GetComponent<Transform>(entity).GetPositon().toGLM(), 1.f));
    //ECS->GetComponent<Transform>(entity).SetPosition(BE::Vec3f(temp.x, temp.y, temp.z));

    ubo.pointLights[lightIndex].position = glm::vec4(ECS->GetComponent<Transform>(entity).GetPosition().toGLM(), 1.f);
    ubo.pointLights[lightIndex].color = glm::vec4(component.getColor(), component.getIntensity());
    ubo.pointLights[lightIndex].radius = component.getRadius();

    ++lightIndex;
  }
  ubo.numLights = lightIndex;
}

void PointLightSystem::render(FrameInfo& frameInfo) {
  bePipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(
      frameInfo.commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipelineLayout,
      0,
      1,
      &frameInfo.globalDescriptorSet,
      0,
      nullptr);

  for (EntityID entity : ECS->GetTracker<lightComponentTracker>()->GetTrackedEntities())
  {
      auto& component = ECS->GetComponent<lightComponent>(entity);
      auto& entityInfo = ECS->GetComponent<EntityInfo>(entity);

      if (!entityInfo.HasFlag(EntityFlag::active))
      {
          continue;
      }
      PointLightPushConstants push{};
      push.position = glm::vec4{ BE::ECS->GetComponent<BE::Transform>(entity).GetPosition().toGLM(),1.0f };
      push.radius = component.getRadius();
      push.color = glm::vec4{ component.getColor(),component.getIntensity() };

#ifdef USE_EDITOR
      push.editorMode = 1;
#else
      push.editorMode = 0;
#endif

      vkCmdPushConstants(
          frameInfo.commandBuffer,
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(PointLightPushConstants),
          &push);
      vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
  }
}

void PointLightSystem::setupSystem(BeDevice* device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
{
    beDevice = device;
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

void PointLightSystem::Shutdown()
{
    vkDestroyPipelineLayout(beDevice->device(), pipelineLayout, nullptr);
    bePipeline->destroyGraphicsPipeline();
}

}  // namespace BE
#pragma warning (pop)