// /******************************************************************************/
// /*!
// \file         DamagedScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the damaged script 
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
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include "DamagedScript.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>
#include <random>
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"

/**
\brief - random number generator for integers
\param start - the starting integer range 
\param end - the ending integer range 
\returns - the random number within the range 
**/
template<typename T>
T randomnumgen(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_int_distribution<T> random(start, end);
	return random(mt);
}


template<typename T>
T randomnumgenfloat(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_real_distribution<T> random(start, end);
	return random(mt);
}

using std::cout;
using std::endl;

DamagedScript::DamagedScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void DamagedScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	if (ecs.entityHas<Com_Health>(id)) {
		prevHealth = ecs.getComponent<Com_Health>(id).health;//getting the inital health 
	}
}

void DamagedScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<material>(id)) {
		auto& oMaterial = ecs.getComponent<material>(id);//getting the material component 

		//lighten it 
		oMaterial.color.x = 1.f;
		oMaterial.color.y = 1.f;
		oMaterial.color.z = 1.f;

		//if damaged, make it to red 
		if (prevHealth != ecs.getComponent<Com_Health>(id).health && ecs.getComponent<Com_Health>(id).health > 1 || currentTimer >= 0.f && ecs.getComponent<Com_Health>(id).health > 1) {

			currentTimer -= dt;//decrement by the dt

			//start the timer for the effect
			if (prevHealth != ecs.getComponent<Com_Health>(id).health) {
				currentTimer = effectTimer;
			}

			prevHealth = ecs.getComponent<Com_Health>(id).health;//setting the previous health 
			oMaterial.color.y = oMaterial.color.y / 4.5f;
			oMaterial.color.z = oMaterial.color.z / 4.5f;


			//post processing blur effect
			/*if (PostProcessing::GetInstance().currentEffects != PostProcessing::Effects::effectBlur) {
				PostProcessing::GetInstance().toggleEffects = true;
				PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectBlur;//setting to blur screen sha
			}*/


			//camera shake effect
			if (activeCameraData) {
				activeCameraTransform->pos.x += randomnumgenfloat(-shakeLowerRange, shakeUpperRange);
				activeCameraTransform->pos.y += randomnumgenfloat(-shakeLowerRange, shakeUpperRange);
			}

			//Apply particle effect===============
							// //TODO: set to destory emitter and componet itself to save on mem space
							//if no EC, add EC
			if (!ecs.entityHas<EmitterComponent>(id))
			{
				EmitterComponent EC{};
				EC.emitterContainer.insert(std::make_pair("EmitterExplosionDamaged", std::make_shared<EmitterExplosionDamaged>()));
				ecs.addComponent<EmitterComponent>(id, EC);
				dynamic_cast<EmitterExplosionDamaged*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosionDamaged"].get())->_newParticleProperties.startColor = { 1.f, 0.8f, 0.2f, 0.5f };
			}
			EmitterExplosionDamaged* emi = dynamic_cast<EmitterExplosionDamaged*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosionDamagedPlayer"].get());
			if (emi == nullptr)
			{
				//Dont use map insert after using it in map square bracket overloading
				ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosionDamagedPlayer"] = std::make_shared<EmitterExplosionDamaged>();
				emi = dynamic_cast<EmitterExplosionDamaged*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosionDamagedPlayer"].get());
				emi->_newParticleProperties.startColor = { 1.f, 0.8f, 0.2f, 0.5f };
				emi->setTriggered(true);
			}
			else
				emi->setTriggered(true);
			//Apply particle effect===============END
		}
		else {

			//check if anything has changed to as not to unnecessary change 
			if (PostProcessing::GetInstance().currentEffects != PostProcessing::Effects::effectNormal) {
				PostProcessing::GetInstance().toggleEffects = true;
				PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectNormal;//setting to normal
			}
		}
		
	}

}

void DamagedScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> DamagedScript::createCopy()
{
	return std::make_shared <DamagedScript>();
}

std::string DamagedScript::getScriptName()
{
	return name;
}






