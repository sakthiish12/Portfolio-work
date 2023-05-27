#include "pch.h"
#include "Scene.h"


namespace BE
{
	const SceneID NULL_SCENE_ID = SceneID(0);
	const SceneID LOADING_SCENE_ID = SceneID(1);
	const Scene NULL_SCENE = Scene("Invalid", "", NULL_SCENE_ID);
	const Scene LOADING_SCENE = Scene("Loading", "", LOADING_SCENE_ID);
	Scene::Scene(std::string&& _name, std::string&& _file, SceneID _id)
		: m_id(_id), m_sceneName(std::move(_name))
	{
		m_sceneFile = std::move(_file);
		ConvertFilePathSlash(m_sceneFile);
	}

	bool Scene::operator==(const Scene& _otherScene) const
	{
		return (m_id		== _otherScene.m_id			&&
				m_sceneName == _otherScene.m_sceneName  &&
				m_sceneFile == _otherScene.m_sceneFile);
	}

	bool Scene::operator!=(const Scene& _otherScene) const
	{
		return !(*this == _otherScene);
	}
}