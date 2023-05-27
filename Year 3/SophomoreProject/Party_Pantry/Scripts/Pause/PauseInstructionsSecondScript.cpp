// /******************************************************************************/
// /*!
// \file         PauseInstructionsSecondScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause instructions second script 
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

#include "PauseInstructionsSecondScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../../Engine/Editor/Editor.h"//editor 
#include <iostream>

using std::cout;
using std::endl;

PauseInstrucSecondScript::PauseInstrucSecondScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseInstrucSecondScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void PauseInstrucSecondScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite

		//if the first instruction scene is out 
		if (UiLayer::GetInstance().m_InstructionsState == UiLayer::InstructionsState::Instruction2) {
			oSprite.isVisible = true;

			if (KeyManager.isKeyTriggered(VK_ESCAPE)) {
				clickCounter = 0;
				//when instructions button is clicked, set the instruction state to the third instructions
				UiLayer::GetInstance().m_InstructionsState = UiLayer::InstructionsState::Instruction3;
			}

			//mouse picking 
			Vec2 mousePos = MouseManager.getWorldPosition();

			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && clickCounter++ > 0)
			{
				clickCounter = 0;
				//when instructions button is clicked, set the instruction state to the third instructions
				UiLayer::GetInstance().m_InstructionsState = UiLayer::InstructionsState::Instruction3;
			}

		}
		else {
			oSprite.isVisible = false;
		}
	}

}

void PauseInstrucSecondScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseInstrucSecondScript::createCopy()
{
	return std::make_shared <PauseInstrucSecondScript>();
}

std::string PauseInstrucSecondScript::getScriptName()
{
	return name;
}






