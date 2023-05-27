// /******************************************************************************/
// /*!
// \file         BlenderCapDragScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the blender cap drag script 
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

#include "../../../Engine/InputMgr/InputMgr.h"//input mgr
#include "BlenderCapDragScript.h"
#include <iostream>

using std::cout;
using std::endl;

BlenderCapDragScript::BlenderCapDragScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderCapDragScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void BlenderCapDragScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform com 

		Vec2 mousePos = MouseManager.getWorldPosition();

		if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) &&
			mousePos.x >= (oTransform.pos.x - oTransform.scale.x / 2.0f) &&
			mousePos.x <= (oTransform.pos.x + oTransform.scale.x / 2.0f) &&
			mousePos.y >= (oTransform.pos.y - oTransform.scale.y / 2.0f) &&
			mousePos.y <= (oTransform.pos.y + oTransform.scale.y / 2.0f))
		{
			oTransform.pos.x = mousePos.x;
			oTransform.pos.y = mousePos.y;
		}

		//check if the cap is within the capped range 
		if (oTransform.pos.x > (cappedPosX - cappedRadiusOffset) && oTransform.pos.x < (cappedPosX + cappedRadiusOffset)
			&& oTransform.pos.y >(cappedPosY - cappedRadiusOffset) && oTransform.pos.y < (cappedPosY + cappedRadiusOffset)) {

			//set capped to true 
			withinRange = true;
		}
		else {
			withinRange = false;
		}
	}
}

void BlenderCapDragScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BlenderCapDragScript::createCopy()
{
	return std::make_shared <BlenderCapDragScript>();
}

std::string BlenderCapDragScript::getScriptName()
{
	return name;
}






