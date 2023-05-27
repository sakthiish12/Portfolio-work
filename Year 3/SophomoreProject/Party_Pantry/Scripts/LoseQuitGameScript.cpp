// /******************************************************************************/
// /*!
// \file         LoseQuitGameScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function defintion of the lose quit game script button 
// \lines        42
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

#include "LoseQuitGameScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

LoseQuitGameScript::LoseQuitGameScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void LoseQuitGameScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	UiLayer::GetInstance().isGamePlayLevel = false;//TODO : think of a better way to do this 
}

void LoseQuitGameScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform


		//mouse picking 
		Vec2 mousePos = MouseManager.getWorldPosition();
		const float pos[] = { 0.f,0.f,0.f };
		if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) &&
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f))
		{
			
			for (int i = 0; i < 37; i++)
			{
				//SoundClass::getInstance()->Pause(i);
			}
			// Losing music here
			audio.chBGM->stop();
			audio.chSFX->stop();
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Edit;//setting the scene state to edit 
			Editor::GetInstance().isPlaying = false;//is playing false 
			//Reload temporary level
			ecs.loadLevel("Resources/TemporaryLevel.json");
			//Delete temporary level

			std::filesystem::remove("Resources/TemporaryLevel.json");
			LogicSystemsPtr->sceneIsChanging = true;
		}

	
	}

}

void LoseQuitGameScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

std::shared_ptr<scriptBase> LoseQuitGameScript::createCopy()
{
	return std::make_shared <LoseQuitGameScript>();
}

std::string LoseQuitGameScript::getScriptName()
{
	return name;
}






