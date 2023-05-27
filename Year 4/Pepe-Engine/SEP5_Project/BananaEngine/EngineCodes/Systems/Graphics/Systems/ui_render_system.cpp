#include "pch.h"

#include "ui_render_system.hpp"
#include "../Be_descriptors.hpp"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../../ECS/ECS.h"
#include "../Be_swap_chain.hpp"
#include "../../RendererSystem/RendererSystem.h"
#include "../../../EditorSystem/UI_Hierarchy_Window/UI_Hierarchy_Window.h"


#pragma warning (push)
#pragma warning (disable:26812)
namespace BE {
UIRenderSystem::UIRenderSystem()
{
}

UIRenderSystem::~UIRenderSystem() {
}

void UIRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(uiPushConstantData);

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

void UIRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  BePipeline::defaultPipelineConfigInfo(pipelineConfig);
  BePipeline::enableTransparentTextures(pipelineConfig);
  pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  bePipeline = std::make_unique<BePipeline>(
      beDevice,
      "../Assets/Shaders/ui_shader_vert.spv",
      "../Assets/Shaders/ui_shader_frag.spv",
      pipelineConfig);
}

void UIRenderSystem::createBlankTextureImage()
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = 1;
    imageInfo.extent.height = 1;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    beDevice->createImageWithInfo(
        imageInfo,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        blankImage,
        blankImageMemories);

    // Create sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(beDevice->getPhysicalDevice(), &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    vkCreateSampler(beDevice->device(), &samplerInfo, nullptr, &blankImageSampler);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = blankImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    VkBuffer stagingBuffers{ VK_NULL_HANDLE };
    VkDeviceMemory stagingBufferMemories{ VK_NULL_HANDLE };
    BE::RendererSystem::GetInstance()->getDevice()->createBuffer(imageInfo.extent.width* imageInfo.extent.height*8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffers, stagingBufferMemories);

    transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffers, blankImage, static_cast<uint32_t>(imageInfo.extent.width), static_cast<uint32_t>(imageInfo.extent.height));
    transitionImageLayout(blankImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkCreateImageView(beDevice->device(), &viewInfo, nullptr, &blankImageView);
    blankImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    blankImageInfo.imageView = blankImageView;
    blankImageInfo.sampler = blankImageSampler;

    vkDestroyBuffer(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBuffers, nullptr);
    vkFreeMemory(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBufferMemories, nullptr);
}

void UIRenderSystem::setupSystem(BeDevice* device, VkRenderPass pass, VkDescriptorSetLayout globalSetLayout)
{
    beDevice = device;
    createPipelineLayout(globalSetLayout);
    createPipeline(pass);
    createBlankTextureImage();
    createRectangleShape();
}

void UIRenderSystem::renderGameObjects(FrameInfo& frameInfo, int frameIndex, BeDescriptorSetLayout& setLayout,BeDescriptorPool& pool
, CameraLocal* camera)
{
#ifdef USE_EDITOR
    //Don't draw UI
    if (!UI_Hierarchy_Window::Get()->getDrawUI())
        return;
#endif
    bePipeline->bind(frameInfo.commandBuffer);

    uiPushConstantData push{};

    //YOU MUST DRAW BACK TO FRONT! @donavern
    //For each canvas
    for (CanvasID cid : UI->GetAllCanvases())
    {
        //Get the canvas pointer
        const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(cid);

        //Skip rendering all UI Elements if the canvas is inactive
        //Skip rendering is UI Element is inactive
        if (!canvas->GetCanvasFlag(UICanvasFlag::isActive))
            continue;

        //For each UI Elements in the canvas
        for (UIElementID uid : canvas->GetAllUIElements())
        {
            //Check what type of UI element is it
            const std::shared_ptr<UIElement>& uiBase = canvas->GetUI(uid);

            //Skip rendering is UI Element is inactive
            if (!uiBase->GetUIElementFlag(UIElementFlags::isActive))
                continue;

            //Get position and scale, see which one you need.
            const Vec3f& uiPos1 = uiBase->GetUIDrawPos();		//Between 0 and 1
            const Vec3f& uiScale1 = uiBase->GetUIScale();		//Between 0 and 1

            //Check what type of UI element is it
            switch (uiBase->GetUIType())
            {
            case UIElementType::button:
            {
                //Get the derived class ptr
                const std::shared_ptr<UIButton> ui = canvas->GetUI<UIButton>(uid);

                //Get the asset to draw depending on the current status
                AssetID textureToUse = ui->GetTextureToUse();

                //Only render is there's texture to render
                if (textureToUse)
                {
                    if (ui->getHasInitialized() == false)
                    {
                        ui->InitializeForRendering(setLayout, pool, blankImageInfo, textureToUse,BEID(0),BEID(0));
                        ui->setHasInitialized(true);
                    }
                    if (ui->getHasTexturesChanged() || ui->GetHasTexturesChanged())
                    {
                        ui->addTexturesToUpdateList(setLayout, blankImageInfo, textureToUse,BEID(0),BEID(0));
                        ui->setHasTexturesChanged(false);
                        ui->updateDescriptorSet(*beDevice);
                    }
                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y};
                    push.textureToUse = 1;
                    push.uiType = 1;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);         

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }
                break;
            }

            case UIElementType::image:
            {
                //Get the derived class ptr
                const std::shared_ptr<UIImage> ui = canvas->GetUI<UIImage>(uid);

                //Get the asset to draw depending on the current status
                AssetID textureToUse = ui->GetTexture();

                //Only render is there's texture to render
                if (textureToUse)
                {
                    if (ui->getHasInitialized() == false)
                    {
                        ui->InitializeForRendering(setLayout, pool, blankImageInfo, textureToUse, BEID(0), BEID(0));
                        ui->setHasInitialized(true);
                    }
                    if (ui->getHasTexturesChanged())
                    {
                        ui->addTexturesToUpdateList(setLayout, blankImageInfo, textureToUse, BEID(0), BEID(0));
                        ui->setHasTexturesChanged(false);
                        ui->updateDescriptorSet(*beDevice);
                    }
                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 1;
                    push.uiType = 3;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }
                break;
            }

            case UIElementType::progressBar:
            {
                //Get the derived class ptr
                const std::shared_ptr<UIProgressBar> ui = canvas->GetUI<UIProgressBar>(uid);

                //Progress bar is split into 3 - background, middle, and border
                //The middle will be the one that is shrinking up and down

                //Retrieve the 3 textures using
                const auto& textures = ui->GetAllTextures();

                //Also get the bar % and opacity
                float percentage = ui->GetPercentage();
                float opacity = ui->GetFlickerOpacity();

                //If any of the textures have ID 0, then don't render
                //Render the background first using textures[UIProgressBarParts::background]
                if (ui->getHasInitialized() == false)
                {
                    ui->InitializeForRendering(setLayout, pool, blankImageInfo, textures[(size_t)UIProgressBarParts::background],
                        textures[(size_t)UIProgressBarParts::foreground], textures[(size_t)UIProgressBarParts::border]);
                    ui->setHasInitialized(true);
                }
                if (ui->getHasTexturesChanged())
                {
                    ui->addTexturesToUpdateList(setLayout, blankImageInfo, textures[(size_t)UIProgressBarParts::background],
                        textures[(size_t)UIProgressBarParts::foreground], textures[(size_t)UIProgressBarParts::border]);
                    ui->setHasTexturesChanged(false);
                    ui->updateDescriptorSet(*beDevice);
                }

                if (textures[(size_t)UIProgressBarParts::background])
                {
                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 1;
                    push.isVertical = ui->GetIsVertical();
                    push.uiType = 4;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }

                //Render the middle next using textures[UIProgressBarParts::foreground]
                //The U/horizontal coordinates (of UV) of the middle bar is affected by percentage
                //Do not simply scale the UI by the percentage value
                //the middle bar is also affected by opacity
                if (textures[(size_t)UIProgressBarParts::foreground])
                {
                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 2;
                    push.percentage = percentage;
                    push.flickerOpacity = opacity;
                    push.isVertical = ui->GetIsVertical();
                    push.uiType = 5;
                    push.brightness = ui->getBrightnessFactor();
                    push.color = ui->getFlickerColor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }
                if (textures[(size_t)UIProgressBarParts::border])
                {
                    //Finally, render the border using textures[UIProgressBarParts::border]
                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 3;
                    push.isVertical = ui->GetIsVertical();
                    push.uiType = 6;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }
                //All 3 parts of the progress bar renders at the same position/scale
                //However, if the middle bar isn't rendering in the correct location due to UV scaling
                //Then render the position at drawPos - (uiScale/2 * (1.0f - percentage)) (X position only)
                break;
            }

            case UIElementType::slider:
            {
                //Get the derived class ptr
                const std::shared_ptr<UISlider> ui = canvas->GetUI<UISlider>(uid);

                //Slider is split into 2 parts - the background and the knob
                //The knob is the one that is going to move around

                //Retrieve the background to render
                AssetID background = ui->GetSliderBarBackgroundTexture();

                //Retrieve the slider knob texture, texture will change depending on interaction status
                AssetID knob = ui->GetTextureToUse();

                //If any of the textures have ID 0, then don't render
                if (background && knob)
                {
                    //Render the background first at the UI Position obtained above
                    //uiPos1, uiScale1, uiPos2, uiScale2
                    if (ui->getHasInitialized() == false)
                    {
                        ui->InitializeForRendering(setLayout, pool, blankImageInfo, background, knob, BEID(0));
                        ui->setHasInitialized(true);
                    }
                    if (ui->getHasTexturesChanged())
                    {
                        ui->addTexturesToUpdateList(setLayout, blankImageInfo, background, knob, BEID(0));
                        ui->setHasTexturesChanged(false);
                        ui->updateDescriptorSet(*beDevice);
                    }

                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 1;
                    push.uiType = 7;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);

                    //Then render the knob using the scale and position obtained using these depending on your needs
                    //No need to worry about percentage because the pos has already factored that in for you
                    //ui->GetSliderKnobToDrawPos();		    //Returns between 0 and 1
                    //ui->GetSliderKnobToDrawScale();		//Returns between 0 and 1
                    //ui->GetSliderKnobScreenPos();		    //Returns between 0 and screen w/h
                    //ui->GetSliderKnobScreenScale();		//Returns between 0 and screen w/h

                    push.posAndScale = glm::vec4{ ui->GetSliderKnobToDrawPos().x,ui->GetSliderKnobToDrawPos().y,ui->GetSliderKnobToDrawScale().x,ui->GetSliderKnobToDrawScale().y };
                    push.textureToUse = 2;
                    push.uiType = 8;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                   // push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);
                }


                break;
            }

            case UIElementType::toggle:
            {
                //Get the derived class ptr
                const std::shared_ptr<UIToggle> ui = canvas->GetUI<UIToggle>(uid);

                //Toggle is made up of 2 parts: background and the checkmark

                //Retrieve the background to render, texture will change depending on interaction status
                AssetID background = ui->GetTextureToUse();

                //Retrieve the slider knob texture
                AssetID checkmark = ui->GetCheckmarkTexture();

                //If any of the textures have ID 0, then don't render
                if (background && checkmark)
                {
                    //Render the background first
                    if (ui->getHasInitialized() == false)
                    {
                        ui->InitializeForRendering(setLayout, pool, blankImageInfo, background, checkmark, BEID(0));
                        ui->setHasInitialized(true);
                    }
                    if (ui->getHasTexturesChanged())
                    {
                        ui->addTexturesToUpdateList(setLayout, blankImageInfo, background, checkmark, BEID(0));
                        ui->setHasTexturesChanged(false);
                        ui->updateDescriptorSet(*beDevice);
                    }

                    push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                    push.textureToUse = 1;
                    push.uiType = 9;
                    push.brightness = ui->getBrightnessFactor();
                    push.opacity = ui->getOpacity();
                    //push.rotationRad = ui->GetUIRot();
                    push.colorIntensityFactors = ui->getColorIntensityFactor();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                    bindRectangle(frameInfo.commandBuffer);
                    drawRectangle(frameInfo.commandBuffer);

                    //If toggle is on, then render the checkmark
                    if (ui->GetToggle())
                    {
                        push.posAndScale = glm::vec4{ uiPos1.x,uiPos1.y,uiScale1.x,uiScale1.y };
                        push.textureToUse = 2;
                        push.opacity = ui->getOpacity();
                        //push.rotationRad = ui->GetUIRot();
                        push.colorIntensityFactors = ui->getColorIntensityFactor();

                        vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uiPushConstantData), &push);
                        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &ui->getDescriptorSets()[frameIndex], 0, nullptr);

                        bindRectangle(frameInfo.commandBuffer);
                        drawRectangle(frameInfo.commandBuffer);
                    }
                }

                break;
            }
            }
        }
    }
}

void UIRenderSystem::Shutdown()
{
    vertexBuffer.reset();
    indexBuffer.reset();
    vkDestroyImage(beDevice->device(), blankImage, nullptr);
    vkDestroyImageView(beDevice->device(), blankImageView, nullptr);
    vkFreeMemory(beDevice->device(), blankImageMemories, nullptr);
    vkDestroySampler(beDevice->device(), blankImageSampler, nullptr);
    vkDestroyPipelineLayout(beDevice->device(), pipelineLayout, nullptr);
    bePipeline->destroyGraphicsPipeline();
}

void UIRenderSystem::setAllUIToRefreshTextures()
{
    for (CanvasID cid : UI->GetAllCanvases())
    {
        //Get the canvas pointer
        const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(cid);

        //Skip rendering all UI Elements if the canvas is inactive
        //Skip rendering is UI Element is inactive
        if (!canvas->GetCanvasFlag(UICanvasFlag::isActive))
            continue;

        //For each UI Elements in the canvas
        for (UIElementID uid : canvas->GetAllUIElements())
        {
            //Check what type of UI element is it
            const std::shared_ptr<UIElement>& uiBase = canvas->GetUI(uid);

            //Check what type of UI element is it
            switch (uiBase->GetUIType())
            {
            case UIElementType::button:
            {
                //Get the derived class ptr
                const std::shared_ptr<UIButton> ui = canvas->GetUI<UIButton>(uid);
                ui->SetHasTexturesChanged(true);
                break;
            }

            case UIElementType::fontText:
            case UIElementType::image:
            case UIElementType::progressBar:
            case UIElementType::slider:
            case UIElementType::toggle:
                uiBase->setHasTexturesChanged(true);
                break;
            }
        }
    }
}

void UIRenderSystem::transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer newCommandBuffer = BE::RendererSystem::GetInstance()->getDevice()->beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        newCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    BE::RendererSystem::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
}

void UIRenderSystem::createRectangleShape()
{
    Vertex newVertex;
    newVertex.position = glm::vec3{ -1.0f,-1.0f,0.0f };
    newVertex.uv = glm::vec2{ 0.0f,0.0f };
    vertices.emplace_back(newVertex);
    newVertex.position = glm::vec3{ 1.0f,-1.0f,0.0f };
    newVertex.uv = glm::vec2{ 1.0f,0.0f };
    vertices.emplace_back(newVertex);
    newVertex.position = glm::vec3{ 1.0f,1.0f,0.0f };
    newVertex.uv = glm::vec2{ 1.0f,1.0f };
    vertices.emplace_back(newVertex);
    newVertex.position = glm::vec3{ -1.0f,1.0f,0.0f };
    newVertex.uv = glm::vec2{ 0.0f,1.0f };
    vertices.emplace_back(newVertex);

    indices.emplace_back(3);
    indices.emplace_back(2);
    indices.emplace_back(1);
    indices.emplace_back(1);
    indices.emplace_back(0);
    indices.emplace_back(3);

    //Vertex
    assert(vertices.size() >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    uint32_t vertexSize = sizeof(vertices[0]);

    BeBuffer stagingBuffer{
        BE::RendererSystem::GetInstance()->getDevice(),
        vertexSize,
        static_cast<uint32_t>(vertices.size()),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    vertexBuffer = std::make_unique<BeBuffer>(
        BE::RendererSystem::GetInstance()->getDevice(),
        vertexSize,
        static_cast<uint32_t>(vertices.size()),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    BE::RendererSystem::GetInstance()->getDevice()->copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);

    //Indices
    VkDeviceSize bufferSize2 = sizeof(indices[0]) * indices.size();
    uint32_t indexSize = sizeof(indices[0]);

    BeBuffer stagingBuffer2{
        BE::RendererSystem::GetInstance()->getDevice(),
        indexSize,
        static_cast<uint32_t>(indices.size()),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer2.map();
    stagingBuffer2.writeToBuffer((void*)indices.data());

    indexBuffer = std::make_unique<BeBuffer>(
        BE::RendererSystem::GetInstance()->getDevice(),
        indexSize,
        static_cast<uint32_t>(indices.size()),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    BE::RendererSystem::GetInstance()->getDevice()->copyBuffer(stagingBuffer2.getBuffer(), indexBuffer->getBuffer(), bufferSize2);
}

void UIRenderSystem::bindRectangle(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void UIRenderSystem::drawRectangle(VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void UIRenderSystem::copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
{
    VkCommandBuffer newCommandBuffer = BE::RendererSystem::GetInstance()->getDevice()->beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width,height,1 };

    vkCmdCopyBufferToImage(newCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    BE::RendererSystem::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
}
}  // namespace BE
#pragma warning (pop)