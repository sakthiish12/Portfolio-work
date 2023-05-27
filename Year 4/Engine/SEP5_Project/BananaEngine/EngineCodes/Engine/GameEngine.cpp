#include "pch.h"
#include "GameEngine.h"
#include <Serialisation/BananaSerialisation.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <locale>
#include <codecvt>
#include "../../../BananaGame/GameCodes/BehaviourScripts/Misc/AmbienceSFX.h"
#include "../../../BananaGame/GameCodes/BehaviourScripts/Enemies/EnemyScript.h"

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<EngineConfig>(BE::RegistrationNameCreator<EngineConfig>())
	.constructor()()
	.property("m_maxFrameRate", &EngineConfig::m_maxFrameRate)
	.property("m_windowWidth", &EngineConfig::m_windowWidth)
	.property("m_windowHeight", &EngineConfig::m_windowHeight)
	.property("m_isFullScreen", &EngineConfig::m_isFullScreen)
	.property("m_audioChannels", &EngineConfig::m_audioChannels)
	.property("m_musicVolume", &EngineConfig::m_musicVolume)
	.property("m_soundVolume", &EngineConfig::m_soundVolume);
}
namespace BE
{
	GameEngine::GameEngine(const std::string& in)
		:m_windowName{in}
	{
		//Init RNG
		Rand::InitRNG();
		//Init the miscellaneous systems
		Debug::CreateInstance();
		EventDispatcher::CreateInstance();
		AssetManager::CreateInstance();
		ScriptManager::CreateInstance();
		SpeedControl::CreateInstance();
		//Load the engine configuration
		EngineConfig config = LoadConfig();
		m_windowWidth = config.m_windowWidth;
		m_windowHeight = config.m_windowHeight;
		m_isFullScreen = config.m_isFullScreen;
		
		m_desiredFPS = config.m_maxFrameRate;
		//Init engine configs
		
		m_deltaTime = 0.0;
		m_frameCount = 0;
		m_startTime = 0.0;
		m_endTime = 0.0;
		m_fixedDT = 1.0 / m_desiredFPS;
		m_accumulatedTime = 0.0;
		m_physSteps = 0;
		m_run = true;
		m_cappedDt = m_fixedDT * 2.0;

		//for physics system
		JPH::RegisterDefaultAllocator();

		//Create an instance of the systems
		//GLFW
		
		this->CreateSystem<InputController>();
		this->CreateSystem<GameLogicSystem>();
		this->CreateSystem<sPhysicsSystem>();
		this->CreateSystem<ECSCoordinator>();
		this->CreateSystem<UIManager>();
		this->CreateSystem<AnimationSystem>();
		this->CreateSystem<RendererSystem>();
#ifdef USE_EDITOR
		//Editor
		this->CreateSystem<EditorSystem>();
#endif
		this->CreateSystem<AudioManager>(config.m_audioChannels, config.m_musicVolume, config.m_soundVolume);
		this->CreateSystem<SceneManager>();



		//Subscribe events
		EventMgr->SubscribeWPrio(UnloadSceneEvent, this, GameEngine::OnSceneUnload, 4);
		EventMgr->SubscribeWPrio(SceneRestartingEvent, this, GameEngine::OnSceneUnload, 4);
		EventMgr->SubscribeWPrio(SceneLoadedEvent, this, GameEngine::OnSceneLoaded, 4);
		EventMgr->Subscribe(QuitEvent, this, GameEngine::OnQuit);
	}

	GameEngine::~GameEngine()
	{
		LogCore("Destroyed GameEngine");
	}

	void GameEngine::Init()
	{
		//Init all the systems
		for (auto& sys : m_systems)
			sys->Init();
	
		m_frameCount = 0;

		m_startTime = 0.0;
		m_endTime = 0.0;

		m_accumulatedTime = 0.0;
		m_physSteps = 0;

		m_run = true;
		glfwSetTime(0.0);
	}

	void GameEngine::Awake()
	{
		//Init all the systems
		for (auto& sys : m_systems)
			sys->Awake();
		
		m_frameCount = 0;

		m_startTime = glfwGetTime();
		m_endTime = glfwGetTime();

		m_accumulatedTime = 0.0;
		m_physSteps = 0;

		m_run = true;

		PauseControl::SetPause(false);
	}

	void GameEngine::Update()
	{
		//Get the new delta time value for the next frame
		m_endTime = glfwGetTime();
		m_deltaTime = m_endTime - m_startTime;

#if 0
		//Cap the DT if DT is too large
		if (m_deltaTime > m_cappedDt)
			m_deltaTime = m_cappedDt;
#endif
		/*------------ Start of new frame ----------------*/

		//Time at the start of current frame
		m_startTime = glfwGetTime();

		//Accumulate time for this frame
		m_accumulatedTime += m_deltaTime;
		m_physSteps = 0;

		//Until the frame has lasted longer than the fixed DT
		while (m_accumulatedTime >= m_fixedDT)
		{
			m_accumulatedTime -= m_fixedDT;
			++m_physSteps;
		}

		//Increase frame count
		++m_frameCount;

		//Update all the systems
		RendererSystem::GetInstance()->startRenderPass();
		//if (InputController::GetInstance()->IsKeyTriggered(Key::_L))
		//{
		//	LogCore("Triggered custom code\n");
		//	for (const auto entity : ECS->GetAllEntities())
		//	{
		//		if (ECS->GetComponent<EntityInfo>(entity).HasTag("AmbienceEmitter"))
		//		{
		//			if (!ECS->HasComponent<AudioSource>(entity))
		//				ECS->AddComponent<AudioSource>(entity);
		//			if (!ECS->HasComponent<GameLogic>(entity))
		//				ECS->AddComponent<GameLogic>(entity);
		//			if (!ECS->GetComponent<GameLogic>(entity).HasScript<TH::AmbienceSFX>())
		//				ECS->GetComponent<GameLogic>(entity).AddScript<TH::AmbienceSFX>();
		//			if (ECS->GetComponent<EntityInfo>(entity).GetName() == "Server")
		//			{
		//				ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(5535257091486974901));
		//				ECS->GetComponent<AudioSource>(entity).SetVolume(0.033f,AssetID(5535257091486974901));
		//				ECS->GetComponent<AudioSource>(entity).SetMaxDist(25.0f, AssetID(5535257091486974901));
		//				TH::AmbienceSFX& asfx = *ECS->GetComponent<GameLogic>(entity).GetScriptDerived<TH::AmbienceSFX>();
		//				asfx.lowerRange = 1.5f;
		//				asfx.upperRange = 4.0f;
		//			}
		//			else if (ECS->GetComponent<EntityInfo>(entity).GetName() == "CeilingDoor")
		//			{
		//				ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(8494876587477478932));
		//				ECS->GetComponent<AudioSource>(entity).SetVolume(0.075f, AssetID(8494876587477478932));
		//				ECS->GetComponent<AudioSource>(entity).SetMaxDist(40.0f, AssetID(8494876587477478932));
		//			}
		//			else if (ECS->GetComponent<EntityInfo>(entity).GetName() == "Computer")
		//			{
		//				ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(6099201371617927317));
		//				ECS->GetComponent<AudioSource>(entity).SetVolume(0.075f, AssetID(6099201371617927317));
		//				ECS->GetComponent<AudioSource>(entity).SetMaxDist(25.0f, AssetID(6099201371617927317));
		//				TH::AmbienceSFX& asfx = *ECS->GetComponent<GameLogic>(entity).GetScriptDerived<TH::AmbienceSFX>();
		//				asfx.lowerRange = 1.5f;
		//				asfx.upperRange = 4.0f;
		//			}
		//			std::cout << entity << " edited\n";
		//		}
		//		if (ECS->GetComponent<EntityInfo>(entity).GetName() == "BGMPlayer")
		//		{
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(8977411895274234169));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(0.3f, AssetID(8977411895274234169));
		//			ECS->GetComponent<AudioSource>(entity).SetIsBGM(true, AssetID(8977411895274234169));
		//		}
		//		if (ECS->GetComponent<EntityInfo>(entity).GetName() == "Baby Monkey")
		//		{
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(17521126164417407763));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(0.3f, AssetID(17521126164417407763));
		//		}
		//		
		//		if (ECS->HasComponent<GameLogic>(entity) && ECS->GetComponent<GameLogic>(entity).HasScript<TH::EnemyBase>())
		//		{
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(8629549385509738384));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(0.1f, AssetID(8629549385509738384));
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(6678469896061799648));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(1.0f, AssetID(6678469896061799648));
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(2470535914051851891));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(1.0f, AssetID(2470535914051851891));
		//			ECS->GetComponent<AudioSource>(entity).AddAudio(AssetID(7859595392513741910));
		//			ECS->GetComponent<AudioSource>(entity).SetVolume(1.0f, AssetID(7859595392513741910));
		//		}

		//	}
		//	
		//}
		for (auto& sys : m_systems)
		{
#ifdef USE_DEBUGGER
			//Profiling - how much time each system is using to run update
			sys->StartSystemDt();
			sys->Update(m_deltaTime);
			sys->EndSystemDt();
#else
			sys->Update(m_deltaTime);
#endif
		}
		RendererSystem::GetInstance()->endRenderPass();
	}

	void GameEngine::Reload()
	{
		for (auto& sys : m_systems)
			sys->Reload();
	}

	void GameEngine::Shutdown()
	{
		SaveConfig();

		for (auto& sys : m_systems)
			sys->Shutdown();

		//Unload all the assets here
		AssetMgr->DeleteInstance();
	}

	void GameEngine::OnSceneUnload(IEvent& e)
	{
		AssertCore(e.GetEventType() == UnloadSceneEvent::GetStaticType() || e.GetEventType() == SceneRestartingEvent::GetStaticType(), \
			"Wrong event, expected Scene unloading event or Scene restaring event");
		this->Reload();
	}

	void GameEngine::OnSceneLoaded(IEvent& e)
	{
		AssertCore(e.GetEventType() == SceneLoadedEvent::GetStaticType() || e.GetEventType() == SceneRestartingEvent::GetStaticType(), 
			"Wrong event, expected Scene unloading event or Scene restaring event");
#ifdef USE_EDITOR
		//If IMGUI is pausing, then don't update
		if (Debug::GetInstance()->GetDebugFlag(Debug::DebugFlags::imguiPause))
		{
			m_startTime = glfwGetTime();
			m_endTime = glfwGetTime();
			return;
		}
#endif
		this->Awake();
	}

	void GameEngine::OnQuit(IEvent& e)
	{
		AssertCore(e.GetEventType() == QuitEvent::GetStaticType(), "Wrong event, expected QuitEvent");

		RendererSystem::GetInstance()->GetBEWindow()->CloseWindow();
	}

	EngineConfig GameEngine::LoadConfig()
	{
		PWSTR path;
		SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
		std::wstring wremaining(m_windowName.begin(), m_windowName.end());
		std::wstring wstart = path;
		wstart.append(std::wstring(L"\\") + wremaining);
		CreateDirectoryW(wstart.c_str(), NULL);
		wstart.append(L"\\config.json");
		std::ifstream ifs(wstart);
		EngineConfig config;
		if (ifs.fail())
		{
			LogCoreError("Config file not found, using default values");
		}
		else
		{
			std::stringstream ss(std::string(std::istreambuf_iterator<char>(ifs), {}));
			rapidjson::Document d;
			d.Parse(ss.str().c_str());
			Serialisation::LoadObjectRecur(d["EngineConfig"], rttr::type::get<EngineConfig>(), NULL_ENTITY, config);
		}
		return config;
	}

	void GameEngine::SaveConfig()
	{
		EngineConfig config;
		config.m_windowWidth = m_windowWidth;
		config.m_windowHeight = m_windowHeight;
		config.m_isFullScreen = m_isFullScreen;
		config.m_maxFrameRate = m_desiredFPS;
		config.m_audioChannels = AudioManager::GetInstance()->GetMaxChannelCount();
		config.m_musicVolume = AudioManager::GetInstance()->GetGlobalMusicVol();
		config.m_soundVolume = AudioManager::GetInstance()->GetGlobalSoundVol();
		PWSTR path;
		SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
		std::wstring wremaining(m_windowName.begin(), m_windowName.end());
		std::wstring wstart = path;
		wstart.append(std::wstring(L"\\") + wremaining);
		CreateDirectoryW(wstart.c_str(), NULL);
		wstart.append(L"\\config.json");
		std::ofstream ofs(wstart, std::ofstream::out | std::ofstream::trunc);
		if (ofs.fail())
		{
			LogCoreError("Failed to save config!!!!");
			return;
		}
		using namespace rapidjson;
		Document d;
		Value& rootValue = d.SetObject();
		Value configVal;
		configVal.SetObject();
		Serialisation::SerializeToValueBase(config, configVal, d);
		rootValue.AddMember("EngineConfig", configVal, d.GetAllocator());
		OStreamWrapper osw(ofs);
		//Create a writer to write the document to the output stream
		PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		d.Accept(writer);
		ofs.close();
	}
}
