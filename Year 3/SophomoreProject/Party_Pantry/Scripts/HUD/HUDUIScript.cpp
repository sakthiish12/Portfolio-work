// /******************************************************************************/
// /*!
// \file         HUDUIScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         11th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the abilities UI
// \lines        
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
#include "HUDUIScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

HUDUIScript::HUDUIScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void HUDUIScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void HUDUIScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<spriteCom>(id))
	{
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting sprite component 
		//check if camera transition is in progress 
		if (UiLayer::GetInstance().cameraTransition) {
			oSprite.isVisible = false;
		}
		else {
			oSprite.isVisible = true;
		}

		//check what the current stage of airblast now & setting the corresponding ui
		switch (stage)
		{
		case HUDUIScript::airBlastStages::none:
			oSprite.textureId = "P0.png";
			break;
		case HUDUIScript::airBlastStages::first:
			oSprite.textureId = "P1.png";
			break;
		case HUDUIScript::airBlastStages::second:
			oSprite.textureId = "P2.png";
			break;
		case HUDUIScript::airBlastStages::third:
			oSprite.textureId = "P3.png";
			break;
		case HUDUIScript::airBlastStages::fourth:
			oSprite.textureId = "P4.png";
			break;
		default:
			oSprite.textureId = "P0.png";
			break;
		}
		
	}
}

void HUDUIScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> HUDUIScript::createCopy()
{
	return std::make_shared <HUDUIScript>();
}

std::string HUDUIScript::getScriptName()
{
	return name;
}






