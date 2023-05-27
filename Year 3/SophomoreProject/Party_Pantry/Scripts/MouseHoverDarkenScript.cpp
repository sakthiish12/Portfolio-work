// /******************************************************************************/
// /*!
// \file         MouseHoverDarkenScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the mouse hover darken script
// \lines        
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

#include "MouseHoverDarkenScript.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

MouseHoverDarkScript::MouseHoverDarkScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void MouseHoverDarkScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		initialScaleX = oTransform.scale.x;
		initialScaleY = oTransform.scale.y;

		//setting scaled up scales 
		scaledUpScaleX = initialScaleX + offSetScale;
		scaledUpScaleY = initialScaleY + offSetScale;
	}
}

void MouseHoverDarkScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oMaterial = ecs.getComponent<material>(id);//getting the material component 
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform

		//lighten it 
		oMaterial.color.x = 1.f;
		oMaterial.color.y = 1.f;
		oMaterial.color.z = 1.f;


		//mouse picking 
		Vec2 mousePos = MouseManager.getWorldPosition();

		if (
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f))
		{
			//darken it 
			oMaterial.color.x = oMaterial.color.x / 2;
			oMaterial.color.y = oMaterial.color.y / 2;
			oMaterial.color.z = oMaterial.color.z / 2;

			//check if it has a parent entity 
			if (!ecs.doesEntityHaveParent(id)) {
				//tweak the scale 
				oTransform.scale.x = scaledUpScaleX;
				oTransform.scale.y = scaledUpScaleY;
			}
		}
		else {
			//resetting the scale 
			oTransform.scale.x = initialScaleX;
			oTransform.scale.y = initialScaleY;
		}

	}

}

void MouseHoverDarkScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> MouseHoverDarkScript::createCopy()
{
	return std::make_shared <MouseHoverDarkScript>();
}

std::string MouseHoverDarkScript::getScriptName()
{
	return name;
}






