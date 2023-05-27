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

#include "SpaceToSkipScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>

using std::cout;
using std::endl;

SpaceToSkipScript::SpaceToSkipScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void SpaceToSkipScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void SpaceToSkipScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id)) {

		//if the space is triggered
		if(KeyManager.isKeyTriggered(VK_SPACE,true)){

			//load the info scene 
			ecs.loadLevel("Resources/SavedLevels/NewLevel1PanningLevel.json");
			LogicSystemsPtr->sceneIsChanging = true;
		}

		
	}

}

void SpaceToSkipScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

std::shared_ptr<scriptBase> SpaceToSkipScript::createCopy()
{
	return std::make_shared <SpaceToSkipScript>();
}

std::string SpaceToSkipScript::getScriptName()
{
	return name;
}






