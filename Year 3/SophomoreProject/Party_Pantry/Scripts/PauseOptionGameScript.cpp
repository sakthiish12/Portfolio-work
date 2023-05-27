// /******************************************************************************/
// /*!
// \file         PauseOptionGameScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         18th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"

#include "PauseOptionGameScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>
#include <Graphics/Source/PostProcessing.h>

using std::cout;
using std::endl;

PauseOptionGameScript::PauseOptionGameScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseOptionGameScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	if (audio.unmuteBGM == true)
	{
		//playing
		ecs.getComponent<transform2D>(id).pos.z = 1.5f;
	}
	else
	{
		//mute
		ecs.getComponent<transform2D>(id).pos.z = 0.f;
	}
	state = scriptBase::ScriptState::UPDATE;
}

void PauseOptionGameScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	int counter = 1;
	

	
	if (ecs.entityHas<spriteCom>(id)) {
		//auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		//mouse picking 

		/*if (bgmuted == true) {
			ecs.getComponent<transform2D>(id).pos.z = 0;
		}*/

		Vec2 mousePos = MouseManager.getWorldPosition();
		if ((
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f)) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON) && counter == 1)
		{
			if (audio.unmuteBGM == true) 
			{
				//SoundClass::getInstance()->channels[background]->setMute(true);
				audio.chBGM->setVolume(0.0f);
				audio.unmuteBGM = false;
				ecs.getComponent<transform2D>(id).pos.z = 0;
			}
			else 
			{
				//SoundClass::getInstance()->channels[background]->setMute(false);
				audio.chBGM->setVolume(audio.volumeBGM);
				audio.unmuteBGM = true;
				ecs.getComponent<transform2D>(id).pos.z = 1.5;
			}
			
		}

	}

}

void PauseOptionGameScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	
}

std::shared_ptr<scriptBase> PauseOptionGameScript::createCopy()
{
	return std::make_shared <PauseOptionGameScript>();
}

std::string PauseOptionGameScript::getScriptName()
{
	return name;
}






