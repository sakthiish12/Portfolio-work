// /******************************************************************************/
// /*!
// \file         CaveTeleportingScript.cpp
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

#include "CaveTeleportingScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include <iostream>
#include <Sound/SoundManagerV2.h>
using std::cout;
using std::endl;
bool teleportActived = false;
CaveTeleportScript::CaveTeleportScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void CaveTeleportScript::init(float dt, unsigned int idToBeSet)
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
int counter5 = 0;
void CaveTeleportScript::update(float dt)
{
	
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 
		//if player triggered hit box, activate the boulder 
		if (playerTransform.pos.x > (oTransform.pos.x - radiusOfDetection) && playerTransform.pos.x < (oTransform.pos.x + radiusOfDetection) &&
			playerTransform.pos.y >(oTransform.pos.y - radiusOfDetection) && playerTransform.pos.y < (oTransform.pos.y + radiusOfDetection)) {

			//activate fade in and out
			teleportActive = true;
			teleportActived = true;
			//SoundClass::getInstance()->channels[background]->setPaused(true);
			//SoundClass::getInstance()->channels[CaveBgm]->setPaused(false);

		}
		
		//if it is teleporting 
		if (teleportActive) {
			//check if it's fading out 
			if (darkenToggle) {
				//decrease the alpha of the texture 
				PostProcessing::GetInstance().m_AlphaValue <= 0.f ? PostProcessing::GetInstance().m_AlphaValue = 0.f : PostProcessing::GetInstance().m_AlphaValue -= speedOfTransition * dt;
				
				if (counter5 == 0) {
					audio.changeBGM(audio.bgmNewLevel2);
					counter5 = 1;
				}
				
				
				//if the player wants to skip this cut scene 
				if (PostProcessing::GetInstance().m_AlphaValue <= 0.f) {
					//teleport the player to the cave position 
					playerTransform.pos.x = cavePosX;
					playerTransform.pos.y = cavePosY;
					darkenToggle = false;//setting to fade in 
				}
			}
			else {
				//fade in
				PostProcessing::GetInstance().m_AlphaValue >= 1.f ? PostProcessing::GetInstance().m_AlphaValue = 1.f : PostProcessing::GetInstance().m_AlphaValue += speedOfTransition * dt;

				if (PostProcessing::GetInstance().m_AlphaValue >= 1.f) {
					//stop the transition 
					teleportActive = false;
					counter5 = 0;
				}

			}
		}
	}
}

void CaveTeleportScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> CaveTeleportScript::createCopy()
{
	return std::make_shared <CaveTeleportScript>();
}

std::string CaveTeleportScript::getScriptName()
{
	return name;
}






