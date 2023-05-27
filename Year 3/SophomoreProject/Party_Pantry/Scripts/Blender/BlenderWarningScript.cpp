// /******************************************************************************/
// /*!
// \file         BlenderWarningScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Match, 2022 <Date of last SIGNIFICANT CHANGE>
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
#include "BlenderWarningScript.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

BlenderWarningScript::BlenderWarningScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderWarningScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);
		originalScaleX = oTransform.scale.x;
		originalScaleY = oTransform.scale.y;
		targetScaleX = originalScaleX * 1.5f;
		targetScaleY = originalScaleY * 1.5f;
	}
}

void BlenderWarningScript::update(float dt)
{
	if (ecs.entityHas<spriteCom>(id))
	{
		//increase the size for awhile 
		auto& oTransform = ecs.getComponent<transform2D>(id);
		//if enlarging 
		if (!isShrinking) {
			oTransform.scale.x >= targetScaleX ? oTransform.scale.x = targetScaleX, isShrinking = true : oTransform.scale.x += enlargingRate * dt;
			oTransform.scale.y >= targetScaleY ? oTransform.scale.y = targetScaleY, isShrinking = true : oTransform.scale.y += enlargingRate * dt;
		}
		else {
			//is shrinking 
			oTransform.scale.x <= originalScaleX ? oTransform.scale.x = originalScaleX, isShrinking = false, effectTrigger = false : oTransform.scale.x -= enlargingRate * dt;
			oTransform.scale.y <= originalScaleY ? oTransform.scale.y = originalScaleX, isShrinking = false, effectTrigger = false : oTransform.scale.y -= enlargingRate * dt;
		}
	}
}

void BlenderWarningScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> BlenderWarningScript::createCopy()
{
	return std::make_shared <BlenderWarningScript>();
}

std::string BlenderWarningScript::getScriptName()
{
	return name;
}






