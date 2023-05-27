// /******************************************************************************/
// /*!
// \file         BoulderScript.cpp
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

#include "BoulderScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>
#include <random>
#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>


using std::cout;
using std::endl;
int counter = 0;
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

BoulderScript::BoulderScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BoulderScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	//getting the starting position
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);
		startingPosX = oTransform.pos.x;
		startingPosY = oTransform.pos.y;
	}

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}
	
	UiLayer::GetInstance().m_currentLevelState = UiLayer::LevelState::level2;
}

void BoulderScript::update(float dt)
{
	
	if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play) {
		if (ecs.entityHas<transform2D>(id) && ecs.entityHas<transform2D>(playerID))
		{
			auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
			auto& playerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 
			//if player triggered hit box, activate the boulder 
			if (playerTransform.pos.x > (oTransform.pos.x - radiusOfDetectionX) && playerTransform.pos.x < (oTransform.pos.x + radiusOfDetectionX) &&
				playerTransform.pos.y >(oTransform.pos.y - radiusOfDetectionY) && playerTransform.pos.y < (oTransform.pos.y + radiusOfDetectionY)) {

				//activate the boulder 
				boulderActive = true;

			}

			//if the boulder is active
			if (boulderActive && stageCompleted == false) {
				float pos[] = { 0,0,0 };
				//Insert Sound Here #FallingRocks
				// 
				if (counter == 0) 
				{
					audio.playPannedSoundEffect(audio.sfxFallingRocks);
					counter = 1;
				}
				//SoundClass::getInstance()->setArgsAndPlay(FallingRocks, pos);
				//reset boulder
				if (oTransform.pos.x >= boulderEndPosX) {
					oTransform.pos.x = startingPosX;//reset the boulder pos 
					oTransform.pos.y = startingPosY;//reset the boulder pos 
					counter = 0;
					return;
				}

				//deactivate boulder if the player has completed the boulder run
				if (playerTransform.pos.x >= boulderEndPosX) {
					boulderActive = false;
					stageCompleted = true;
					//SoundClass::getInstance()->Pause(FallingRocks);
					return;
				}
			
				//slowly increase rotation & translation speed 
				if (rotationSpeed < maxRotationSpeed) {
					rotationSpeed += rotationSpeedIncrement * dt;
				}
				if (translateSpeed < maxTranslateSpeed) {
					translateSpeed += translationSpeedIncrement * dt;
				}

				//keep rotate right till it hits the wall
				oTransform.rotation -= rotationSpeed * dt;
				oTransform.pos.x += translateSpeed * dt;

				//camera shake effect
				if (activeCameraData && UiLayer::GetInstance().dialoguePause == false && UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Pause) {
					//activeCameraTransform->pos.x += randomnumgenfloat(-shakeLowerRange, shakeUpperRange);
					activeCameraTransform->pos.y += randomnumgenfloat(-shakeLowerRange, shakeUpperRange);
				}
			}

			//check if the player collided with the boulder
			std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
			for (entityID& eID_collided : collidedEntities)
			{
				//handle player collided with 
				if (eID_collided == playerID && !stageCompleted) {
					auto& playerHealth = ecs.getComponent<Com_Health>(playerID);
					playerHealth.health = 0;//kill player 
					boulderActive = false;
					stageCompleted = true;
				}
			}

		}
	}
}

void BoulderScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BoulderScript::createCopy()
{
	return std::make_shared <BoulderScript>();
}

std::string BoulderScript::getScriptName()
{
	return name;
}






