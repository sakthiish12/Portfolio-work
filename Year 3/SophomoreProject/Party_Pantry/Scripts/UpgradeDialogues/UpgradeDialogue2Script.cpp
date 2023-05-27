// /******************************************************************************/
// /*!
// \file         UpgradeDialogue2Script.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Feb, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the teleporting to enter the cave 
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

#include "UpgradeDialogue2Script.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>
#include <Sound/SoundManager.h>

using std::cout;
using std::endl;
UpgradeDialogue2Script::UpgradeDialogue2Script(const char* scriptName) :
	scriptBase(scriptName)
{
}

void UpgradeDialogue2Script::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}
}

void UpgradeDialogue2Script::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 
		//if trigger hit box 
		if (playerTransform.pos.x > (oTransform.pos.x - oTransform.scale.x / 2.f) && playerTransform.pos.x < (oTransform.pos.x + oTransform.scale.x / 2.f) &&
			playerTransform.pos.y >(oTransform.pos.y - oTransform.scale.y / 2.f) && playerTransform.pos.y < (oTransform.pos.y + (oTransform.scale.y / 2.f)) && dialogueActivated == false
			&& UpgradeManager::GetInstance().showUpgradeTutorial && UpgradeManager::GetInstance().part2UpgradeTutorial) {

			dialogueActivated = true;
			//show this dialogue 
			entityID firstDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/UpgradeChatBubble2.json");
			//setting the new position 
			ecs.getComponent<transform2D>(firstDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
			ecs.getComponent<transform2D>(firstDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
			UpgradeManager::GetInstance().upgradeTutorial = true;//telling dialogue manager that it is upgrading dialogue 
		}
	}
}

void UpgradeDialogue2Script::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> UpgradeDialogue2Script::createCopy()
{
	return std::make_shared <UpgradeDialogue2Script>();
}

std::string UpgradeDialogue2Script::getScriptName()
{
	return name;
}






