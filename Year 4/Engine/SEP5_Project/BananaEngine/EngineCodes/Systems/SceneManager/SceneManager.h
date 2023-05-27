#pragma once
#ifndef	SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../ISystem.h"

namespace BE
{
	enum class SceneMgrFlag
	{
		isQuiting,			//Prompt quitting of application
		isRestart,			//Prompt restarting of scene
		isLoading,			//Whether loading of scene is happening
		loadNextFrame,
		last
	};

	class SceneManager : public Singleton<SceneManager>, public ISystem
	{
		SceneID m_currScene;		//The current scene being run
		SceneID m_nextScene;		//The scene to change to

		//Flags for the scene manager
		std::bitset<(size_t)SceneMgrFlag::last> m_flags;

		//List of scenes loaded - Maps Scene ID to Scene itself
		std::unordered_map<SceneID, Scene> m_sceneList;

		//List of scenes loaded - Maps Scene Name to SceneID
		std::unordered_map<std::string, SceneID> m_sceneIDList;

		//Constructor/Destructor
		SceneManager();
		~SceneManager();

	public:
		/*
			Creating and Destroying a scene object - Updates the scene list.
		*/
		//Creates a new scene given the scene constructor parameters. SceneID 0 is reserved for invalid.
		Scene& CreateScene(std::string&& _name, std::string&& _file, SceneID _id = SceneID{});

		//Creates a new scene by putting in the scene object itself. SceneID 0 is reserved for invalid.
		Scene& CreateScene(Scene&& _scene);

		//Removes a scene from the scene list, by Scene ID
		void DestroyScene(SceneID _id);

		//Removes a scene from the scene list, by scene name
		void DestroyScene(const std::string& _sceneName);

		//Set Loading Scene, which sets the file to read during loading scene
		Scene& SetLoadingScene(std::string&& _file);

		//Get the Loading Scene
		const Scene& GetLoadingScene() const;



		/*
			Serialisation and Deserialisation of all the scenes
		*/
		//Create all the scenes from a given file, use CreateScene inside to update the scene list
		void DeserialiseAllScenes(const std::string& _file);

		//Save all the scenes into a given file
		void SerialiseAllScenes(const std::string& _file);



		/*
			Switching scenes
		*/
		//Prompt changing to a new scene, cannot set to 0 or 1
		void LoadScene(SceneID _id);

		//Prompt changing to a new scene
		void LoadScene(const std::string& _sceneName);

		//Prompt restarting of scene
		void RestartScene();

		//Prompt quitting of application
		void QuitScene();

		//Use externally to signal that it is a start of a new frame to do the load/restart
		void checkedNewFrame();

		/*
			Getters
		*/
		//Return the number of scenes loaded into the scene manager
		size_t GetSceneCount() const							{ return m_sceneList.size(); }

		//Get the scene that is currently active
		inline Scene& GetCurrentScene()							{ return m_sceneList.at(m_currScene); }

		//Get the scene that is going to be loaded
		inline Scene& GetNextScene()							{ return m_sceneList.at(m_nextScene); }

		//Get the scene manager flag
		inline bool GetSceneMgrFlag(SceneMgrFlag _f) const		{ return m_flags.test((size_t)_f); }

		//Get scene in the scene list by ID
		Scene& GetSceneByID(SceneID _id);

		//Get scene in the scene list by scene name
		Scene& GetSceneByName(const std::string& _sceneName);

		//Get all loaded scenes - .first is scene ID, .second is scene object
		inline const auto& GetAllScenes() const							{ return m_sceneList; }

		//Get all loaded scene IDs = .first is scene name, .second is scene ID
		inline const auto& GetAllSceneIDs() const						{ return m_sceneIDList; }

		//Is scene created
		inline bool IsSceneCreated(const std::string& _sName) const		{ return m_sceneIDList.count(_sName) != 0; }

		//Functions to overide
		ISYSTEM_FUNCTIONS;

		/*
			Event handling
		*/
		void OnSceneLoaded(IEvent& e);

		friend class Singleton<SceneManager>;

		/*
			When any change scene is called, it only changes the value of m_next scene or the m_flag
			During Scene manager update, if m_next and m_curr are different, or any m_flag is set, then stuff happens
			1: Dispatch the SwitchScene event, where the subscribers will then deal with it: eg: ECS destroy all entities in current scene
			2: Asset Manager will unload all current non-loading screen assets, and Start async loading of assets. ScMgr set "isLoading" flag to true
			3: Game Engine will run LoadingUpdate loop, where all systems will run the "LoadingUpdate" until loading is complete
			4: Once loading is done, dispatch SceneLoaded event, the subscribers will deal with it: eg: ECS destroy entities for loading and load new ones
		*/
	};

#define SceneMgr SceneManager::GetInstance()
}

#endif