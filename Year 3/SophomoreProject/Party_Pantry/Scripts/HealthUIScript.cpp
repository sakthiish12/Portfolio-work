// /******************************************************************************/
// /*!
// \file         HealthUIScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the health ui script 
// \lines        
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

#include "HealthUIScript.h"
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities system 
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

HealthUIScript::HealthUIScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void HealthUIScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//getting the 
	//get the only registered player with abilities
	for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
		if (ecs.entityHas<Com_Health>(eInner))
		{
			auto& oHealth = ecs.getComponent<Com_Health>(eInner);//getting player health
			auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform 

			initialTotalHealth = oHealth.health;//init health 
			prevHealth = (float)oHealth.health;//tracks the previous health 

			//getting the current scale of the entity and divide it by the initial health 
			deductionScale = oTransform.scale.x / oHealth.health;
		}
	}
}

void HealthUIScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform 

		auto& oSprite = ecs.getComponent<spriteCom>(id);
		//check if camera transition is in progress 
		if (UiLayer::GetInstance().cameraTransition) {
			oSprite.isVisible = false;
		}
		else {
			oSprite.isVisible = true;
		}
		//if player has been damaged, 
		for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
			auto& oHealth = ecs.getComponent<Com_Health>(eInner);//getting player health
			
																 
																 
			//health must be more than 0
			if (oHealth.health >= 0) {
				//if the health has been updated 
				if (prevHealth != oHealth.health) {
					oTransform.pos.x = oTransform.pos.x - ((deductionScale / 2) * (prevHealth - oHealth.health));//make the left position consistent 
					prevHealth = (float)oHealth.health;//update the previous health 
				}

				//if the health is already not the max
				if (KeyManager.isKeyTriggered('n', true) && oHealth.health < initialTotalHealth)
				{
					oHealth.health = initialTotalHealth;//reset the health 
				}

				oTransform.scale.x = deductionScale * oHealth.health;//setting the scale of the health bar to the 
			}
			else if (oHealth.health < 0) {
				oTransform.scale.x = 0;//the scale is 0
			}

			//if health lesser than half, make it glow red
			if (oHealth.health < initialTotalHealth / 2) {

				auto& oMaterial = ecs.getComponent<material>(id);//getting the material 
				if (pulsatingFadeIn == true) {
					//oMaterial.color.x >= 1.f ? pulsatingFadeIn = false : oMaterial.color.x += 0.5f * dt;
					oMaterial.color.y >= 1.f ? pulsatingFadeIn = false : oMaterial.color.y += 0.5f * dt;
					oMaterial.color.z >= 1.f ? pulsatingFadeIn = false : oMaterial.color.z += 0.5f * dt;
				}
				else {
					//oMaterial.color.x <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.x -= 0.5f * dt;
					oMaterial.color.y <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.y -= 0.5f * dt;
					oMaterial.color.z <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.z -= 0.5f * dt;
				}
			}
		}
	}
}

void HealthUIScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

std::shared_ptr<scriptBase> HealthUIScript::createCopy()
{
	return std::make_shared <HealthUIScript>();
}

std::string HealthUIScript::getScriptName()
{
	return name;
}






