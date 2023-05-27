// /******************************************************************************/
// /*!
// \file         PauseQuitGameScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines		 32
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
#include "SpaceToSkipEndSceneScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>

using std::cout;
using std::endl;

SpaceToSkipEndSceneScript::SpaceToSkipEndSceneScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void SpaceToSkipEndSceneScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void SpaceToSkipEndSceneScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id)) {

		//if the space is triggered
		if (KeyManager.isKeyTriggered(VK_SPACE, true)) {

			//load the info scene 
			ecs.loadLevel("Resources/SavedLevels/CreditsSceneLevel.json");
			LogicSystemsPtr->sceneIsChanging = true;
		}


	}

}

void SpaceToSkipEndSceneScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

std::shared_ptr<scriptBase> SpaceToSkipEndSceneScript::createCopy()
{
	return std::make_shared <SpaceToSkipEndSceneScript>();
}

std::string SpaceToSkipEndSceneScript::getScriptName()
{
	return name;
}






