// /******************************************************************************/
// /*!
// \file         BlenderProximityScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the blender proximity script
// \lines        51
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <Sound/SoundManagerV2.h>
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"

#include "../../Engine/InputMgr/InputMgr.h"//input mgr
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities 
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/AssetManager/AssetManager.h"
#include "BlenderProximityScript.h"
#include <iostream>

using std::cout;
using std::endl;

BlenderProximityScript::BlenderProximityScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderProximityScript::init(float dt, unsigned int idToBeSet)
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

void BlenderProximityScript::update(float dt)
{
	//for the dialogue of first collect of the blender 
	static bool firstCollect;
	static bool firstDialogueLoaded;
	static entityID firstDialogueID;

	if (ecs.entityHas<transform2D>(id)) {

		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& oMaterial = ecs.getComponent<material>(id);//getting the material component 

		if (textLoaded == true) {
			ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
		}

		//show the pulsating 
		if (collected == false) {
			if (pulsatingFadeIn == true) {
				oMaterial.color.x >= 1.f ? pulsatingFadeIn = false : oMaterial.color.x += 0.5f * dt;
				oMaterial.color.y >= 1.f ? pulsatingFadeIn = false : oMaterial.color.y += 0.5f * dt;
				oMaterial.color.z >= 1.f ? pulsatingFadeIn = false : oMaterial.color.z += 0.5f * dt;
			}
			else {
				oMaterial.color.x <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.x -= 0.5f * dt;
				oMaterial.color.y <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.y -= 0.5f * dt;
				oMaterial.color.z <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.z -= 0.5f * dt;
			}
		}

		//get the only registered player with abilities
		for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
			auto& eInnerTransform = ecs.getComponent<transform2D>(eInner);//getting the transform of the player 
			//if the player is near to the blender, show the dialogue , AABB
			if (eInnerTransform.pos.x > (oTransform.pos.x - radiusOfDetection) && eInnerTransform.pos.x < (oTransform.pos.x + radiusOfDetection) &&
				eInnerTransform.pos.y >(oTransform.pos.y - radiusOfDetection) && eInnerTransform.pos.y < (oTransform.pos.y + radiusOfDetection)) {

				//show the dialogue 
				if (textLoaded == false) {
					textId = ecs.createEntityFromFile("Resources/PresetEntities/blendText.json");
					//setting the position of the text 
					ecs.getComponent<transform2D>(textId).pos.x = oTransform.pos.x;
					ecs.getComponent<transform2D>(textId).pos.y = oTransform.pos.y;
					textLoaded = true;
				}
				
				if(collected == false) ecs.getComponent<spriteCom>(textId).isVisible = true;//set to visible 
				
				if (KeyManager.isKeyTriggered('f',true) && collected == false) {

					//Collection sound
					audio.playPannedSoundEffect(audio.sfxPop);

					AssetManager::GetInstance().blenderCollected = true;//set blender collected to true 

					collected = true;//indicates that it's been collected 
						
					auto& oSprite = ecs.getComponent<spriteCom>(id);
					oSprite.isVisible = false;

					//show dialogue 
					if (firstCollect == false) {


						//if the dialogue is not loaded and the dialogue to show is true 
						if (firstDialogueLoaded == false) {
							//creating the entity right below the player pos 
							firstDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/dialogueBlender.json");
							//setting the new position 
							ecs.getComponent<transform2D>(firstDialogueID).pos.x = ecs.getComponent<transform2D>(eInner).pos.x;
							ecs.getComponent<transform2D>(firstDialogueID).pos.y = ecs.getComponent<transform2D>(eInner).pos.y;
							firstDialogueLoaded = true;
						}

					}
				}

				if (firstDialogueLoaded == true) {
					//check if the player clicked 
					UiLayer::GetInstance().dialoguePause = true;//the dialogue is pause
					UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game 

					//if left clicked 
					if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON)) {
						//ecs.deleteEntity(firstDialogueID);//delete the entity 
						firstCollect = true;
						UiLayer::GetInstance().dialoguePause = false;//the dialogue is pause
						firstDialogueLoaded = false;
						UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
					}
				}

			}
		}
	}


}

void BlenderProximityScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BlenderProximityScript::createCopy()
{
	return std::make_shared <BlenderProximityScript>();
}

std::string BlenderProximityScript::getScriptName()
{
	return name;
}






