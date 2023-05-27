// /******************************************************************************/
// /*!
// \file         PauseRestartScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause restart script
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

#include "PauseRestartScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../../Engine/Editor/Editor.h"//editor 
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include "../../../Engine/Graphics/Source/PostProcessing.h"
#include <iostream>
#include <Sound/SoundManager.h>
#include "../Level2/CaveTeleportingScript.h"
#include <Sound/SoundManagerV2.h>
using std::cout;
using std::endl;

PauseRestartScript::PauseRestartScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseRestartScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void PauseRestartScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		const float pos[] = { 0.f,0.f,0.f };
		//if the scene state is pause, toggle visibility
		if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause && UiLayer::GetInstance().dialoguePause == false) {
			oSprite.isVisible = true;

			//mouse picking 
			Vec2 mousePos = MouseManager.getWorldPosition();

			if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) && UiLayer::GetInstance().pauseQuitConfirmation != true &&
				mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
				mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) &&
				UiLayer::GetInstance().m_InstructionsState == UiLayer::InstructionsState::InstructionsOff && InputManager.mouseClickEnabled)
			{
				//restart the gameplay
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//resume the playing 	
				PostProcessing::GetInstance().m_AlphaValue = 1.f;//reset alpha value 
				
				//SoundClass::getInstance()->channels[CaveBgm]->setPaused(false);
				if (teleportActived == true) {
					//SoundClass::getInstance()->channels[CaveBgm]->setPaused(false);
				}
				else {
					//SoundClass::getInstance()->channels[background]->setPaused(false);
				}
				if (UpgradeManager::GetInstance().level1Completed) {
					ecs.loadLevel("Resources/SavedLevels/NewLevel1Part2Level.json");
					audio.changeBGM(audio.bgmNewLevel1);
				}
				else{
					ecs.loadLevel("Resources/SavedLevels/NewLevel1Level.json");
					audio.changeBGM(audio.bgmNewLevel1);
				}
				LogicSystemsPtr->sceneIsChanging = true;
			}

		}
		else {
			oSprite.isVisible = false;
		}
	}

}

void PauseRestartScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseRestartScript::createCopy()
{
	return std::make_shared <PauseRestartScript>();
}

std::string PauseRestartScript::getScriptName()
{
	return name;
}






