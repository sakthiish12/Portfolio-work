// /******************************************************************************/
// /*!
// \file         MushroomDynamicCameraScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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
#include "MushroomDynamicCameraScript.h"
#include <iostream>
#include "../../../Engine/ECS/Systems/Physics/CollisionSystem.h"

#include "DynamicHUDUIScript.h"//script 

using std::cout;
using std::endl;
MushroomDynamicCameraScript::MushroomDynamicCameraScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void MushroomDynamicCameraScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	originalCameraHeight = activeCameraData->height;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}

	originalScaleX = activeCameraTransform->scale.x;
	originalScaleY = activeCameraTransform->scale.y;
}

void MushroomDynamicCameraScript::update(float dt)
{

	if (ecs.entityHas<transform2D>(id)) {
		//if it is activated 
		if (isActivated) {

			//increasing the height of the camera 
			if (activeCameraData->height <= idealHeight) {
				activeCameraData->height += heightIncreasing * dt;
				activeCameraTransform->scale.x += heightIncreasing/heightChildIncreasingScale * dt;
				activeCameraTransform->scale.y += heightIncreasing/heightChildIncreasingScale * dt;
			}
			else {
				isActivated = false;
			}
		}

		if (mushroomBossDead) {
			//return back to normal size 
			if (activeCameraData->height >= originalCameraHeight) {
				activeCameraData->height -= heightIncreasing * dt;
				activeCameraTransform->scale.x -= heightIncreasing / heightChildIncreasingScale * dt;
				activeCameraTransform->scale.y -= heightIncreasing / heightChildIncreasingScale * dt;
			}
		}
	}

}

void MushroomDynamicCameraScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> MushroomDynamicCameraScript::createCopy()
{
	return std::make_shared <MushroomDynamicCameraScript>();
}

std::string MushroomDynamicCameraScript::getScriptName()
{
	return name;
}






