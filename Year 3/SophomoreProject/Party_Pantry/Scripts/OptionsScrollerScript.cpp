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

#include "OptionsScrollerScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <cmath>
#include <Sound/SoundManagerV2.h>
#include <WinMgr/WinMgr.h>
#include <Graphics/Source/PostProcessing.h>

using std::cout;
using std::endl;
float StartingPosX;
OptionsScrollerScript::OptionsScrollerScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void OptionsScrollerScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	//ecs.getComponent<transform2D>(id).pos.x = StartingPosX;
	if (audio.volumeBGM == 0.1f) {
		ecs.getComponent<transform2D>(id).pos.x = -136.f;
	}
	if (audio.volumeBGM == 0.4f) {
		ecs.getComponent<transform2D>(id).pos.x = 36.f;
	}
	if (audio.volumeBGM == 0.6f) {
		ecs.getComponent<transform2D>(id).pos.x = 138.f;
	}
	if (audio.volumeBGM == 0.8f) {
		ecs.getComponent<transform2D>(id).pos.x = 240.f;
	}
	if (audio.volumeBGM == 1.0f) {
		ecs.getComponent<transform2D>(id).pos.x = 374.f;
	}
	state = scriptBase::ScriptState::UPDATE;
}

void OptionsScrollerScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	
	if (ecs.entityHas<spriteCom>(id)) {
		//auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		//mouse picking 
		float MaxVol = 0.2f;
		UNREFERENCED_PARAMETER(MaxVol);
		float increase = 1 / 237;
		UNREFERENCED_PARAMETER(increase);
		Vec2 mousePos = MouseManager.getWorldPosition();
		float max = -137;
		float min = 375;
		float volume;

		//std::cout << mousePos.x << " , " << mousePos.y << std::endl;
		if ((
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f)) && MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON)&&
			(mousePos.x >= -137) && (mousePos.x <= 375))
		{
			oTransform.pos.x = mousePos.x;
			

			volume = mousePos.x / (min - max);
			//std::cout << mousePos.x << std::endl;
			
			if (-137<mousePos.x  && mousePos.x<35) {
				StartingPosX = oTransform.pos.x;
				audio.chBGM->setVolume(0.1f);
				audio.volumeBGM = 0.1f;
			}
			if (35 <mousePos.x && mousePos.x < 137) {
				StartingPosX = oTransform.pos.x;
				audio.chBGM->setVolume(0.4f);
				audio.volumeBGM = 0.4f;
			}
			if (137 <mousePos.x && mousePos.x < 239) {
				StartingPosX = oTransform.pos.x;
				audio.chBGM->setVolume(0.6f);
				audio.volumeBGM = 0.6f;
			}
			if (239 <mousePos.x && mousePos.x < 341) {
				StartingPosX = oTransform.pos.x;
				audio.chBGM->setVolume(0.8f);
				audio.volumeBGM = 0.8f;
			}
			if (341 <mousePos.x && mousePos.x <375) {
				StartingPosX = oTransform.pos.x;
				audio.chBGM->setVolume(1.0f);
				audio.volumeBGM = 1.0f;
			}
			//if (volume>0.1 && volume<0.9) 
			//{
			//	audio.volumeBGM += volume;
			//	//SoundClass::getInstance()->setVolume(background, volume);
			//}
			//else
			//{
			//	audio.volumeBGM = 0;
			//	//SoundClass::getInstance()->setVolume(background, 0);
			//}
		}
	}
}

void OptionsScrollerScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);

}

std::shared_ptr<scriptBase> OptionsScrollerScript::createCopy()
{
	return std::make_shared <OptionsScrollerScript>();
}

std::string OptionsScrollerScript::getScriptName()
{
	return name;
}





