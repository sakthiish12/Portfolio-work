// /******************************************************************************/
// /*!
// \file         HUDFireballOverlayScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         21th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#include "HUDFireballOverlayScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>

using std::cout;
using std::endl;

HUDFireballOverlayScript::HUDFireballOverlayScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void HUDFireballOverlayScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void HUDFireballOverlayScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<spriteCom>(id))
	{
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting sprite component 
		//check if camera transition is in progress 
		if (UiLayer::GetInstance().cameraTransition) {
			oSprite.isVisible = false;
			return;
		}
		else {
			oSprite.isVisible = true;
		}
		//check if the super speed is upgraded 
		if (!UpgradeManager::GetInstance().upgradeFireBall) {
			oSprite.isVisible = true;
		}
		else {
			oSprite.isVisible = false;
		}
	}
}

void HUDFireballOverlayScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> HUDFireballOverlayScript::createCopy()
{
	return std::make_shared <HUDFireballOverlayScript>();
}

std::string HUDFireballOverlayScript::getScriptName()
{
	return name;
}






