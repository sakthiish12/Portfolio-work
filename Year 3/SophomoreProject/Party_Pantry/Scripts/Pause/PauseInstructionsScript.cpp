// /******************************************************************************/
// /*!
// \file         PauseInstructionsScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function defintion of the pause instructions script
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

#include "PauseInstructionsScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../../Engine/Editor/Editor.h"//editor 
#include <iostream>

using std::cout;
using std::endl;

PauseInstrucScript::PauseInstrucScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseInstrucScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void PauseInstrucScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform

		//if the scene state is pause, toggle visibility
		if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause && UiLayer::GetInstance().dialoguePause == false) {
			oSprite.isVisible = true;

			//mouse picking 
			Vec2 mousePos = MouseManager.getWorldPosition();

			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && UiLayer::GetInstance().pauseQuitConfirmation != true &&
				mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
				mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) && 
				InputManager.mouseClickEnabled)
			{
				//when instructions button is clicked, set the instruction state to the first instructions
				UiLayer::GetInstance().m_InstructionsState = UiLayer::InstructionsState::Instruction1;
			}

		}
		else {
			oSprite.isVisible = false;
		}
	}

}

void PauseInstrucScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseInstrucScript::createCopy()
{
	return std::make_shared <PauseInstrucScript>();
}

std::string PauseInstrucScript::getScriptName()
{
	return name;
}






