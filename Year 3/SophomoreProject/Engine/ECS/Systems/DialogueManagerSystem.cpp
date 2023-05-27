// /******************************************************************************/
// /*!
// \file         DialogueManagerSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         2nd March , 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the dialogue system
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "DialogueManagerSystem.h"
#include "AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../AssetManager/UpgradeManager.h"

std::shared_ptr<DialogueManagerSystem> DialogueManagerSysPtr;
#define MAX_TEXT_TO_DISPLAY 70


void DialogueManagerSystem::update(float dt) {

	for (auto const& e : getEntitiesRegistered()) {
		if (ecs.entityHas<Com_DialogueManager>(e) && ecs.entityHas<Com_Font>(e)) {
			auto& oDialogue = ecs.getComponent<Com_DialogueManager>(e);//getting the dialogue com 
			auto& oFont = ecs.getComponent<Com_Font>(e);//getting the font com 

			//if it's completed, and clicked 
			if (oDialogue.isCompleted) {
				//if there is not second entity 
				if (oDialogue.secondEntityID == -1) {
					oDialogue.allFullyLoaded = true;
					//if left clicked
					if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON)) {
						//delete the entity
						ecs.deleteEntity(e);
						//std::cout << "text entity destroyed\n";
					}
				}
				else {
					//double check
					if (ecs.entityHas<Com_DialogueManager>(oDialogue.secondEntityID)) {
						//toggle to check if both is completed 
						if (ecs.getComponent<Com_DialogueManager>(oDialogue.secondEntityID).isCompleted == true) {
							oDialogue.allFullyLoaded = true;
						}

						//check if the second entity is completed 
						if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && ecs.getComponent<Com_DialogueManager>(oDialogue.secondEntityID).isCompleted == true) {
							//delete the entity
							ecs.deleteEntity(e);
							//std::cout << "text entity destroyed\n";
						}
					}
				}
			}

			//if it's active
			if (oDialogue.isActive) {

				//if the user wants to skip and the second one is not even loaded 
				if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && oDialogue.dialogueStorage.size() > MAX_TEXT_TO_DISPLAY && oDialogue.isSerialized == false && oDialogue.isPrevLoaded == false) {
					//there is still a second line not loaded in 
					oFont.text = oDialogue.dialogueStorage.substr(0, MAX_TEXT_TO_DISPLAY);
					oDialogue.currentTimer = 0.f;
					oDialogue.textIndex = MAX_TEXT_TO_DISPLAY;//setting to the max text 
					oDialogue.toLoadTextFully = true;
					oDialogue.isCompleted = true;//set this current dialogue portion is completed 
					oDialogue.allFullyLoaded = true;//indicates that it is all fully loaded 
				}
				else if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && oDialogue.isPrevLoaded == false) {
					//the user wants to skip and there is only one message 
					oFont.text = oDialogue.dialogueStorage;//pass the entire message in 
				}

				oDialogue.isPrevLoaded = false;

				//based on the speed
				if (oDialogue.currentTimer <= 0.f) {
					oDialogue.currentTimer = oDialogue.textSpeed;//resetting timer 
					if (oDialogue.isSerialized == false) {
						//add new char to the font if the text is not full
						if (oFont.text.size() != oDialogue.dialogueStorage.size()) {
							oFont.text += oDialogue.dialogueStorage[oDialogue.textIndex++];
						}
					}

					//if the size of the text is hitting the max, create another text below it 
					if (oDialogue.textIndex > MAX_TEXT_TO_DISPLAY) {
						//create another text below only once 
						if (oDialogue.isSerialized == false) {
							//getting the player 
							for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
								std::string tmpStr = oDialogue.dialogueStorage;
								oDialogue.dialogueStorage = oDialogue.dialogueStorage.substr(0, MAX_TEXT_TO_DISPLAY+1);//cut short the thing 
								oDialogue.isSerialized = true;//it is serialized 
								bool toLoadFully = oDialogue.toLoadTextFully;
								entityID textEntityId;
								if (UpgradeManager::GetInstance().upgradeTutorial) {
									textEntityId = ecs.createEntityFromFile("Resources/PresetEntities/EmptyTextCamera.json");
								}
								else {
									textEntityId = ecs.createEntityFromFile("Resources/PresetEntities/EmptyText.json");
								}
								oDialogue.secondEntityID = textEntityId;
								//oDialogue.extendedDialogue.emplace_back(textEntityId);//push back to the vector that stores all the extended dialogue 
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
									ecs.setEntityParent(textEntityId, eInner);//setting the parent to the player 
								}
								ecs.getComponent<Com_DialogueManager>(textEntityId).textIndex = 0;//setting the dialogue to be true 
								ecs.getComponent<Com_DialogueManager>(textEntityId).isActive = true;//setting the dialogue to be true 
								ecs.getComponent<Com_DialogueManager>(textEntityId).dialogueStorage = tmpStr.substr(MAX_TEXT_TO_DISPLAY, tmpStr.size());
								if (toLoadFully) {
									ecs.getComponent<Com_Font>(textEntityId).text = ecs.getComponent<Com_DialogueManager>(textEntityId).dialogueStorage;
									ecs.getComponent<Com_DialogueManager>(textEntityId).isActive = false;//setting the dialogue to be true 
									ecs.getComponent<Com_DialogueManager>(textEntityId).isCompleted = true;
								}
							}
						}
					}

				}
				else {
					oDialogue.currentTimer -= dt;//decerement timer 
				}

				//if the dialogue is fully added 
				if (oFont.text.size() == oDialogue.dialogueStorage.size()) {
					oDialogue.isActive = false;//set to false 
					oDialogue.isCompleted = true;
				}
			}
		}
	}
}