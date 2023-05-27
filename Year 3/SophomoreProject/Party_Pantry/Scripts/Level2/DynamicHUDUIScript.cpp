// /******************************************************************************/
// /*!
// \file         DynamicHUDUIScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         15th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the lever activation script 
// \lines        51
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

#include "../../../Engine/InputMgr/InputMgr.h"//input mgr
#include "../../../Engine/ECS/Systems/Logic.h"//logic
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities 
#include "../../../Engine/Sound/SoundManager.h"
#include "DynamicHUDUIScript.h"
#include <iostream>
#include "../../../Engine/ECS/Systems/Physics/CollisionSystem.h"

using std::cout;
using std::endl;
DynamicHUDUIScript::DynamicHUDUIScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void DynamicHUDUIScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//storing of the original transform 
	if (ecs.entityHas<transform2D>(id)) {
		auto x = ecs.getComponent<transform2D>(id);
		originalPosX = x.pos.x;
		originalPosY = x.pos.y;
		originalScaleX = x.scale.x;
		originalScaleY = x.scale.y;
	}

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}
}

void DynamicHUDUIScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

		//if it is activated 
		if (isActivated) {
			//check if camera ratio 
			if (cameraRatio) {
				//calculate the rate of increasing 
				finalScaleX = originalScaleX * cameraRatio;
				finalScaleY = originalScaleY * cameraRatio;
			}

			//tweak the transform accordingly 
			if (oTransform.scale.x <= finalScaleX)
				oTransform.scale.x += increasingScaleX * dt;

			if (oTransform.scale.y <= finalScaleY)
				oTransform.scale.y += increasingScaleX * dt;

		}
	}

}

void DynamicHUDUIScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> DynamicHUDUIScript::createCopy()
{
	return std::make_shared <DynamicHUDUIScript>();
}

std::string DynamicHUDUIScript::getScriptName()
{
	return name;
}






