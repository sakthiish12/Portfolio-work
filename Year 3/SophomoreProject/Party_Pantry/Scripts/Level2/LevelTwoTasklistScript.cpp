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
#include "../../../Engine/ECS/component.h"

#include "../../../Engine/ECS/ECS.h"
#include"../../../Engine/AssetManager/AssetManager.h"
#include "LevelTwoTasklistScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>
#include "CaveTeleportingScript.h"


using std::cout;
using std::endl;
//std::shared_ptr<EnemySpawningSystem> EnemySpawningSysPtr;
LevelTwoTasklistScript::LevelTwoTasklistScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void LevelTwoTasklistScript::init(float dt, unsigned int idToBeSet)
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

void LevelTwoTasklistScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id))
	{
		//auto& GetWorld = ecs.getComponent <transform2D>(id);
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component sprite

		//check if the dialogue is being toggle 
		if (UiLayer::GetInstance().dialoguePause == true || UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause) {
			oFont.isVisible = false;//make the text not visible 
		}
		else {
			oFont.isVisible = true;
		}
		//get to the cave
		if (teleportActived != true)
		{
			ecs.getComponent<Com_Font>(id).text = Text;//setting the text 
		}

		//get to the cave
		if (teleportActived == true)
		{
			ecs.getComponent<Com_Font>(id).text = Text1;//setting the text 
		}


	}
}

void LevelTwoTasklistScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> LevelTwoTasklistScript::createCopy()
{
	return std::make_shared <LevelTwoTasklistScript>();
}

std::string LevelTwoTasklistScript::getScriptName()
{
	return name;
}






