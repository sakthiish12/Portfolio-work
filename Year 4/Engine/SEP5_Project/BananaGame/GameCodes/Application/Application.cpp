#include "pch.h"
#include "Application.h"
#include "../AllBehaviour.h"

Application::Application(const std::string& in) : m_engine{ BE::GameEngine::CreateInstance(in) },
beWindow(BE::Engine->GetWindowWidth(), BE::Engine->GetWindowHeight(), BE::Engine->GetWindowName())
{
    //Behaviour registration
    BE::ScriptAPI->RegisterScript<TH::CharacterScript>();
    BE::ScriptAPI->RegisterScript<TH::PlayerSurround>();
    BE::ScriptAPI->RegisterScript<TH::CharacterPather>();
    BE::ScriptAPI->RegisterScript<TH::VirtualCharacter>();
    BE::ScriptAPI->RegisterScript<TH::PlayerCharacter>();
    BE::ScriptAPI->RegisterScript<TH::PlayerInteract>();
    BE::ScriptAPI->RegisterScript<TH::EnemyBase>();
    BE::ScriptAPI->RegisterScript<TH::EnemyRobot>();
    BE::ScriptAPI->RegisterScript<TH::EnemyScientist>();
    BE::ScriptAPI->RegisterScript<TH::DOTObject>();
    BE::ScriptAPI->RegisterScript<TH::SpinningLasers>();
    BE::ScriptAPI->RegisterScript<TH::ElectricTowers>();
    BE::ScriptAPI->RegisterScript<TH::BossMinionSpawner>();
    BE::ScriptAPI->RegisterScript<TH::RobotBoss>();
    BE::ScriptAPI->RegisterScript<TH::BossDecoy>();
    BE::ScriptAPI->RegisterScript<TH::BabyMonkey>();
    BE::ScriptAPI->RegisterScript<TH::Projectile>();
    BE::ScriptAPI->RegisterScript<TH::CameraController>();
    BE::ScriptAPI->RegisterScript<TH::Interactable>();
    BE::ScriptAPI->RegisterScript<TH::DoorScript>();
    BE::ScriptAPI->RegisterScript<TH::BananaPickUp>();
    BE::ScriptAPI->RegisterScript<TH::MilkBottlePickUp>();
    BE::ScriptAPI->RegisterScript<TH::LevelManager>();
    BE::ScriptAPI->RegisterScript<TH::LevelScript>();
    BE::ScriptAPI->RegisterScript<TH::LevelOne>();
    BE::ScriptAPI->RegisterScript<TH::ScientistManager>();
    BE::ScriptAPI->RegisterScript<TH::RobotManager>();
    BE::ScriptAPI->RegisterScript<TH::Teleporter>();
    BE::ScriptAPI->RegisterScript<TH::DoorUnlockTrigger>();
    BE::ScriptAPI->RegisterScript<TH::AggroPlayerTrigger>();
    BE::ScriptAPI->RegisterScript<TH::EndPoint>();
    BE::ScriptAPI->RegisterScript<TH::StressTest>();
    BE::ScriptAPI->RegisterScript<TH::IGMusicPlayer>();
    BE::ScriptAPI->RegisterScript<TH::MMMusicPlayer>();
    BE::ScriptAPI->RegisterScript<TH::MMPlayButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::MMQuitButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::MMCreditsButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::MMControlsButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::MMOptionsButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::BackButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGResumeGameButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGRestartButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGControlsButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGOptionsButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGQuitGameButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGBackButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGRetryButtonScript>();
    BE::ScriptAPI->RegisterScript<TH::IGAbilityOuterRingAnimation>();
    BE::ScriptAPI->RegisterScript<TH::AbilityButton>();
    BE::ScriptAPI->RegisterScript<TH::MMTeeheeAnimationScript>();
    BE::ScriptAPI->RegisterScript<TH::GlassScript>();
    BE::ScriptAPI->RegisterScript<TH::SFXPlayer>();
    BE::ScriptAPI->RegisterScript<TH::DialogueController>();
    BE::ScriptAPI->RegisterScript<TH::CutsceneController>();
    BE::ScriptAPI->RegisterScript<TH::ObjectivesController>();
    BE::ScriptAPI->RegisterScript<TH::CDOTriggers>();
    BE::ScriptAPI->RegisterScript<TH::LevelFinisher>();
    BE::ScriptAPI->RegisterScript<TH::BabyPointerUI>();
    BE::ScriptAPI->RegisterScript<TH::PathFinder>();
    BE::ScriptAPI->RegisterScript<TH::MilkBottleUIController>();
    BE::ScriptAPI->RegisterScript<TH::TutorialUIController>();
    BE::ScriptAPI->RegisterScript<TH::OffRobot>();
    BE::ScriptAPI->RegisterScript<TH::Beam>();
    BE::ScriptAPI->RegisterScript<TH::InteractableConsole>();
    BE::ScriptAPI->RegisterScript<TH::Turret>();
    BE::ScriptAPI->RegisterScript<TH::TurretBullet>();
    BE::ScriptAPI->RegisterScript<TH::RotationScript>();
    BE::ScriptAPI->RegisterScript<TH::AreaZone>();
    BE::ScriptAPI->RegisterScript<TH::MusicSliderController>();
    BE::ScriptAPI->RegisterScript<TH::SoundSliderController>();
    BE::ScriptAPI->RegisterScript<TH::Checkpoint>();
    BE::ScriptAPI->RegisterScript<TH::CheckpointManager>();
    BE::ScriptAPI->RegisterScript<TH::IGGameOverLevelCompleteAnimation>();
    BE::ScriptAPI->RegisterScript<TH::AmbienceSFX>();
    BE::ScriptAPI->RegisterScript<TH::AnimatedTexture>();
    BE::ScriptAPI->RegisterScript<TH::ObjectivesUIDisabler>();
    BE::ScriptAPI->RegisterScript<TH::BossRoomTrigger>();

    BE::RendererSystem::GetInstance()->setupRenderer(beWindow, beDevice, beRenderer, MOffScreen);
    BE::RendererSystem::GetInstance()->GameCamera.tf.SetPosition(BE::Vec3f{ 0.0f,1.0f,-5.0f });
    BE::AssetManager::GetInstance()->setDevice(&beDevice);
    BE::InputController::GetInstance()->passGLFWwindow(beWindow.GetGLFWWindow());
    BE::GameEngine::GetInstance()->passGLFWwindow(beWindow.GetGLFWWindow());

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    if (!BE::GameEngine::GetInstance()->GetIsFullScreen())
    {
        glfwSetWindowMonitor(beWindow.GetGLFWWindow(), NULL, 20, 20, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(beWindow.GetGLFWWindow(), primary, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    BE::WindowResizeEvent wre(mode->width, mode->height);
    BE::EventMgr->Dispatch(wre);
#ifdef USE_EDITOR
    BE::EditorSystem::GetInstance()->setupSystem(beWindow, beDevice, beRenderer.getSwapChainRenderPass(), beRenderer.getImageCount());
#endif
}

Application::~Application()
{
}

void Application::Init()
{
	m_engine->Init();
    
#ifndef USE_EDITOR
    BE::AssetManager::GetInstance()->Deserialise("../Assets/asset.json");
    BE::SceneMgr->DeserialiseAllScenes("../Scene/AllScenes.json");
    BE::SceneMgr->LoadScene("MainMenu");
#endif
}

void Application::Awake()
{
    m_engine->Awake();
}

void Application::Run()
{
    BE::RendererSystem::GetInstance()->setCurrentTime(std::chrono::high_resolution_clock::now());

    while (!BE::SceneMgr->GetSceneMgrFlag(BE::SceneMgrFlag::isQuiting))
    {
        while (!beWindow.shouldClose()) 
        {
            BE::PauseControl::SetPause((glfwGetWindowAttrib(beWindow.GetGLFWWindow(), GLFW_FOCUSED) == 0));
            glfwPollEvents();
            m_engine->Update();
        }
        vkDeviceWaitIdle(beDevice.device());
        BE::SceneMgr->QuitScene();
    }
}

void Application::Shutdown()
{
	m_engine->Shutdown();
}
