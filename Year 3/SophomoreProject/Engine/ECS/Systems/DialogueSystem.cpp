// /******************************************************************************/
// /*!
// \file         DialogueSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the dialogue system
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "DialogueSystem.h"
#include "AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../AssetManager/UpgradeManager.h"
#include "../../Sound/SoundManagerV2.h"

std::shared_ptr<DialogueSystem> DialogueSysPtr;


void DialogueSystem::update(float dt) {
	UNREFERENCED_PARAMETER(dt);

	for (auto const& e : getEntitiesRegistered()) {
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting transform com 
		auto& oDialogue = ecs.getComponent<Com_Dialogue>(e);//getting the dialogue com 

		entityID dialogueEntityId;//storing of the dialogue entity id 
		entityID textEntityId;//storing of the text entity id 

		static bool deletedOnce;
		static bool dialogueLoaded;//tracks if any of the dialogue is loaded 
		static std::unordered_map<float,entityID> vecEntityId;//map of entity ids
		static std::unordered_map<float,entityID> vecDialogueId;//map of entity ids
		static std::unordered_map<float, entityID> textDialogueId;//map of the text dialogue id

		for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
			auto& eInnerTransform = ecs.getComponent<transform2D>(eInner);//getting the transform of the player 

			//if it's within the proximity of the dialogue 
			if (eInnerTransform.pos.x > (oTransform.pos.x - oTransform.scale.x/2) && eInnerTransform.pos.x < (oTransform.pos.x + oTransform.scale.x/2) &&
				eInnerTransform.pos.y >(oTransform.pos.y - oTransform.scale.y/2) && eInnerTransform.pos.y < (oTransform.pos.y + oTransform.scale.y/2)) {

				//if the dialogue is not loaded and the dialogue to show is true 
				if (oDialogue.isDialogueLoaded == false && oDialogue.isToShow == true) {
					//creating the entity right below the player pos 
					dialogueEntityId = ecs.createEntityFromFile(oDialogue.dialogueTexture);
					vecEntityId.emplace(std::make_pair(ecs.getComponent<transform2D>(dialogueEntityId).pos.z,dialogueEntityId));
					vecDialogueId.emplace(std::make_pair(ecs.getComponent<transform2D>(dialogueEntityId).pos.z,e));

					if (!UpgradeManager::GetInstance().upgradeTutorial) {
						ecs.getComponent<transform2D>(dialogueEntityId).pos.x = oDialogue.offSetXPos;//setting the dialogue position x with the offset 
						ecs.getComponent<transform2D>(dialogueEntityId).pos.y = oDialogue.offSetYPos;//setting the dialogue position y with the offset 
					}

					if (UpgradeManager::GetInstance().upgradeTutorial) {
						//entityID cameraID;
						//get the camera as the parent 
						for (auto const& ec : ecs.getEntityNames())
						{
							if (ecs.entityHas<Com_type>(ec.first) && ecs.getComponent<Com_type>(ec.first).type == Com_type::type::camera)
							{
								//cameraID = ec.first;//camera is found
								ecs.setEntityParent(dialogueEntityId, ec.first);//setting the parent to the player 
								break;
							}
						}
						//ecs.setEntityParent(dialogueEntityId, cameraID);//setting the parent to the player 
					}
					else {
						ecs.setEntityParent(dialogueEntityId, eInner);//setting the parent to the player 
					}
					ecs.getComponent<spriteCom>(dialogueEntityId).isVisible = true;//setting it to be visible 
					oDialogue.isDialogueLoaded = true;
					oDialogue.isToShow = false;
					dialogueLoaded = true;
				}
			}

		}

		//if there are existing dialogues
		if (vecEntityId.size() != 0) {
			UiLayer::GetInstance().dialoguePause = true;//the dialogue is pause
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game 
			auto maxElement = std::max_element(std::begin(vecDialogueId), std::end(vecDialogueId), [](auto a, auto b) { return a < b; });
			if (ecs.entityHas<Com_Dialogue>(maxElement->second))
			{
				auto& maxDialogue = ecs.getComponent<Com_Dialogue>(maxElement->second);
				if (maxDialogue.dialogueText.size() != 0 && maxDialogue.isNewlyLoaded == false) {
					textEntityId = ecs.createEntityFromFile(maxDialogue.dialogueText);
					textDialogueId.emplace(std::make_pair(ecs.getComponent<transform2D>(e).pos.z, textEntityId));
					if (UpgradeManager::GetInstance().upgradeTutorial) {
						//entityID cameraID;
						//get the camera as the parent 
						for (auto const& ec : ecs.getEntityNames())
						{
							if (ecs.entityHas<Com_type>(ec.first) && ecs.getComponent<Com_type>(ec.first).type == Com_type::type::camera)
							{
								//cameraID = ec.first;//camera is found 
								ecs.setEntityParent(textEntityId, ec.first);//setting the parent to the camera 
								break;
							}
						}
						//ecs.setEntityParent(textEntityId, cameraID);//setting the parent to the camera 
					}
					else {
						for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
							ecs.setEntityParent(textEntityId, eInner);//setting the parent to the player 
						}
					}
					ecs.getComponent<Com_DialogueManager>(textEntityId).isActive = true;//setting the dialogue to be true 
					ecs.getComponent<Com_DialogueManager>(textEntityId).isPrevLoaded = true;//setting the dialogue to be true 
					maxDialogue.isNewlyLoaded = true;
				}
			}
		}
		else if(vecEntityId.size() == 0 && dialogueLoaded == true){
			UiLayer::GetInstance().dialoguePause = false;//the dialogue is not paused
			//audio.killChannel(audio.chSFX);
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
			dialogueLoaded = false;
		}


		//if dialogue has been loaded 
		if (oDialogue.isDialogueLoaded == true && MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && deletedOnce == false) {
			//checking which dialogue that is loaded has the highest z value 
			if (vecEntityId.size() != 0) {
				//need to check with the current dialogue is it is fully loaded 
				auto maxElementDialogue = std::max_element(std::begin(textDialogueId), std::end(textDialogueId), [](auto a, auto b) { return a < b; });
				//sanity check 
				if (ecs.entityHas<Com_DialogueManager>(maxElementDialogue->second)) {
					if (ecs.getComponent<Com_DialogueManager>(maxElementDialogue->second).allFullyLoaded == true) {
						auto maxDialogue = std::max_element(std::begin(vecDialogueId), std::end(vecDialogueId), [](auto a, auto b) { return a < b; });
						auto maxElement = std::max_element(std::begin(vecEntityId), std::end(vecEntityId), [](auto a, auto b) { return a < b; });
						ecs.deleteEntity(maxElement->second);//delete the entity 
						ecs.renameEntity(e, "deleted");//change the name of the entity to signify it's deleted 
						textDialogueId.erase(maxElementDialogue);//removing this from the text map
						vecDialogueId.erase(maxDialogue);//removing from the dialogue static map
						vecEntityId.erase(maxElement);//removing from the static map 


						//resume gameplay 
						if (vecEntityId.size() == 0 && dialogueLoaded == true) {
							if (UpgradeManager::GetInstance().upgradeTutorial == true) {
								UpgradeManager::GetInstance().upgradeTutorial = false;//reset 
							}
							UiLayer::GetInstance().dialoguePause = false;//the dialogue is not paused
							UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
							dialogueLoaded = false;
						}
						ecs.deleteEntity(e);
						//std::cout << "dialogue deleted" << std::endl;
						deletedOnce = true;
					}
				}
			}
		}
		else if(!MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && oDialogue.isDialogueLoaded == true){
			deletedOnce = false;
		}
	}
}