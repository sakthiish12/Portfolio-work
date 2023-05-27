
#include "pch.h"
#include "Grid_System.hpp"

namespace BE
{
    GridSystem::GridSystem()
    {
    }

    GridSystem::~GridSystem() {
    }

    void GridSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(engineDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            LogCore("failed to create pipeline layout!");
        }
    }

    void GridSystem::createPipeline(VkRenderPass renderPass) {
       // assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        LogCore("Create Grid pipeline ");
        PipelineConfigInfo pipelineConfig{};
        BePipeline::defaultPipelineConfigInfoGrid(pipelineConfig);
        BePipeline::enableAlphaBlending2(pipelineConfig, VK_BLEND_OP_ADD);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        bePipeline = std::make_unique<BePipeline>(
           engineDevice,
            "../Assets/Shaders/grid_vert.spv",
            "../Assets/Shaders/grid_frag.spv",
            pipelineConfig
            );
    }

    void GridSystem::recreatePipeline(VkRenderPass renderPass) {
        bePipeline->destroyGraphicsPipeline();
        vkDestroyPipelineLayout(engineDevice->device(), pipelineLayout, nullptr);

        //assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        
        PipelineConfigInfo pipelineConfig{};
        BePipeline::defaultPipelineConfigInfoGrid(pipelineConfig);
        BePipeline::enableAlphaBlending2(pipelineConfig, VK_BLEND_OP_ADD);

        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        bePipeline = std::make_unique<BePipeline>(
            engineDevice,
            "../Assets/Shaders/grid_vert.spv",
            "../Assets/Shaders/grid_frag.spv",
            pipelineConfig
            );
    }

    void GridSystem::setupSystem(BeDevice* device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    {
        engineDevice = device;
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    void GridSystem::Shutdown()
    {
        vkDestroyPipelineLayout(engineDevice->device(), pipelineLayout, nullptr);
        bePipeline->destroyGraphicsPipeline();
    }

    void GridSystem::render(FrameInfo& frameInfo) {
        bePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );
        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}