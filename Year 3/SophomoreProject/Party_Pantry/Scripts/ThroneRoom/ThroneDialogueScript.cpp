// /******************************************************************************/
// /*!
// \file         ThroneDialogueScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#include "ThroneDialoguesScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

ThroneDialogueScript::ThroneDialogueScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void ThroneDialogueScript::init(float dt, unsigned int idToBeSet)
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

void ThroneDialogueScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<transform2D>(id))
	{
		//if player stepped on it, start the bridge 
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 

		//load the text 
		if (isTextLoaded == false) {
			textID = ecs.createEntityFromFile("Resources/PresetEntities/throneText.json");
			//setting the position of the text 
			ecs.getComponent<transform2D>(textID).pos.x = oTransform.pos.x;
			ecs.getComponent<transform2D>(textID).pos.y = oTransform.pos.y + posYOffset;
			ecs.getComponent<spriteCom>(textID).isVisible = false;
			isTextLoaded = true;
		}

		//if player is within the trigger 
		if (playerTransform.pos.x > (oTransform.pos.x - proximityRadiusX) && playerTransform.pos.x < (oTransform.pos.x + proximityRadiusX) &&
			playerTransform.pos.y >(oTransform.pos.y - proximityRadius) && playerTransform.pos.y < (oTransform.pos.y + proximityRadius) && isTextLoaded) {
			ecs.getComponent<spriteCom>(textID).isVisible = true;//make it visible	


			if ("dialogue" == ecs.getNameOfEntity(currentDialogueID)) {
				//find if there is active dialogue 
				//std::cout << "toggle true" << std::endl;
				activeDialogue = true;//there is still an active dialogue 
			}
			else {
				//std::cout << "toggle false" << std::endl;
				activeDialogue = false;
			}

			//switch between dialogues
			if (KeyManager.isKeyTriggered('F') && !activeDialogue) {
				//show the dialogues 
				switch (currentDialogueCount)
				{
				case 1:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble1.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 2:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble2.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 3:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble3.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 4:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble4.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 5:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble5.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 6:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble6.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 7:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble7.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 8:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble8.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 9:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble9.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				case 10:
					currentDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ThroneChatBubble10.json");
					ecs.renameEntity(currentDialogueID, "dialogue");
					ecs.getComponent<transform2D>(currentDialogueID).pos.x = ecs.getComponent<transform2D>(playerID).pos.x;
					ecs.getComponent<transform2D>(currentDialogueID).pos.y = ecs.getComponent<transform2D>(playerID).pos.y;
					currentDialogueCount++;
					break;
				default:
					break;//no next dialogue
				}
			}
		}
		else {
			if(isTextLoaded) 
				ecs.getComponent<spriteCom>(textID).isVisible = false;//make it invisible 
		}
	}
}

void ThroneDialogueScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> ThroneDialogueScript::createCopy()
{
	return std::make_shared <ThroneDialogueScript>();
}

std::string ThroneDialogueScript::getScriptName()
{
	return name;
}






