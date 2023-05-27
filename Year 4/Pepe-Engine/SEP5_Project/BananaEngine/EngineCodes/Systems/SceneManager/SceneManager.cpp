#include "pch.h"
#include "SceneManager.h"
#include "../../../../BananaGame/GameCodes/Application/Application.h"
namespace BE
{
	SceneManager::SceneManager()
	{
		//Init the values
		m_currScene = NULL_SCENE_ID;
		m_nextScene = NULL_SCENE_ID;
		m_flags = 0;

		//Add null scene and restart scene into the scene list 
		m_sceneList.emplace(NULL_SCENE_ID, NULL_SCENE);
		m_sceneList.emplace(LOADING_SCENE_ID, LOADING_SCENE);
		m_sceneIDList.emplace(NULL_SCENE.GetSceneName(), NULL_SCENE_ID);
		m_sceneIDList.emplace(LOADING_SCENE.GetSceneName(), LOADING_SCENE_ID);

		//Subscribing to events

		//
		EventMgr->Subscribe(SceneLoadedEvent, this, SceneManager::OnSceneLoaded);

		LogCore("Created Scene Manager");
	}

	SceneManager::~SceneManager()
	{
		LogCore("Destroyed Scene Manager");
	}

	//Creates a new scene given the scene constructor parameters. SceneID 0 is reserved for invalid.
	Scene& SceneManager::CreateScene(std::string&& _name, std::string&& _file, SceneID _id)
	{
		AssertCore(_id != NULL_SCENE_ID && _id != LOADING_SCENE_ID, "Cannot create a scene with ID %llu or %llu, \
			they are reserved.", *NULL_SCENE_ID, *LOADING_SCENE_ID);
		AssertCore(_name != NULL_SCENE.GetSceneName() && _name != LOADING_SCENE.GetSceneName(), "Cannot create\
			scene named %s or %s, they are reserved.", NULL_SCENE.GetSceneName().c_str(), LOADING_SCENE.GetSceneName().c_str());
		ConvertFilePathSlash(_file);
		//check whether the scene name exists
		if (m_sceneIDList.count(_name) != 0)
		{
			LogCoreError("Scene named \"%s\" already exists", _name.c_str());
			return m_sceneList.at(m_sceneIDList.at(_name));
		}

		//Check whether the scene ID exists
		if (m_sceneList.count(_id) != 0)
		{
			LogCoreError("Scene ID %llu already exists", *_id);
			return m_sceneList.at(_id);
		}
		
		//check whether scene file exists
		for (const auto& [sid, sObj]  : m_sceneList)
		{
			if (sObj.GetSceneFile() == _file)
			{
				LogCoreError("File name \"%s\" is already used by the scene \"%s\".", _file.c_str(), sObj.GetSceneName().c_str());
				return m_sceneList.at(sid);
			}
		}
		std::ifstream ifs(_file.c_str());
		if (ifs.fail())
		{
			std::ofstream ofs(_file.c_str(), std::ofstream::out | std::ofstream::trunc);
			ofs.close();
		}
		ifs.close();
		//Otherwise just create the scene or overwrite the values if it exists
		m_sceneList.emplace(_id, Scene(std::move(_name), std::move(_file), _id));
		m_sceneIDList.emplace(m_sceneList.at(_id).GetSceneName(), _id);
		
		return m_sceneList.at(_id);
	}

	//Creates a new scene by putting in the scene object itself. SceneID 0 is reserved for invalid.
	Scene& SceneManager::CreateScene(Scene&& _scene)
	{
		AssertCore(_scene.GetSceneID() != NULL_SCENE_ID && _scene.GetSceneID() != LOADING_SCENE_ID, "Cannot create a scene with ID %llu or %llu, \
			they are reserved.", *NULL_SCENE_ID, *LOADING_SCENE_ID);
		AssertCore(_scene.GetSceneName() != NULL_SCENE.GetSceneName() && _scene.GetSceneName() != LOADING_SCENE.GetSceneName(), "Cannot create\
			scene named %s or %s, they are reserved.", NULL_SCENE.GetSceneName().c_str(), LOADING_SCENE.GetSceneName().c_str());

		//check whether the scene name exists
		if (m_sceneIDList.count(_scene.GetSceneName()) != 0)
		{
			LogCoreError("Scene named \"%s\" already exists", _scene.GetSceneName().c_str());
			return m_sceneList.at(m_sceneIDList.at(_scene.GetSceneName()));
		}

		//Check whether the scene ID exists
		if (m_sceneList.count(_scene.GetSceneID()) != 0)
		{
			LogCoreError("Scene ID %llu already exists", *_scene.GetSceneID());
			return m_sceneList.at(_scene.GetSceneID());
		}

		//check whether scene file exists
		for (const auto& [sid, sObj] : m_sceneList)
		{
			if (sObj.GetSceneFile() == _scene.GetSceneFile())
			{
				LogCoreError("File name \"%s\" is already used by the scene \"%s\".", _scene.GetSceneFile().c_str(), sObj.GetSceneName().c_str());
				return m_sceneList.at(sid);
			}
		}

		SceneID sid = _scene.GetSceneID();
		//Otherwise just create the scene or overwrite the values if it exists
		m_sceneIDList.emplace(_scene.GetSceneName(), sid);
		m_sceneList.emplace(sid, std::move(_scene));

		return m_sceneList.at(sid);
	}

	//Removes a scene from the scene list, by Scene ID
	void SceneManager::DestroyScene(SceneID _id)
	{
		//Cannot destroy scene 0 or 1
		if (_id == NULL_SCENE_ID || _id == LOADING_SCENE_ID)
		{
			LogCoreError("Cannot destroy scene with ID %llu or %llu, they are reserved.", *NULL_SCENE_ID, *LOADING_SCENE_ID);
			return;
		}

		//Cannot destroy scene if it is currently the active or next scene
		if (_id == m_currScene || _id == m_nextScene)
		{
			LogCoreError("Cannot destroy a scene that is currently running or getting loaded next");
			return;
		}

		//If scene ID doesn't exist, then don't do anything
		if (m_sceneList.count(_id) == 0)
		{
			LogCoreError("Scene ID %llu does not exist", *_id);
			return;
		}

		m_sceneIDList.erase(m_sceneList.at(_id).GetSceneName());
		m_sceneList.erase(_id);
	}

	//Removes a scene from the scene list, by scene name
	void SceneManager::DestroyScene(const std::string& _sceneName)
	{
		//If Scene doesn't exist, then don't do anything
		if (m_sceneIDList.count(_sceneName) == 0)
		{
			LogCoreError("Scene %s does not exist", _sceneName.c_str());
			return;
		}

		DestroyScene(m_sceneIDList.at(_sceneName));
	}

	//Set which file to read during loading scene
	Scene& SceneManager::SetLoadingScene(std::string&& _file)
	{
		m_sceneList[LOADING_SCENE_ID].SetSceneFile(std::move(_file));

		/*
			Call Asset Manager Load function
		*/

		return m_sceneList.at(LOADING_SCENE_ID);
	}

	//Get the Loading Scene
	const Scene& SceneManager::GetLoadingScene() const
	{
		return m_sceneList.at(LOADING_SCENE_ID);
	}

	//Create all the scenes from a given file, use CreateScene inside to update the scene list
	void SceneManager::DeserialiseAllScenes(const std::string& _file)
	{
		std::ifstream ifs(_file, std::ifstream::in);

		if (!ifs.is_open())
		{
			LogCoreError("File named \"%s\" not opened!", _file.c_str());
			return;
		}

		std::stringstream sstream;
		sstream << ifs.rdbuf();

		rapidjson::Document doc;
		rapidjson::ParseResult result = doc.Parse<0>(sstream.str().c_str());

		AssertCore(result, "Error with parsing file \"%s\"! Parse error: %d (%zu)", _file.c_str(), (int)result.Code(), result.Offset());
		ifs.close();

		rapidjson::Value& v = doc["Scenes"];

		auto arr = v.GetArray();

		for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
		{
			AssertCore(arr[i].IsObject(), "Expected an object");
			
			//Assert for member 
			AssertCore(arr[i].HasMember("Scene ID"), "Missing member field \"Scene ID\"");
			AssertCore(arr[i].HasMember("Scene Name"), "Missing member field \"Scene Name\"");
			AssertCore(arr[i].HasMember("Scene File"), "Missing member field \"Scene File\"");

			//Assert for variable types
			AssertCore(arr[i]["Scene ID"].IsUint64(), "Expected Scene ID to be Uint64");
			AssertCore(arr[i]["Scene Name"].IsString(), "Expected Scene ID to be String");
			AssertCore(arr[i]["Scene File"].IsString(), "Expected Scene ID to be String");

			SceneID id(arr[i]["Scene ID"].GetUint64());
			std::string name(arr[i]["Scene Name"].GetString());
			std::string file(arr[i]["Scene File"].GetString());

			this->CreateScene(std::move(name), std::move(file), id);
		}

	}

	//Save all the scenes into a given file
	void SceneManager::SerialiseAllScenes(const std::string& _file)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ buffer };

		writer.StartObject();
		writer.Key("Scenes");
		writer.StartArray();

		//For each scene
		for (const auto& scene : std::views::values(m_sceneList))
		{
			//Do not save reserved scene
			if (scene.GetSceneID() == NULL_SCENE_ID || scene.GetSceneID() == LOADING_SCENE_ID)
				continue;

			writer.StartObject();

			//ID
			writer.Key("Scene ID");
			writer.Uint64(scene.GetSceneID());

			//Scene name
			writer.Key("Scene Name");
			writer.String(scene.GetSceneName().c_str());

			//Scene file
			writer.Key("Scene File");
			writer.String(scene.GetSceneFile().c_str());

			writer.EndObject();
		}

		writer.EndArray();
		writer.EndObject();

		//Output to json
		std::ofstream ofs(_file, std::ofstream::out | std::ofstream::trunc);
		if (!ofs.good())
		{
			LogCoreError("Unable to write Json string to file named \"%s\"!", _file.c_str());
			buffer.Clear();
			ofs.close();
			return;
		}

		ofs << buffer.GetString();
		ofs.close();
		buffer.Clear();
	}

	//Change to another scene by ID, cannot set to 0 or 1
	void SceneManager::LoadScene(SceneID _id)
	{
		//Cannot set to 0 or 1
		if (_id == NULL_SCENE_ID || _id == LOADING_SCENE_ID)
		{
			LogCoreError("Cannot change to scenes with ID %llu or %llu, they are reserved.", *NULL_SCENE_ID, *LOADING_SCENE_ID);
			return;
		}

		//If scene doesn't exist, then don't change
		if (m_sceneList.count(_id) == 0)
		{
			LogCoreError("Scene ID %llu was not loaded", *_id);
			return;
		}

		m_nextScene = _id;
		m_flags.set((size_t)SceneMgrFlag::loadNextFrame);
#ifdef USE_EDITOR
		BE::EditorSystem::GetInstance()->getToolBar()->resetButtons();
		BE::EditorSystem::GetInstance()->getToolBar()->setSceneState(SceneState::Pause);
		BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
#endif
	}

	//Change to another scene by name
	void SceneManager::LoadScene(const std::string& _sceneName)
	{
		//Cannot change to scene scene with "Invalid" or "Restart" as the name
		if (_sceneName == NULL_SCENE.GetSceneName() || _sceneName == LOADING_SCENE.GetSceneName())
		{
			LogCoreError("Cannot change to scenes named %s or %s, they are reserved.", \
				NULL_SCENE.GetSceneName().c_str(), LOADING_SCENE.GetSceneName().c_str());

			return;
		}
		//If scene doesn't exist, then don't change
		if (m_sceneIDList.count(_sceneName) == 0)
		{
			LogCoreError("Scene named %s was not loaded", _sceneName.c_str());
			return;
		}
		//using namespace BE;

		m_nextScene = m_sceneIDList.at(_sceneName);
		m_flags.set((size_t)SceneMgrFlag::loadNextFrame);
//#ifdef USE_EDITOR
//		BE::EditorSystem::GetInstance()->getToolBar()->resetButtons();
//		BE::EditorSystem::GetInstance()->getToolBar()->setSceneState(SceneState::Edit);
//		BE::Debug::GetInstance()->SetDebugFlag(BE::Debug::DebugFlags::imguiPause, true);
//#endif
	}

	//Set the flag to restart scene
	void SceneManager::RestartScene()
	{
		m_flags.set((size_t)SceneMgrFlag::isRestart);
		m_flags.set((size_t)SceneMgrFlag::loadNextFrame);
	}

	//Set the flag to quit application
	void SceneManager::QuitScene()
	{
		m_flags.set((size_t)SceneMgrFlag::isQuiting);
	}

	void SceneManager::checkedNewFrame()
	{
		m_flags.set((size_t)SceneMgrFlag::loadNextFrame,false);
	}

	//Get scene in the scene list by ID
	Scene& SceneManager::GetSceneByID(SceneID _id)
	{
		//If scene exists
		if(m_sceneList.count(_id) != 0)
			return m_sceneList.at(_id);
		else
		{
			LogCoreError("Scene ID %llu was not loaded", *_id);
			return m_sceneList.at(NULL_SCENE_ID);
		}
	}

	//Get scene in the scene list by scene name
	Scene& SceneManager::GetSceneByName(const std::string& _sceneName)
	{
		//If scene exists
		if (m_sceneIDList.count(_sceneName) != 0)
			return m_sceneList.at(m_sceneIDList.at(_sceneName));
		else
		{
			LogCoreError("Scene named %s was not loaded", _sceneName.c_str());
			return m_sceneList.at(NULL_SCENE_ID);
		}
	}

	void SceneManager::Init() 
	{

	}

	void SceneManager::Awake()
	{

	}

	void SceneManager::Update(double dt)
	{
		UNUSED(dt);
		//If current and next scene IDs are different, and loading has not yet started && its the start of a new frame
		if (m_currScene != m_nextScene && !m_flags.test((size_t)SceneMgrFlag::loadNextFrame))
		{	
			AssertCore(m_sceneList.contains(m_nextScene), "Given next scene is not loaded!");

			//Starts the loading
			m_flags.set((size_t)SceneMgrFlag::isLoading);

			//Unload the current scene first - Engine calls reload, asset manager clears all assets and loads new assets
			UnloadSceneEvent sul(m_sceneList.at(m_nextScene));
			EventMgr->Dispatch(sul);
	
			//------- At this point everything has been unloaded  ----------
#if 0
			//Start loading the assets
			LoadAssetEvent ale(m_sceneList.at(m_nextScene));
			EventMgr->Dispatch(ale);
#endif
			//-------- Load entities and UI for the next scene ------------------------------------------

			Serialisation::LoadEntities(m_sceneList.at(m_nextScene).GetSceneFile());

			//------- At this point, entities and UI for new scene has been loaded -----

			//Flag that the scene has finished loading, call Awake on all systems to initialise
			SceneLoadedEvent sl;
			EventMgr->Dispatch(sl);

			//Finished the loading
			m_flags.set((size_t)SceneMgrFlag::isLoading, false);
		}
		//If it's flagged to restart scene
		else if (m_flags.test((size_t)SceneMgrFlag::isRestart) && !m_flags.test((size_t)SceneMgrFlag::loadNextFrame))
		{
			m_flags.set((size_t)SceneMgrFlag::isLoading);

			//Flag the scene as going to restart, engine will call reload 
			SceneRestartingEvent sr;
			EventMgr->Dispatch(sr);

			//-------- Load entities and UI for the next scene ------------------------------------------
			Serialisation::LoadEntities(m_sceneList.at(m_currScene).GetSceneFile());

			//Flag that the scene has finished loading, call Awake on all systems to initialise
			SceneLoadedEvent sl;
			EventMgr->Dispatch(sl);

			m_flags.set((size_t)SceneMgrFlag::isRestart, false);
			m_flags.set((size_t)SceneMgrFlag::isLoading, false);
		}
		//If it's flagged to quit application
		else if (m_flags.test((size_t)SceneMgrFlag::isQuiting))
		{
			QuitEvent q;
			EventMgr->Dispatch(q);
		}

	}

	void SceneManager::Reload()
	{

	}

	void SceneManager::Shutdown()
	{

	}

	void SceneManager::OnSceneLoaded(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::sceneFinishedLoading, "Wrong event, expected SceneLoadedEvent, passed in %s instead.", e.GetEventName());

		//Scene is no longer loading
		m_flags.set((size_t)SceneMgrFlag::isLoading, false);

		//next scene is now current scene
		m_currScene = m_nextScene;
	}
}