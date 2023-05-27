// /******************************************************************************/
// /*!
// \file         CounterUIScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         30th Match, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the blender scene text script
// \lines        41
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

#include "../../../Engine/ECS/Systems/Logic.h"
#include "CounterUIScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

CounterUIScript::CounterUIScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void CounterUIScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

}

void CounterUIScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<spriteCom>(id))
	{
		auto& oSprite = ecs.getComponent<spriteCom>(id);

		if (UiLayer::GetInstance().cameraTransition) {
			oSprite.isVisible = false;
		}
		else {
			oSprite.isVisible = true;
		}
	}
}

void CounterUIScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> CounterUIScript::createCopy()
{
	return std::make_shared <CounterUIScript>();
}

std::string CounterUIScript::getScriptName()
{
	return name;
}






