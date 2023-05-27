// /******************************************************************************/
// /*!
// \file         CameraTransitionScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the camera transition
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
#include "CreditSceneScript.h"
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities system

CreditSceneScript::CreditSceneScript(const char* scriptName) :
	scriptBase(scriptName)
{

}

void CreditSceneScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	startingPosY = ecs.getComponent<transform2D>(id).pos.y;
}

void CreditSceneScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);
		
		if(oTransform.pos.y <= endingPosY || (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && oTransform.pos.y != startingPosY)){
			//set the script
			ecs.loadLevel("Resources/SavedLevels/MainMenuLevel.json");
			LogicSystemsPtr->sceneIsChanging = true;
		}

		if (oTransform.pos.y >= endingPosY) {
			oTransform.pos.y -= transitionSpeed * dt;
		}
	}

}

void CreditSceneScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> CreditSceneScript::createCopy()
{
	return std::make_shared <CreditSceneScript>();
}

std::string CreditSceneScript::getScriptName()
{
	return name;
}






