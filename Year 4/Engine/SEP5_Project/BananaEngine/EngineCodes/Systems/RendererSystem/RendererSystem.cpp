#pragma once
#include "pch.h"
#include "RendererSystem.h"
#include "../ECS/ECS.h"
#include "../SceneManager/SceneManager.h"


#pragma warning ( push )
#pragma warning ( disable: 4458 )
#pragma warning (disable:26812)
namespace BE
{
	void RendererSystem::Init()
	{
        globalPool =
            BE::BeDescriptorPool::Builder(*beDevice)
            .setMaxSets(BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        texturedPool =
            BE::BeDescriptorPool::Builder(*beDevice)
            .setMaxSets(15000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 5000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5000)
            .build();

        uiPool =
            BE::BeDescriptorPool::Builder(*beDevice)
            .setMaxSets(1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .build();

        textPool =
            BeDescriptorPool::Builder(*beDevice)
            .setMaxSets(1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .build();
        ParticlePool =
            BeDescriptorPool::Builder(*beDevice)
            .setMaxSets(1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .build();

        globalSetLayout =
            BE::BeDescriptorSetLayout::Builder(*beDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        texturedSetLayout =
            BE::BeDescriptorSetLayout::Builder(*beDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //added by don for samplers
            .addBinding(10, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        uiSetLayout =
            BE::BeDescriptorSetLayout::Builder(*beDevice)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        textSetLayout =
            BeDescriptorSetLayout::Builder(*beDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        particleSetLayout =
            BE::BeDescriptorSetLayout::Builder(*beDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT)
            .build();

#ifdef USE_EDITOR
        simpleRenderSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), texturedSetLayout->getDescriptorSetLayout());
        pointLightSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), globalSetLayout->getDescriptorSetLayout());
        gridSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), globalSetLayout->getDescriptorSetLayout());
        uiRenderSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), uiSetLayout->getDescriptorSetLayout());
        textRenderSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), textSetLayout->getDescriptorSetLayout(),textPool,static_cast<float>(*MOffScreen->getPass().width),static_cast<float>(*MOffScreen->getPass().height));
        particleRenderSystem.setupSystem(beDevice, MOffScreen->getRenderPass(), particleSetLayout->getDescriptorSetLayout());

#else
        simpleRenderSystem.setupSystem(beDevice, beRenderer->getSwapChainRenderPass(), texturedSetLayout->getDescriptorSetLayout());
        pointLightSystem.setupSystem(beDevice, beRenderer->getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        uiRenderSystem.setupSystem(beDevice, beRenderer->getSwapChainRenderPass(), uiSetLayout->getDescriptorSetLayout());
        particleRenderSystem.setupSystem(beDevice, beRenderer->getSwapChainRenderPass(), particleSetLayout->getDescriptorSetLayout());
        textRenderSystem.setupSystem(beDevice, beRenderer->getSwapChainRenderPass(), textSetLayout->getDescriptorSetLayout(), textPool,
            static_cast<float>(beRenderer->getSwapChain()->getSwapChainExtent().width), static_cast<float>(beRenderer->getSwapChain()->getSwapChainExtent().height));
#endif

        for (int i{}; i < BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            uboBuffers.emplace_back(nullptr);
            globalDescriptorSets.emplace_back(VK_NULL_HANDLE);

            uboBuffers[i] = std::make_unique<BE::BeBuffer>(
                beDevice,
                sizeof(BE::GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();

            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            BE::BeDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
        ubo.projection = GameCamera.cc.getProjection();
        ubo.view = GameCamera.cc.getView();
        ubo.inverseView = GameCamera.cc.getInverseView();
    }

	void RendererSystem::Awake()
	{
	}

	void RendererSystem::Update(double)
	{
        if (canRender) {
            #ifdef USE_EDITOR
            EditorSystem::GetInstance()->newFrame(); //can be changed to isystem
            #endif
            GameCamera.cc.setViewYXZ(GameCamera.tf.GetPosition(), GameCamera.tf.GetRotation());
            GameCamera.cc.setPerspectiveProjection(glm::radians(50.f), beRenderer->getAspectRatio(), 0.1f, MAX_UPDATE_DISTANCE);
            int frameIndex = beRenderer->getFrameIndex();
            BE::FrameInfo frameInfo{
                frameIndex,
                frameTime,
                renderCommandBuffer,
                globalDescriptorSets[frameIndex]};

            // update
#ifdef USE_EDITOR
            ubo.projection = GameCamera.cc.getProjection();
            ubo.view = GameCamera.cc.getView();
            ubo.inverseView = GameCamera.cc.getInverseView();
#else
            if (sPhysicsSystem::GetInstance()->getUpdatedThisFrame())
            {
                ubo.projection = GameCamera.cc.getProjection();
                ubo.view = GameCamera.cc.getView();
                ubo.inverseView = GameCamera.cc.getInverseView();
            }
#endif

            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            MOffScreen->start(frameInfo);
            
            #ifdef USE_EDITOR
            if(showGrid)
                gridSystem.render(frameInfo);

            simpleRenderSystem.renderGameObjects(frameInfo, ubo, uboBuffers, frameIndex, *texturedSetLayout, *texturedPool, &GameCamera,beWindow,reloadedAssets,globalWireframeMode);
            pointLightSystem.render(frameInfo);
            EditorSystem::GetInstance()->getGraphicsWindow()->run(MOffScreen);
            uiRenderSystem.renderGameObjects(frameInfo, frameIndex, *uiSetLayout, *uiPool, &GameCamera);
            textRenderSystem.renderText(frameInfo,ubo);
            particleRenderSystem.renderGameObjects(frameInfo, frameIndex, *particleSetLayout, *ParticlePool, GameCamera);
            #endif
            
            MOffScreen->end(frameInfo);

            beRenderer->beginSwapChainRenderPass(renderCommandBuffer);
            #ifndef USE_EDITOR //comment when in gameplay mode.
            simpleRenderSystem.renderGameObjects(frameInfo, ubo, uboBuffers, frameIndex, *texturedSetLayout, *texturedPool, &GameCamera, beWindow, reloadedAssets, globalWireframeMode);
            pointLightSystem.render(frameInfo);
            uiRenderSystem.renderGameObjects(frameInfo, frameIndex, *uiSetLayout, *uiPool, &GameCamera);
            textRenderSystem.renderText(frameInfo, ubo);
            #endif

            #ifdef USE_EDITOR
            EditorSystem::GetInstance()->render(renderCommandBuffer);
            #endif
        }
	}

	void RendererSystem::Reload()
	{  
        for (EntityID entity : ECS->GetTracker<RendererTracker>()->GetTrackedEntities())
        {
            ECS->GetComponent<Renderer>(entity).release(beDevice, *texturedPool);
        }
        texturedPool->resetPool();
	}

	void RendererSystem::Shutdown()
	{
        simpleRenderSystem.Shutdown();
        pointLightSystem.Shutdown();
        particleRenderSystem.Shutdown();
#ifdef USE_EDITOR
        gridSystem.Shutdown();
#endif
        uiRenderSystem.Shutdown();
        textRenderSystem.Shutdown();
        


        globalPool.reset();
        texturedPool.reset();
        uiPool.reset();
        textPool.reset();
        ParticlePool.reset();
        globalSetLayout.reset();
        texturedSetLayout.reset();
        uiSetLayout.reset();
        textSetLayout.reset();
        particleSetLayout.reset();
        for (int i{}; i < uboBuffers.size(); ++i)
        {
            uboBuffers[i].reset();
        }
	}	

    void RendererSystem::setupRenderer(BeWindow& w, BeDevice& d, BeRenderer& r, OffScreen& s)
    {
        beWindow = &w;
        beDevice = &d;
        beRenderer = &r;
        MOffScreen = &s;
    }

    void RendererSystem::startRenderPass()
    {
        newTime = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

#ifdef USE_EDITOR
        cameraController.UpdateEditorCamera((float)Engine->GetDeltaTime(), GameCamera.tf);
        // cameraController.MoveInPlaneXZ(beWindow->getGLFWwindow(), frameTime, GameCamera.tf);
#endif

        //GameCamera.cc.setViewYXZ(GameCamera.tf.GetPositon().toGLM(), GameCamera.tf.GetEulerRotation().toGLM());
     

        if (SceneMgr->GetSceneMgrFlag(SceneMgrFlag::loadNextFrame) == true)
        {
            SceneMgr->checkedNewFrame();
        }
        else if (AssetMgr->getWaitingToRegenerateAssets() == true)
        {
            AssetMgr->setWaitingToRegenerateAssets(false);
            AssetMgr->setClearToRegenerateAssets(true);
        }
        else if (SceneMgr->GetSceneMgrFlag(SceneMgrFlag::loadNextFrame) == false && !SceneMgr->GetSceneMgrFlag(SceneMgrFlag::isRestart)
            && !SceneMgr->GetSceneMgrFlag(SceneMgrFlag::isLoading) && AssetMgr->getRegeneratingAssets() == false && AssetMgr->getClearToRegenerateAssets() == false)
        {
            renderCommandBuffer = beRenderer->beginFrame();
            if (renderCommandBuffer != VK_NULL_HANDLE) {
                canRender = true;
            }
        }
        else if(BE::AssetMgr->getClearToRegenerateAssets())
        {
            BE::AssetMgr->UnloadAllAssets(true);
            BE::AssetMgr->Deserialise("../Assets/asset.json");
            BE::AssetMgr->setClearToRegenerateAssets(false);
            BE::AssetMgr->setRegeneratingAssets(false);
            BE::AssetMgr->setWaitingToRegenerateAssets(false);
            uiRenderSystem.setAllUIToRefreshTextures();
            reloadedAssets = true;
        }
    }

    void RendererSystem::endRenderPass()
    {
        if (canRender)
        {
            beRenderer->endSwapChainRenderPass(renderCommandBuffer);
            beRenderer->endFrame();
            renderCommandBuffer = VK_NULL_HANDLE;
            reloadedAssets = false;

            vkDeviceWaitIdle(beDevice->device());
            canRender = false;
#ifdef USE_EDITOR
            EditorSystem::GetInstance()->getGraphicsWindow()->destroyImage(EditorSystem::GetInstance()->getGraphicsWindow()->getImage());
#endif
        }
    }

    bool RendererSystem::getCanRender()
    {
        return canRender;
    }

    void RendererSystem::setCurrentTime(std::chrono::steady_clock::time_point t)
    {
        currentTime = t;
    }

    uint64_t RendererSystem::getSelectedID()
    {
        return simpleRenderSystem.getSelectedID();
    }

    GlobalUbo& RendererSystem::getUBO()
    {
        return ubo;
    }

    BeDevice* RendererSystem::getDevice()
    {
        return beDevice;
    }

    void RendererSystem::flipEntitiesOneTime()
    {
        if (Input->IsKeyReleased(Key::_H))
        {
            for (EntityID id : ECS->GetAllEntities())
            {
                Transform& infoTrans = ECS->GetComponent<Transform>(id);

                //Only flip those were affected
                    //Get all existing children
                std::vector<EntityID> idOfChildren = infoTrans.GetChildren();

                //if it has children, detach all first
                for (size_t i{}; i < idOfChildren.size(); ++i)
                {
                    infoTrans.DetachChild(idOfChildren[i]);
                }

                infoTrans.SetEulerRotation(Vec3f{ infoTrans.GetEulerRotation().x + 90.0f,infoTrans.GetEulerRotation().y,infoTrans.GetEulerRotation().z });
                infoTrans.SetScale(Vec3f{ infoTrans.GetScale().x,infoTrans.GetScale().z,infoTrans.GetScale().y });

                for (size_t i{}; i < idOfChildren.size(); ++i)
                {
                    infoTrans.AttachChild(idOfChildren[i]);
                }
            }
        }
    }
    bool& RendererSystem::getGlobalWireframeMode()
    {
        return globalWireframeMode;
    }
    void RendererSystem::setGlobalWireframeMode(bool b)
    {
        globalWireframeMode=b;
    }
    bool& RendererSystem::getShowGrid()
    {
        return showGrid;
    }
}
#pragma warning(pop)