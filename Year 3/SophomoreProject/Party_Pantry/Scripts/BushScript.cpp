// /******************************************************************************/
// /*!
// \file         BushScript.cpp
// \project name PartyPantry
// \author(s)    Poh Chun Ren, 100%
// \date         18th Feb, 2022
// \brief        Script to destory bush as interactive object upon suction by player or shoot buy player.
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


#include <iostream>
#include "../../Engine/ECS/Systems/Logic.h"
#include "BushScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosion.h"
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

BushScript::BushScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BushScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//If entity has no collider, add it
	if (ecs.entityHas<AABB>(id) == false)
	{
		AABB aabb;
		ecs.addComponent<AABB>(id, aabb);
	}

	//If entity has no rigidbody, add it
	if (ecs.entityHas<rigidBody2D>(id) == false)
	{
		rigidBody2D rb;
		ecs.addComponent<rigidBody2D>(id, rb);
	}

	ecs.getComponent<rigidBody2D>(id).Type = rigidBody2D::Types::Static;
	ecs.getComponent<rigidBody2D>(id).forceManager.setMaxVel(300.f);

	if (ecs.entityHas<spriteCom>(id))
		ecs.getComponent<spriteCom>(id).isVisible = true;


	{
		//create EmitterComponent
		EmitterComponent EC{};
		//EC.emitterContainer.insert(std::make_pair("EmitterExplosion",std::make_shared<EmitterExplosion>()));
		EC.emitterContainer.insert(std::make_pair("EmitterExplosion",std::make_shared<EmitterExplosion>()));
		ecs.addComponent<EmitterComponent>(id, EC);
	}

	_state = BUSH_STATE::Untouched;
	startingScale = ecs.getComponent<transform2D>(id).scale;

	SuctionOriginalPosition = ecs.getComponent<transform2D>(id).pos;

	//add rb to bush
	if (!ecs.entityHas<rigidBody2D>(id))
	{
		ecs.addComponent<rigidBody2D>(id, rigidBody2D());
	}
}

void BushScript::update(float dt)
{
	if (_state == BUSH_STATE::Untouched)
	{
		//add aabb if missing (possible if player stop suction midway
		if (!ecs.entityHas<AABB>(id))
		{
			ecs.addComponent(id, AABB{});
		}

		//check if hit by bullet 
		//change state to FreeFloat
		//get pushed, scale down, and die
		std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
		for (entityID& eID_collided : collidedEntities)
		{
			//Handle bullet hitting bush
			if (ecs.entityHas<Com_type>(eID_collided) && ecs.getComponent<Com_type>(eID_collided).type == Com_type::type::bullet)
			{
				//maybe need produce some sound effect
				if (std::rand() < RAND_MAX / 2)
					audio.playPannedSoundEffect(audio.sfxGrassWalk1);
				else
					audio.playPannedSoundEffect(audio.sfxGrassWalk2);

				//adds sucking in particle effect
				if (ecs.entityHas<EmitterComponent>(id))
				{
					EmitterExplosion* emi = dynamic_cast<EmitterExplosion*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosion"].get());
					if (emi)
						emi->setTriggered(true);
				}

				//check bullet have rb
				if (ecs.entityHas<rigidBody2D>(eID_collided))
				{
					auto& rb = ecs.getComponent<rigidBody2D>(id);
					PushedVel = ecs.getComponent<rigidBody2D>(eID_collided).vel * 0.3f;
					Force newF(Force::ForceType::LINEAR_FORCE, PushedVel, 1.f);
					rb.forceManager.AddLinearForce("PushedForce", newF);
					rb.Type = rigidBody2D::Types::Dynamics;
				}

				//start to disable aabb
				ecs.removeComponent<AABB>(id);
				_state = BUSH_STATE::FreeFloat;
			}
		}

		//check if suction instead 
		//change state to BeingSucked_Shake
		//shake, get suck, scale down, and die
		//but if released mid way, will revert back to Untouch
		if (InSuctionRange == true)
		{
			_state = BUSH_STATE::BeingSucked_Shake;
		}
	}
	else if (_state == BUSH_STATE::FreeFloat)
	{
		//float
		auto& rb = ecs.getComponent<rigidBody2D>(id);
		Force newF(Force::ForceType::LINEAR_FORCE, PushedVel, 1.f);
		rb.forceManager.AddLinearForce("PushedForce", newF);

		life -= dt;
		if (life < DestructionTime)
			_state = BUSH_STATE::ScalingDown;
	}
	else if (_state == BUSH_STATE::ScalingDown)
	{
		//float
		auto& rb = ecs.getComponent<rigidBody2D>(id);
		Force newF(Force::ForceType::LINEAR_FORCE, PushedVel, 1.f);
		rb.forceManager.AddLinearForce("PushedForce", newF);

		//scale down
		life -= dt;
		float percentageRemain = life / DestructionTime;
		ecs.getComponent<transform2D>(id).scale = { startingScale.x * percentageRemain, startingScale.y * percentageRemain };

		//check if dead
		if (life < 0.f)
		{
			ecs.getComponent<spriteCom>(id).isVisible = false;
			_state = BUSH_STATE::Dead;
		}
	}
	else if (_state == BUSH_STATE::BeingSucked_Shake)
	{
		if (ecs.entityHas<AABB>(id))
		{	
			//start to disable aabb
			ecs.removeComponent<AABB>(id);
		}
	
		InSuctionTimer -= dt;
		
		//BUSH_STATE::BeingSucked_Shake
		auto& trans = ecs.getComponent<transform2D>(id);
		//float posVarianceX = ShakeLimit;
		//trans.pos.x = SuctionOriginalPosition.x + posVarianceX;

		if (RotationInitialised == false)
		{
			//getVectorRotationRadian is based on clockwise,
			//WHEREAS our rotation is counter clock wise.
			//THEREFORE 360 degree or 2pi, - calculated result
			RotationTarget = 6.24f - getVectorRotationRadian(BushToPlayer);
			float RotationTargetDegree = RadianToDegree(RotationTarget);
			float OppositeTransRotation = trans.rotation + 180.f;
			if (OppositeTransRotation > 360.f)
			{
				OppositeTransRotation -= 360.f;
			}

			//if can try to make it rotate towards the cloest side
			//if (trans.rotation + RotationTargetDegree < OppositeTransRotation)
			//if (trans.rotation + RotationTargetDegree)
			//{
				//player is at right
				RotationRequired = RotationTargetDegree - trans.rotation;
			//}
			//else
			//{
			//	//player is at left
			//	RotationRequired = trans.rotation - RotationTargetDegree + 3.14f;
			//}
			RotationInitialised = true;
			RotationReqPerSec = RotationRequired / (InSuctionTimerStart - InSuctionTimerFloating);
			FirstDistanceUponSuction = Length(BushToPlayer);
			//cout << "trans.rotation START: " << trans.rotation << endl;
			//cout << "RotationTarget: " << RotationTarget << endl;
			//cout << "RotationRequired :" << RotationRequired << endl;
			//cout << "RotationReqPerSec :" << RotationReqPerSec << endl;
		}
		
		trans.rotation += RotationReqPerSec * dt;

		if (InSuctionTimer < InSuctionTimerFloating)
		{
			RotationInitialised = false;
			_state = BUSH_STATE::BeingSucked_Floating;
			//cout << "trans.rotation END: " << trans.rotation << endl;
		}
	}
	else if (_state == BUSH_STATE::BeingSucked_Floating)
	{
		if (InSuctionRange == false)
		{
			//suction paused
			_state = BUSH_STATE::BeingSucked_Recovery;
		}
		else
		{
			InSuctionTimer -= dt;
			//BUSH_STATE::BeingSucked_Floating
			constexpr float maximumShrinkDistance = 250.0f; //start scaling down
			constexpr float minimumShrinkDistance = 100.f; //kill bush
			float distance = Length(BushToPlayer);
			auto& scale = ecs.getComponent<transform2D>(id).scale;
			
			


			//float to player
			auto& rb = ecs.getComponent<rigidBody2D>(id);

			Force newF(Force::ForceType::LINEAR_FORCE, BushToPlayer * 1000.f, 1.f);
			rb.forceManager.AddLinearForce("BushToPlayer", newF);

			//scaling down
			float percentageRemain = (distance - minimumShrinkDistance) / FirstDistanceUponSuction;
			ecs.getComponent<transform2D>(id).scale = { startingScale.x * percentageRemain, startingScale.y * percentageRemain };

			if (distance < minimumShrinkDistance)
			{
				if (std::rand() < RAND_MAX / 2)
					audio.playPannedSoundEffect(audio.sfxGrassWalk1);
				else
					audio.playPannedSoundEffect(audio.sfxGrassWalk2);

				//adds sucking in particle effect
				if (ecs.entityHas<EmitterComponent>(id))
				{
					EmitterExplosion* emi = dynamic_cast<EmitterExplosion*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterExplosion"].get());
					if (emi)
						emi->setTriggered(true);
				}

				//killing
				ecs.getComponent<spriteCom>(id).isVisible = false;
				if (ecs.entityHas<AABB>(id))
					ecs.removeComponent<AABB>(id);
				_state = BUSH_STATE::Dead;
			}
			else if (distance < maximumShrinkDistance)
			{
				//scaling down
				scale.x = startingScale.x * (distance / maximumShrinkDistance);
				scale.y = startingScale.y * (distance / maximumShrinkDistance);
			}
			else
				scale = startingScale;

			//reset boolean
			InSuctionRange = false;
		}
	}
	else if (_state == BUSH_STATE::BeingSucked_Recovery)
	{
		auto& trans = ecs.getComponent<transform2D>(id);
		auto& rb = ecs.getComponent<rigidBody2D>(id);
		if (RecoveryInitialised == false)
		{
			RecoveryInitialised = true;
			RecoveryTimer = RecoveryTime;
			rb.vel = { 0.f, 0.f };
			SuctionOriginalPosition = trans.pos;
			rb.Type = rigidBody2D::Types::Static;
			InSuctionTimer = InSuctionTimerStart;

			//init 
			RecoveryRotPerSec = (0.f - trans.rotation) / RecoveryTime;
			RecoveryScalePerSec = (startingScale - trans.scale) / RecoveryTime;
		}
		
		RecoveryTimer -= dt;

		//Player resume suction
		if (InSuctionRange == true )
		{
			_state = BUSH_STATE::BeingSucked_Floating;
			RecoveryInitialised = false;
		}
		else
		{
			//if Recovery in progress
			if (RecoveryTimer > 0.f)
			{
				trans.scale.x += RecoveryScalePerSec.x * dt;
				trans.scale.y += RecoveryScalePerSec.y * dt;
				trans.rotation += RecoveryRotPerSec * dt;
			}
			else
			{
				//RecoveryTime Ended
				_state = BUSH_STATE::Untouched;
				RecoveryInitialised = false;
			}
		}
	}
	
}

void BushScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> BushScript::createCopy()
{
	return std::make_shared <BushScript>();
}

std::string BushScript::getScriptName()
{
	return name;
}
