// /******************************************************************************/
// /*!
// \file         FallingStepScript.cpp
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
#include "FallingStepScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

FallingStepScript::FallingStepScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void FallingStepScript::init(float dt, unsigned int idToBeSet)
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

	//getting original position
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);
		originalPosX = oTransform.pos.x;
		originalPosY = oTransform.pos.y;
		originalScaleX = oTransform.scale.x;
		originalScaleY = oTransform.scale.y;
	}
}

void FallingStepScript::update(float dt)
{
	if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play) {
		if (ecs.entityHas<transform2D>(id))
		{
			if (!UiLayer::GetInstance().dialoguePause) {
				//if player stepped on it, start the bridge 
				auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
				auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 
				//if player triggered hit box, activate the boulder 
				if (playerTransform.pos.x > (oTransform.pos.x - (oTransform.scale.x/2) + offSetDetectionX) && playerTransform.pos.x < (oTransform.pos.x + oTransform.scale.x) &&
					playerTransform.pos.y >(oTransform.pos.y - oTransform.scale.y/2.f) && playerTransform.pos.y < (oTransform.pos.y + oTransform.scale.y/2.f) && shrinkingBridge == false) {

					//activate the bridge 
					shrinkingBridge = true;
					float pos[] = { 0,0,0 };

					//Insert Sound Here #BridgeExtension
					audio.playPannedSoundEffect(audio.sfxBridgeExtend);
					//SoundClass::getInstance()->setArgsAndPlay(bridgeExtension2,pos);
					//SoundClass::getInstance()->Pause(background);
				
				}

				//bridge to shrink rightwards
				if (shrinkingBridge == true && oTransform.scale.x > 0.f) {	
					oTransform.scale.x -= (scaleShrinking) * dt;
					oTransform.pos.x += (scaleShrinking/2.f) * dt;
				}
				else if (oTransform.scale.x <= shrinkBridgeBuffer) {
					oTransform.scale.x = 0.f;
					//shrinkingBridge = false;
					shrinkCompleted = true;//shrinking is completed 
					shrinkingBridge = false;
				}

				//check if player is outside of the bridge 
				if (shrinkingBridge == true) {
					//when it's true check if the player exceeded the boundaries 
					if (playerTransform.pos.x > (oTransform.pos.x - (oTransform.scale.x/2.f) + offSetFallingDetectionX)) {
						//safe
					}
					else {
						//dies 
						if (oTransform.scale.x >= 50.f) {
							ecs.getComponent<Com_Health>(playerID).health = 0;//kill the player 
						}
					}
				}

				if (shrinkCompleted == true) {
					//if the player step into the original area of the bridge, it dies. 
					if ((playerTransform.pos.x > ((originalPosX - originalScaleX/2.f) + offSetDetectionXOldLeft)) && (playerTransform.pos.x < ((originalPosX + originalScaleX/2.f) - offSetDetectionXOld)) &&
						(playerTransform.pos.y > (originalPosY - originalScaleY/2.f)) && (playerTransform.pos.y < (originalPosY + originalScaleY/2.f))) {
						ecs.getComponent<Com_Health>(playerID).health = 0;//kill the player 
					}
				}
			}
		}
	}
}

void FallingStepScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> FallingStepScript::createCopy()
{
	return std::make_shared <FallingStepScript>();
}

std::string FallingStepScript::getScriptName()
{
	return name;
}






