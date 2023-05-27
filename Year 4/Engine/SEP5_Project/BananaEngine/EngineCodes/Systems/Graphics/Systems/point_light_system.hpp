#pragma once


#include "../Camera/Be_camera.hpp"
#include "../Be_device.hpp"
#include "../Be_frame_info.hpp"
#include "../Be_pipeline.hpp"

// std
//#include <memory>
//#include <vector>

namespace BE {
class PointLightSystem {
 public:
     PointLightSystem();
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem &) = delete;
  PointLightSystem &operator=(const PointLightSystem &) = delete;

  void update(FrameInfo &frameInfo, GlobalUbo &ubo);
  void render(FrameInfo &frameInfo);
  void setupSystem(BeDevice* device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
  void Shutdown();
 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  BeDevice *beDevice;

  std::unique_ptr<BePipeline> bePipeline;
  VkPipelineLayout pipelineLayout;
};
}  // namespace BE
