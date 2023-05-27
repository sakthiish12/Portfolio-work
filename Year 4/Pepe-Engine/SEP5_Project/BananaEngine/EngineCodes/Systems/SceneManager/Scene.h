#pragma once
#ifndef	SCENE_H
#define SCENE_H

#include <Precompiled/pch.h>
#include <Utilities/BEID/BEID.h>
#include <Utilities/Utility.h>

namespace BE
{
	using SceneID = BEID;


	class Scene
	{
		SceneID m_id;						//unique ID to this scene, 0 means empty/invalid scene
		std::string m_sceneName;			//Name of the scene
		std::string m_sceneFile;			//Relative File path to load this scene (Json)

	public:
		//Creates a new scene. leave _id empty for random ID
		Scene() = default;
		Scene(std::string&& _name, std::string&& _file, SceneID _id = SceneID{});

		//Getters
		inline SceneID GetSceneID() const					{ return m_id; }
		inline const std::string& GetSceneName() const		{ return m_sceneName; }
		inline const std::string& GetSceneFile() const		{ return m_sceneFile; }
		
		//Setters
		inline void SetSceneName(std::string&& _newName)	{ m_sceneName = std::move(_newName); }
		inline void SetSceneFile(std::string&& _newFile)	{ m_sceneFile = std::move(_newFile); }

		//Check if 2 scenes are the same or different
		bool operator==(const Scene& _otherScene) const;
		bool operator!=(const Scene& _otherScene) const;
	};

	//Reserved Scene
	extern const SceneID NULL_SCENE_ID;
	extern const SceneID LOADING_SCENE_ID;
	extern const Scene NULL_SCENE;
	extern const Scene LOADING_SCENE;
}

#endif
