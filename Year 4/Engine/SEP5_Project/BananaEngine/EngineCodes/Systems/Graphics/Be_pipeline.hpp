#pragma once

#include "Be_device.hpp"

// std
//#include <string>
//#include <vector>

namespace BE {

struct PipelineConfigInfo {
  PipelineConfigInfo() = default;
  PipelineConfigInfo(const PipelineConfigInfo&) = delete;
  PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

  std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VkPipelineViewportStateCreateInfo viewportInfo{};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
  VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
  VkPipelineMultisampleStateCreateInfo multisampleInfo{};
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
  std::vector<VkDynamicState> dynamicStateEnables{};
  VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class BePipeline {
 public:
  BePipeline(
      BeDevice* device,
      const std::string& vertFilepath,
      const std::string& fragFilepath,
      const PipelineConfigInfo& configInfo);
  ~BePipeline();

  BePipeline(const BePipeline&) = delete;
  BePipeline& operator=(const BePipeline&) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
  static void defaultPipelineConfigInfo2(PipelineConfigInfo& configInfo);
  static void defaultPipelineConfigInfoGrid(PipelineConfigInfo& configInfo);
  static void enableAlphaBlending(PipelineConfigInfo& configInfo);
  static void enableAlphaBlending2(PipelineConfigInfo& configInfo, VkBlendOp blendOp);
  static void enableTransparentTextures(PipelineConfigInfo& configInfo);
  static void enableWireframe(PipelineConfigInfo& configInfo);
  static void enableForText(PipelineConfigInfo& configInfo);
  void destroyGraphicsPipeline();

 

 private:
  static std::vector<char> readFile(const std::string& filepath);

  void createGraphicsPipeline(
      const std::string& vertFilepath,
      const std::string& fragFilepath,
      const PipelineConfigInfo& configInfo);

  void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

  BeDevice* beDevice;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};
}  // namespace BE
