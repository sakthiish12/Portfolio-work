// /******************************************************************************/
// /*!
// \file         BoulderCameraScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Feb, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the abilities UI
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

#include "BoulderCameraTransitionScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>
#include <random>


using std::cout;
using std::endl;


BoulderCameraScript::BoulderCameraScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BoulderCameraScript::init(float dt, unsigned int idToBeSet)
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
		}

		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::boulder) {
			boulderID = e.first;//boulder is found 
			break;
		}
	}

	originalCameraHeight = activeCameraData->height;//storing the original camera height 

	UiLayer::GetInstance().m_currentLevelState = UiLayer::LevelState::level2;
}

void BoulderCameraScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 
		//if player triggered hit box, activate the boulder 
		if (playerTransform.pos.x > (oTransform.pos.x - (oTransform.scale.x/2.f)) && playerTransform.pos.x < (oTransform.pos.x + (oTransform.scale.x/2.f)) &&
			playerTransform.pos.y >(oTransform.pos.y - (oTransform.scale.y/2.f)) && playerTransform.pos.y < (oTransform.pos.y + (oTransform.scale.y/2.f))) {

			//activate the transition
			isActive = true;

		}


		//if transition is active
		if (isActive && isCompleted == false) {
			activeCameraData->isFixedToPlayer = false;//disable follow player 

			//freeze the game 
			UiLayer::GetInstance().dialoguePause = true;//the dialogue is pause
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game 
			UiLayer::GetInstance().cameraTransition = true;

			auto& oBoulder = ecs.getComponent<transform2D>(boulderID);

			//skippable 
			if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON)) {
				skippedTransition = true;
			}


			if (skippedTransition) {
				//unfreeze the game 
				UiLayer::GetInstance().dialoguePause = false;//the dialogue is pause
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
				UiLayer::GetInstance().cameraTransition = false;
				activeCameraData->isFixedToPlayer = true;//disable follow player 
				activeCameraData->height = originalCameraHeight;//reset 
				isActive = false;
				isCompleted = true;
			}

			//getting the vector from player to boulder 
			if (vectorReceived == false) {
				playerToBoulder = { oBoulder.pos.x - playerTransform.pos.x , oBoulder.pos.y - playerTransform.pos.y };//getting the vector 
				playerToBoulder = Mathf::Normalize(playerToBoulder);//normalize vector 
			}

			//check if camera has reached boulder 
			if (activeCameraTransform->pos.y >= oBoulder.pos.y) {
				activeCameraTransform->pos.x += rateOfTransition * playerToBoulder.x * dt; 
				activeCameraTransform->pos.y += rateOfTransition * playerToBoulder.y * dt; 
			}
			else {
				if (activeCameraData->height >= cameraMinHeight) {
					activeCameraData->height -= heightTransition * dt;
				}
				else {
					//unfreeze the game 
					UiLayer::GetInstance().dialoguePause = false;//the dialogue is pause
					UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
					UiLayer::GetInstance().cameraTransition = false;
					activeCameraData->isFixedToPlayer = true;//disable follow player 
					activeCameraData->height = originalCameraHeight;//reset 
					isActive = false;
					isCompleted = true;
				}

			}
		}


	}
}

void BoulderCameraScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BoulderCameraScript::createCopy()
{
	return std::make_shared <BoulderCameraScript>();
}

std::string BoulderCameraScript::getScriptName()
{
	return name;
}






