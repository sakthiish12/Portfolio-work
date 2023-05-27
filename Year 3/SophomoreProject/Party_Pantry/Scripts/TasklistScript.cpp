// /******************************************************************************/
// /*!
// \file         TaskListScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the task list script  
// \lines        87
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"
#include"../../Engine/AssetManager/AssetManager.h"
#include "TaskListScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/AssetManager/AssetManager.h"
#include <iostream>


using std::cout;
using std::endl;
//std::shared_ptr<EnemySpawningSystem> EnemySpawningSysPtr;
TaskListScript::TaskListScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void TaskListScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerEntityID = e.first;//player is found 
			break;
		}
	}
}

void TaskListScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id))
	{
		//auto& oSprite = ecs.getComponent<spriteCom>(id);
		//auto& oTransform = ecs.getComponent<transform2D>(id);
		//auto& GetWorld = ecs.getComponent <transform2D>(id);
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component sprite

		//check if the dialogue is being toggle 
		if (UiLayer::GetInstance().dialoguePause == true || UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause) {
			oFont.isVisible = false;//make the text not visible 
		}
		else {
			oFont.isVisible = true;
		}

		//if (isLoaded == false) {
		//	taskTextID = ecs.createEntityFromFile("Resources/SavedEntities/tasktext.json");
		//	ecs.setEntityParent(taskTextID, id);//setting the parentage 
		//	isLoaded = true;
		//}
		if (taskDone1 != true)
		{
			ecs.getComponent<Com_Font>(id).text = Text4;//setting the text 
			if (AssetManager::GetInstance().blenderCollected == true) {
				ecs.getComponent<Com_Font>(id).text = Text1;//setting the text 
				taskDone1 = true;
			}
		}

		if (taskDone1 == true) 
		{
			if (AssetManager::GetInstance().tomatoesCollected < 5)
			{
				if (ecs.entityHas<Com_Font>(id)) {
					ecs.getComponent<Com_Font>(id).text = Text1;//setting the text
				}
			}
			else 
			{
				ecs.getComponent<Com_Font>(id).text = Text3;//setting the text 
			}
		}

	}
}

void TaskListScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> TaskListScript::createCopy()
{
	return std::make_shared <TaskListScript>();
}

std::string TaskListScript::getScriptName()
{
	return name;
}






