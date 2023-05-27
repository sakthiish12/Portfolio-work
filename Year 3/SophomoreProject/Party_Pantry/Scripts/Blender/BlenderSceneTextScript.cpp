// /******************************************************************************/
// /*!
// \file         BlenderSceneTextScritp.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the blender scene text script
// \lines        41
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

#include "../../../Engine/ECS/Systems/Logic.h"
#include "BlenderSceneTextScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include "BlenderCapDragScript.h"
#include <iostream>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

BlenderSceneTextScript::BlenderSceneTextScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderSceneTextScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	UiLayer::GetInstance().isGamePlayLevel = false;//TODO : think of a better way to do this 
	lidClicked = false;
}

void BlenderSceneTextScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<spriteCom>(id))
	{
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite

		//if everything has been put into the blender 
		if (UpgradeManager::GetInstance().numberOfTomatoesToDeduct <= 0 && UpgradeManager::GetInstance().numberOfChilliesToDeduct <= 0 && UpgradeManager::GetInstance().numberOfMushroomToDeduct <= 0) {
			for (auto x : ecs.getEntityNames()) {
				if (ecs.entityHas<scriptComponent>(x.first)) {
					auto script = ecs.getComponent<scriptComponent>(x.first).scriptContainer.find("BlenderCapDragScript");
					//check if it's within the map 
					if (script != ecs.getComponent<scriptComponent>(x.first).scriptContainer.end()) {
						BlenderCapDragScript* player = reinterpret_cast<BlenderCapDragScript*>((*script).second.get());
						if (player->withinRange)
							lidClicked = true;
					}
				}
			}

			oSprite.textureId = "Cover.png";//changing the texture id 

			//when the lid has been clicked !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!********************&&&&&&&&&&&&&&&& THIS SHOULD BE A AABB CHECK AT FINAL POS
			if (lidClicked == true) {
				oSprite.textureId = "E.png";//changing the texture id 
			}

			//if the lid has been placed and F is pressed
			if (lidClicked == true && KeyManager.isKeyTriggered('f', true)) {
				audio.playPannedSoundEffect(audio.sfxBlenderRun);
				UpgradeManager::GetInstance().next_upgrade = UpgradeManager::Next_Upgrade::None;
				ecs.loadLevel("Resources/SavedLevels/ThroneRoomLevel.json");//load the win screen
				LogicSystemsPtr->sceneIsChanging = true;
			}
		}

	}
}

void BlenderSceneTextScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BlenderSceneTextScript::createCopy()
{
	return std::make_shared <BlenderSceneTextScript>();
}

std::string BlenderSceneTextScript::getScriptName()
{
	return name;
}






