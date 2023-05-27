// /******************************************************************************/
// /*!
// \file         SoundsLevel2Script.cpp
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

#include "SoundLevel2Script.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>
#include <Graphics/Source/PostProcessing.h>
#include "../../Engine/AssetManager/UpgradeManager.h"
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

SoundLevel2Script::SoundLevel2Script(const char* scriptName) :
	scriptBase(scriptName)
{
}

void SoundLevel2Script::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void SoundLevel2Script::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	int counter = 1;
	UNREFERENCED_PARAMETER(counter);



	if (ecs.entityHas<spriteCom>(id)) {
		//auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		//mouse picking 


		Vec2 mousePos = MouseManager.getWorldPosition();
		if ((
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f)) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON))
		{
			if (UpgradeManager::GetInstance().level2Completed) {
				//set the script
				//SoundClass::getInstance()->channels[background]->setPaused(false);
				ecs.loadLevel("Resources/SavedLevels/NewLevel2CavePart2Level.json");
				audio.changeBGM(audio.bgmNewLevel2);
				LogicSystemsPtr->sceneIsChanging = true;
			}
			else {
				//set the script
				//SoundClass::getInstance()->channels[background]->setPaused(false);
				ecs.loadLevel("Resources/SavedLevels/NewLevel2CaveLevel.json");
				audio.changeBGM(audio.bgmNewLevel2);
				LogicSystemsPtr->sceneIsChanging = true;
			}
		}

	}

}

void SoundLevel2Script::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);

}

std::shared_ptr<scriptBase> SoundLevel2Script::createCopy()
{
	return std::make_shared <SoundLevel2Script>();
}

std::string SoundLevel2Script::getScriptName()
{
	return name;
}






