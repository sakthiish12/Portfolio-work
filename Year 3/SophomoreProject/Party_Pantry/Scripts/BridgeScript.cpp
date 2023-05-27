// /******************************************************************************/
// /*!
// \file		 BridgeScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the bridge extending script 
// \lines        51
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"

#include "../../Engine/InputMgr/InputMgr.h"//input mgr
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities 
#include "../../Engine/Sound/SoundManagerV2.h"
#include "BridgeScript.h"//bridge script 
#include "LeverActivationScript.h"//bridge script 
#include "../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>

using std::cout;
using std::endl;
int counter1 = 0;
BridgeScript::BridgeScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BridgeScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//auto& oTransform = ecs.getComponent<transform2D>(id);
	startingXPos = 2195.f;
	expectedEndingXpos = startingXPos + maxLength / 2.f;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}

	//If entity has no collider, add it
	if (!UpgradeManager::GetInstance().level1Completed) {
		if (ecs.entityHas<AABB>(id) == false)
		{
			auto& trans = ecs.getComponent<transform2D>(id);
			trans.scale.x = 1.f;
			AABB aabb;

			//aabb.offset_pos.x = maxLength / 4.f;
			aabb.offset_pos.x = 0.f;
			aabb.scale.y = 1.2f;
			aabb.scale.x = maxLength;
			ecs.addComponent<AABB>(id, aabb);
		}
	}
}

void BridgeScript::update(float dt)
{
	if (connectedToEvents == false)
	{
		if (leverScriptPtr != nullptr)
		{
			LeverHasBeenTriggeredStatus.Connect(leverScriptPtr->LeverHasBeenTriggeredStatus);
			connectedToEvents = true;
		}
	}

	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<transform2D>(id)) {
		if (!UpgradeManager::GetInstance().level1Completed) {

			if (activated == false) 
			{
				ecs.getComponent<spriteCom>(id).isVisible = false;
			}

			auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

			//get the only registered player with abilities


			//if the scale has not reached max 
			if (oTransform.scale.x <= maxLength && activated == true) {
				float scaleIncreaseThisFrame = sizeIncreasingScale * dt;

				//if b is activated, activate the bridge slowly moving it 
				oTransform.pos.x += scaleIncreaseThisFrame / 2.f;//make the left position consistent 
				oTransform.scale.x += scaleIncreaseThisFrame;//setting the scale of the health bar to the 
				if (counter1 == 0) {
					audio.playPannedSoundEffect(audio.sfxOperateLever);
					counter1 = 1;
				}
				
					//modify AABB
				if (ecs.entityHas<AABB>(id))
				{
					auto& aabb = ecs.getComponent<AABB>(id);
					aabb.scale.x = (maxLength - oTransform.scale.x) * 0.5f / oTransform.scale.x;
				}

				//std::cout << __func__ << " called , bridge is changing with scaleX = " << oTransform.scale.x << std::endl;

				//freeze the game 
				UiLayer::GetInstance().dialoguePause = true;//the dialogue is pause
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game 
				UiLayer::GetInstance().cameraTransition = true;

				
			}

			if (oTransform.scale.x > maxLength && activated == true && completedOpening == false)
			{
				UiLayer::GetInstance().dialoguePause = false;//the dialogue is pause
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
				UiLayer::GetInstance().cameraTransition = false;
				ecs.removeComponent<AABB>(id);
				completedOpening = true;
			}
		}
		else {
			auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
			oTransform.scale.x = maxLength;
		}
	}


}

void BridgeScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BridgeScript::createCopy()
{
	return std::make_shared <BridgeScript>();
}

std::string BridgeScript::getScriptName()
{
	return name;
}

void BridgeScript::updateLeverStatus(bool a)
{
	activated = a;
	if (activated)
	{
		//std::cout << __func__ << " called , activated is " << activated << std::endl;
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

		oTransform.scale.x = 0.f;
		oTransform.pos.x = startingXPos;
		//activated = true;
		ecs.getComponent<spriteCom>(id).isVisible = true;
		if (ecs.entityHas<AABB>(id))
		{
			ecs.getComponent<AABB>(id).offset_pos.x = maxLength * 0.5f;
		}
	}
}
