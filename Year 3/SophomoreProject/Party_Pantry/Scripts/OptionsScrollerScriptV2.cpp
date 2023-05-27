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

#include "OptionsScrollerScriptV2.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <cmath>
#include <Sound/SoundManager.h>
#include <WinMgr/WinMgr.h>
#include <Graphics/Source/PostProcessing.h>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;
float StartingPosXV2;

OptionsScrollerScriptV2::OptionsScrollerScriptV2(const char* scriptName) :
	scriptBase(scriptName)
{
}

void OptionsScrollerScriptV2::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	//ecs.getComponent<transform2D>(id).pos.x = StartingPosXV2;
	if (audio.volumeSFX == 0.1f) {
		ecs.getComponent<transform2D>(id).pos.x = -136.f;
	}
	if (audio.volumeSFX == 0.4f) {
		ecs.getComponent<transform2D>(id).pos.x = 36.f;
	}
	if (audio.volumeSFX == 0.6f) {
		ecs.getComponent<transform2D>(id).pos.x = 138.f;
	}
	if (audio.volumeSFX == 0.8f) {
		ecs.getComponent<transform2D>(id).pos.x = 240.f;
	}
	if (audio.volumeSFX == 1.0f) {
		ecs.getComponent<transform2D>(id).pos.x = 374.f;
	}
	state = scriptBase::ScriptState::UPDATE;
}

void OptionsScrollerScriptV2::update(float dt)
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
		//float calcVolume = std::log(((volume - slider_min) * (pfVolumeIncrementDB - 1.0f)) / (slider_max - slider_min) + 1.0f) / std::log(pfVolumeIncrementDB) * (pfLevelMaxDB - pfLevelMinDB) + pfLevelMinDB;
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
			//volume = 1 / (max + min);
			//std::cout << volume << std::endl;
			if (-137 < mousePos.x && mousePos.x < 35) {
				StartingPosXV2 = (float)oTransform.pos.x;
				audio.chSFX->setVolume(0.1f);
				audio.volumeSFX = 0.1f;
			}
			if (35 < mousePos.x && mousePos.x < 137) {
				StartingPosXV2 = oTransform.pos.x;
				audio.chSFX->setVolume(0.4f);
				audio.volumeSFX = 0.4f;
			}
			if (137 < mousePos.x && mousePos.x < 239) {
				StartingPosXV2 = oTransform.pos.x;
				audio.chSFX->setVolume(0.6f);
				audio.volumeSFX = 0.6f;
			}
			if (239 < mousePos.x && mousePos.x < 341) {
				StartingPosXV2 = oTransform.pos.x;
				audio.chSFX->setVolume(0.8f);
				audio.volumeSFX = 0.8f;
			}
			if (341 < mousePos.x && mousePos.x < 375) {
				StartingPosXV2 = oTransform.pos.x;
				audio.chSFX->setVolume(1.0f);
				audio.volumeSFX = 1.0f;
			}
		}
	}
}

void OptionsScrollerScriptV2::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);

}

std::shared_ptr<scriptBase> OptionsScrollerScriptV2::createCopy()
{
	return std::make_shared <OptionsScrollerScriptV2>();
}

std::string OptionsScrollerScriptV2::getScriptName()
{
	return name;
}





