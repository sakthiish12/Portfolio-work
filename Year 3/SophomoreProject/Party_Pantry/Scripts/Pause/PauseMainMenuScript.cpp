// /******************************************************************************/
// /*!
// \file         PauseMainMenuScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause main menu script 
// \lines        17
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

#include "PauseMainMenuScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include <iostream>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

PauseMainMenuScript::PauseMainMenuScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseMainMenuScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void PauseMainMenuScript::update(float dt)
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

			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON,true) && UiLayer::GetInstance().pauseQuitConfirmation != true && 
				mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
				mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) &&
				UiLayer::GetInstance().m_InstructionsState == UiLayer::InstructionsState::InstructionsOff && InputManager.mouseClickEnabled)
			{
				//load main menu 
				ecs.loadLevel("Resources/SavedLevels/MainMenuLevel.json");
				audio.changeBGM(audio.bgmMainMenu);
				InputManager.mouseClickDisabled = true;
				InputManager.mouseClickEnabled = false;
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//resume the playing 	
				UiLayer::GetInstance().isGamePlayLevel = false;//TODO : think of a better way to do this 
				LogicSystemsPtr->sceneIsChanging = true;
			}

		}
		else {
			oSprite.isVisible = false;
		}
	}

}

void PauseMainMenuScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseMainMenuScript::createCopy()
{
	return std::make_shared <PauseMainMenuScript>();
}

std::string PauseMainMenuScript::getScriptName()
{
	return name;
}






