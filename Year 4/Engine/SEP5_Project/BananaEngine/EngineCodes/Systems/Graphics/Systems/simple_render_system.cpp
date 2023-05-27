#include "pch.h"

#include "simple_render_system.hpp"
#include "../Be_descriptors.hpp"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../../ECS/ECS.h"
#include "../Be_swap_chain.hpp"
#include "../../RendererSystem/RendererSystem.h"
#include "../../../Components/Transform.h"

#pragma warning (push)
#pragma warning (disable:26812)
namespace BE {

    SimpleRenderSystem::SimpleRenderSystem()
    {
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
    }

    void SimpleRenderSystem::createPipelineLayouts(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

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

        if (vkCreatePipelineLayout(beDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayoutWireframe) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipelines(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        BePipeline::defaultPipelineConfigInfo(pipelineConfig);
        BePipeline::enableTransparentTextures(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        bePipeline = std::make_unique<BePipeline>(
            beDevice,
            "../Assets/Shaders/simple_shader_vert.spv",
            "../Assets/Shaders/simple_shader_frag.spv",
            pipelineConfig);

        BePipeline::enableWireframe(pipelineConfig);
        pipelineConfig.pipelineLayout = pipelineLayoutWireframe;
        bePipelineWireframe = std::make_unique<BePipeline>(
            beDevice,
            "../Assets/Shaders/simple_shader_vert.spv",
            "../Assets/Shaders/simple_shader_frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::createBlankTextureImage()
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
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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
        BE::RendererSystem::GetInstance()->getDevice()->createBuffer(imageInfo.extent.width * imageInfo.extent.height * 8, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffers, stagingBufferMemories);

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

    void SimpleRenderSystem::setupSystem(BeDevice* device, VkRenderPass pass, VkDescriptorSetLayout globalSetLayout)
    {
        beDevice = device;
        createPipelineLayouts(globalSetLayout);
        createPipelines(pass);
        createBlankTextureImage();

        pickBuffer = std::make_unique<BE::BeBuffer>(
            device,
            sizeof(Info),
            1,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        pickBuffer->map();
        pickBuffer->writeToBuffer(&pickerInfo);
        pickBuffer->flush();
    }
    void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo, GlobalUbo& ubo, std::vector<std::unique_ptr<BE::BeBuffer>>& uboBuffers, int frameIndex,
        BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, CameraLocal* gameCamera, BeWindow* window, bool assetReloaded, bool globalWireframeMode)
    {
        bePipeline->bind(frameInfo.commandBuffer);
        wasWireframe = false;

        int frameWidth, frameHeight;
        glfwGetFramebufferSize(window->GetGLFWWindow(), &frameWidth, &frameHeight);
        //Update ubo stuff
        ubo.cameraWorldPos = glm::vec4(gameCamera->tf.GetPosition().toGLM(), 0.0f);
        ubo.mousePos = glm::vec4{ Input->GetMousePosX(),frameHeight - Input->GetMousePosY(),0.0f,0.0f };
        ubo.frameSize = glm::vec4{ frameWidth,frameHeight,0.0f,0.0f };
#ifdef USE_EDITOR
        ubo.vpData = glm::vec4{ EditorSystem::GetInstance()->getGraphicsWindow()->getWindowPos().x,
        frameHeight - (EditorSystem::GetInstance()->getGraphicsWindow()->getWindowPos().y + EditorSystem::GetInstance()->getGraphicsWindow()->getWindowSize().y),
            EditorSystem::GetInstance()->getGraphicsWindow()->getWindowSize() };

        if ((ubo.mousePos.x > ubo.vpData.x) && (ubo.mousePos.y > ubo.vpData.y) && (ubo.mousePos.x < ubo.vpData.x + ubo.vpData.z) && (ubo.mousePos.y < ubo.vpData.y + ubo.vpData.w))
        {
            ubo.pressed = Input->IsMouseTriggered(Mouse::MOUSE_LEFT) ? 1 : (Input->IsMousePressed(Mouse::MOUSE_LEFT) ? 2 : (Input->IsMouseReleased(Mouse::MOUSE_LEFT) ? 3 : 0));
        }
        else
        {
            ubo.pressed = 0;
        }
#else
        ubo.pressed = Input->IsMouseTriggered(Mouse::MOUSE_LEFT) ? 1 : (Input->IsMousePressed(Mouse::MOUSE_LEFT) ? 2 : (Input->IsMouseReleased(Mouse::MOUSE_LEFT) ? 3 : 0));
#endif

        std::vector<std::pair<float, EntityID>> translucentObjects;

        for (EntityID entity : ECS->GetTracker<RendererTracker>()->GetTrackedEntities())
        {
            auto& rendererComponent = ECS->GetComponent<Renderer>(entity);

            //Only continue if it exist
            if (rendererComponent.getModelID() && ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::active))
            {
                if (rendererComponent.getInitialized() == false)
                {
                    rendererComponent.initializeComponent(pickBuffer, uboBuffers, *beDevice, setLayout, pool, blankImageInfo);
                }

                if (rendererComponent.getCheckedForCustomMaps() == false)
                    rendererComponent.checkForCustomMaps();

                if (rendererComponent.getOpacity())
                {
                    if (rendererComponent.getOpacity() < 1.0f)
                    {
                        translucentObjects.emplace_back(std::pair<float, EntityID>(glm::distance(ECS->GetComponent<Transform>(entity).GetPosition().toGLM(), gameCamera->tf.GetPosition().toGLM()), entity));
                        continue;
                    }

                    if (ECS->HasComponent<AnimationComponent>(entity))
                    {
                        setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 0, true);

                        std::copy(ECS->GetComponent<AnimationComponent>(entity).GetFinalBoneMatrices().begin(), ECS->GetComponent<AnimationComponent>(entity).GetFinalBoneMatrices().end(),
                            ubo.boneMatrices);

                        rendererComponent.updateOwnUboAsItHasAnim(frameIndex, ubo, setLayout);
                    }
                    else
                    {
                        setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 0, false);

                        rendererComponent.setAlreadyUsingOwnUbo(false);
                    }

                    push.opacity = rendererComponent.getOpacity();
                    push.flickerColor = rendererComponent.getFlickerColor();
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 1, rendererComponent.getShouldFlicker());
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2, rendererComponent.getWireframeMode() || globalWireframeMode);
                    push.wireframeColor = rendererComponent.getWireframeColor();
                    push.modelMatrix = BE::ECS->GetComponent<BE::Transform>(entity).getModelMatrix();
                    push.normalMatrix = BE::ECS->GetComponent<BE::Transform>(entity).getNormalMatrix();
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 3, rendererComponent.getIgnoreLighting());
                    setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 0, rendererComponent.getReflectSpecular());

                    push.id = entity;

#ifdef USE_EDITOR
                    if (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2) && wasWireframe == false)
                    {
                        bePipelineWireframe->bind(frameInfo.commandBuffer);
                        wasWireframe = true;
                    }
                    else if (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2) == false && wasWireframe == true)
                    {
                        bePipeline->bind(frameInfo.commandBuffer);
                        wasWireframe = false;
                    }
#endif

                    for (int i{}; i < AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).getMeshes().size(); ++i)
                    {
                        //if (ECS->GetComponent<EntityInfo>(entity).GetName().find("NewCeiling") != std::string::npos)
                        //{
                        //    rendererComponent.setAlbedoAlphaMapwithName("../Assets/Textures/RoomPieces/CeilingPiece_DefaultMaterial_BaseColor.1001.dds", i);
                        //    rendererComponent.setRoughnessMapwithName("../Assets/Textures/RoomPieces/CeilingPiece_DefaultMaterial_Roughness.1001.dds", i);
                        //    rendererComponent.setMetallicMapwithName("../Assets/Textures/default/default_metallic.dds", i);
                        //    rendererComponent.setNormalMapwithName("../Assets/Textures/default/default_normal.dds", i);
                        //    rendererComponent.setHeightMapwithName("../Assets/Textures/default/default_height.dds", i);
                        //}
                        push.albedoStrength = rendererComponent.getTextureStrength(i)[0];
                        push.normalStrength = rendererComponent.getTextureStrength(i)[1];
                        push.aoStrength = rendererComponent.getTextureStrength(i)[2];
                        push.roughnessStrength = rendererComponent.getTextureStrength(i)[3];
                        push.emissiveStrength = rendererComponent.getTextureStrength(i)[4];
                        push.metallicStrength = rendererComponent.getTextureStrength(i)[5];
                        push.brightnessFactor = rendererComponent.getBrightnessFactor(i);

                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 1, rendererComponent.getMeshCustomAlbedoStatus(i));
                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 2, rendererComponent.getMeshCustomNormalStatus(i));
                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 3, rendererComponent.getMeshCustomAmbientOcclusionStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 0, rendererComponent.getMeshCustomRoughnessStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 1, rendererComponent.getMeshCustomEmissiveStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 2, rendererComponent.getMeshCustomMetallicStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 3, rendererComponent.getMeshCustomHeightStatus(i));

                        rendererComponent.addTexturesToUpdateList(i, *beDevice, setLayout, blankImageInfo, assetReloaded);
                        rendererComponent.updateDescriptorSet(*beDevice);

                        vkCmdPushConstants(frameInfo.commandBuffer, (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2)) ? pipelineLayoutWireframe : pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2)) ? pipelineLayoutWireframe : pipelineLayout, 0, 1, &rendererComponent.getDescriptorSet(i)[frameIndex], 0, nullptr);

                        AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).bind(frameInfo.commandBuffer, i);
                        AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).draw(frameInfo.commandBuffer, i);
                    }

                    if (rendererComponent.getShouldFlicker())
                    {
                        rendererComponent.setTimer(rendererComponent.getTimer() + static_cast<float>(Engine->GetDeltaTime()));

                        if (rendererComponent.getTimer() > rendererComponent.getFlickerDuration())
                        {
                            rendererComponent.setShouldFlicker(false);
                            rendererComponent.setTimer(0.0f);
                        }
                    }
                }
            }
        }

        //After initial rendering, sort vector based on distance
        std::sort(translucentObjects.begin(), translucentObjects.end(), [](std::pair<float, EntityID>& lhs, std::pair<float, EntityID>& rhs) {return lhs.first > rhs.first; });

        for (std::pair<float, EntityID>& entity : translucentObjects)
        {
            auto& rendererComponent = ECS->GetComponent<Renderer>(entity.second);

            //Only continue if it exist
            if (rendererComponent.getModelID() && ECS->GetComponent<EntityInfo>(entity.second).HasFlag(EntityFlag::active))
            {
                if (rendererComponent.getInitialized() == false)
                {
                    rendererComponent.initializeComponent(pickBuffer, uboBuffers, *beDevice, setLayout, pool, blankImageInfo);
                }

                if (rendererComponent.getCheckedForCustomMaps() == false)
                    rendererComponent.checkForCustomMaps();

                if (rendererComponent.getOpacity())
                {
                    if (ECS->HasComponent<AnimationComponent>(entity.second))
                    {
                        setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 0, true);

                        std::copy(ECS->GetComponent<AnimationComponent>(entity.second).GetFinalBoneMatrices().begin(), ECS->GetComponent<AnimationComponent>(entity.second).GetFinalBoneMatrices().end(),
                            ubo.boneMatrices);

                        rendererComponent.updateOwnUboAsItHasAnim(frameIndex, ubo, setLayout);
                    }
                    else
                    {
                        setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 0, false);
                        rendererComponent.setAlreadyUsingOwnUbo(false);
                    }

                    push.opacity = rendererComponent.getOpacity();
                    push.flickerColor = rendererComponent.getFlickerColor();
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 1, rendererComponent.getShouldFlicker());
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2, rendererComponent.getWireframeMode() || globalWireframeMode);
                    push.wireframeColor = rendererComponent.getWireframeColor();
                    push.modelMatrix = BE::ECS->GetComponent<Transform>(entity.second).getModelMatrix();
                    push.normalMatrix = BE::ECS->GetComponent<Transform>(entity.second).getNormalMatrix();
                    setBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 3, rendererComponent.getIgnoreLighting());
                    setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 0, rendererComponent.getReflectSpecular());
                    push.id = entity.second;

#ifdef USE_EDITOR
                    if (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2) && wasWireframe == false)
                    {
                        bePipelineWireframe->bind(frameInfo.commandBuffer);
                        wasWireframe = true;
                    }
                    else if (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2) == false && wasWireframe == true)
                    {
                        bePipeline->bind(frameInfo.commandBuffer);
                        wasWireframe = false;
                    }
#endif

                    for (int i{}; i < AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).getMeshes().size(); ++i)
                    {
                        push.albedoStrength = rendererComponent.getTextureStrength(i)[0];
                        push.normalStrength = rendererComponent.getTextureStrength(i)[1];
                        push.aoStrength = rendererComponent.getTextureStrength(i)[2];
                        push.roughnessStrength = rendererComponent.getTextureStrength(i)[3];
                        push.emissiveStrength = rendererComponent.getTextureStrength(i)[4];
                        push.metallicStrength = rendererComponent.getTextureStrength(i)[5];
                        push.brightnessFactor = rendererComponent.getBrightnessFactor(i);

                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 1, rendererComponent.getMeshCustomAlbedoStatus(i));
                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 2, rendererComponent.getMeshCustomNormalStatus(i));
                        setBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO, 3, rendererComponent.getMeshCustomAmbientOcclusionStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 0, rendererComponent.getMeshCustomRoughnessStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 1, rendererComponent.getMeshCustomEmissiveStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 2, rendererComponent.getMeshCustomMetallicStatus(i));
                        setBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight, 3, rendererComponent.getMeshCustomHeightStatus(i));

                        rendererComponent.addTexturesToUpdateList(i, *beDevice, setLayout, blankImageInfo, assetReloaded);
                        rendererComponent.updateDescriptorSet(*beDevice);

                        vkCmdPushConstants(frameInfo.commandBuffer, (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2)) ? pipelineLayoutWireframe : pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            (getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting, 2)) ? pipelineLayoutWireframe : pipelineLayout, 0, 1, &rendererComponent.getDescriptorSet(i)[frameIndex], 0, nullptr);

                        AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).bind(frameInfo.commandBuffer, i);
                        AssetMgr->GetAssetData<BeModel>(rendererComponent.getModelID()).draw(frameInfo.commandBuffer, i);
                    }

                    if (rendererComponent.getShouldFlicker())
                    {
                        rendererComponent.setTimer(rendererComponent.getTimer() + static_cast<float>(Engine->GetDeltaTime()));

                        if (rendererComponent.getTimer() > rendererComponent.getFlickerDuration())
                        {
                            rendererComponent.setShouldFlicker(false);
                            rendererComponent.setTimer(0.0f);
                        }
                    }
                }
            }
        }
    }

    void SimpleRenderSystem::Shutdown()
    {
        pickBuffer.reset();
        vkDestroyImage(beDevice->device(), blankImage, nullptr);
        vkDestroyImageView(beDevice->device(), blankImageView, nullptr);
        vkFreeMemory(beDevice->device(), blankImageMemories, nullptr);
        vkDestroySampler(beDevice->device(), blankImageSampler, nullptr);
        vkDestroyPipelineLayout(beDevice->device(), pipelineLayout, nullptr);
        bePipeline->destroyGraphicsPipeline();

        vkDestroyPipelineLayout(beDevice->device(), pipelineLayoutWireframe, nullptr);
        bePipelineWireframe->destroyGraphicsPipeline();
    }

    void SimpleRenderSystem::transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout)
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

    inline void SimpleRenderSystem::setBool(int& storage, int index, bool value)
    {
        storage &= ~(1 << index);
        storage |= (value << index);
    }

    inline bool SimpleRenderSystem::getBool(int& storage, int index)
    {
        return storage & (1 << index);
    }

    uint64_t SimpleRenderSystem::getSelectedID()
    {
        return static_cast<Info*>(pickBuffer->getMappedMemory())->selectedID;
    }

    float SimpleRenderSystem::getSelectedDistance()
    {
        return static_cast<Info*>(pickBuffer->getMappedMemory())->distance;
    }

    void SimpleRenderSystem::copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
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