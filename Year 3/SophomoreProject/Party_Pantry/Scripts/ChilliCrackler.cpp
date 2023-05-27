// /******************************************************************************/
// /*!
// \file         ChilliCrackler.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         31st Mar, 2022
// \brief        This file contains the definitions for chilli crackler enemy's
//               logic
// \lines        312
//
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"

#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/InputMgr/InputMgr.h"

#include "ChilliCrackler.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"

#include "PlayerController.h"

//ctor
ChilliCrackler::ChilliCrackler(const char* scriptName) :
	EnemyBehaviour(scriptName, 100.0f, 1.0f)
{

}

void ChilliCrackler::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
	state = scriptBase::ScriptState::UPDATE;
}

void ChilliCrackler::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play) {

		//Disable flash if not preparing
		if (currentAction != action::prepare)
		{
			flashOn = false;
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		}

		//Update previous direction tracker
		previousFacingDirection = currentFacingDirection;

		//Reduce stunned time remaining.
		if (stunTimeRemaining >= 0.0f && UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
			stunTimeRemaining -= dt;
		if (stunTimeRemaining < 0.0f)
			stunTimeRemaining = 0.0f;

		//To check if the player is found
		entityID playerEntity{ 0 };
		bool playerFound{ false };

		//(attempt to) get the player entity
		for (auto const& e : ecs.getEntityNames())
		{
			if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
			{
				playerFound = true;
				playerEntity = e.first;
				break;
			}
		}

		//Only process AI when player is found
		if (playerFound &&
			ecs.entityHas<transform2D>(id) &&
			ecs.entityHas<AABB>(id) &&
			ecs.entityHas<rigidBody2D>(id) &&
			ecs.entityHas<spriteCom>(id) &&
			ecs.entityHas<Com_Health>(id) &&
			ecs.entityHas<transform2D>(playerEntity) &&
			ecs.entityHas<AABB>(playerEntity) &&
			ecs.entityHas<Com_Health>(playerEntity))
		{
			//GET VARIABLES////////////////////////////////////////////////////////
			transform2D& enemyTransform = ecs.getComponent<transform2D>(id);
			//AABB& enemyAABB = ecs.getComponent<AABB>(id);
			rigidBody2D& enemyRB = ecs.getComponent<rigidBody2D>(id);
			spriteCom& enemySprite = ecs.getComponent<spriteCom>(id);
			Com_Health& enemyHealth = ecs.getComponent<Com_Health>(id);

			transform2D& playerTransform = ecs.getComponent<transform2D>(playerEntity);
			//AABB& playerCollider = ecs.getComponent<AABB>(playerEntity);
			//Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);

			//Distance between player and enemy
			float distance = Mathf::Length(enemyTransform.pos - playerTransform.pos);

			//STATE HANDLING///////////////////////////////////////////////////////

			//Transition into Death State from any state if health <= 0
			//Transitionable from any state
			if (currentAction != action::death &&
				enemyHealth.health <= 0)
			{
				currentAction = action::death;
				stunMultiplier = 0.0f;
				audio.playPannedSoundEffect(audio.sfxEnemyDeath);
				//SoundClass::getInstance()->PlayAgain(enemyDeath);

				//Set animation based off facing
				changeFacingAnimation(enemySprite, "BackDeath", "FrontDeath", "FrontDeath", "FrontDeath");
			}

			//Transition into stun state if not dying and stun timer > 0
			//Transitionable from any state but death
			if (currentAction != action::death &&
				currentAction != action::stunned &&
				currentAction != action::suction &&
				stunTimeRemaining > 0.0f)
			{
				//TODO extra VFX when stunned in a vulnerable state
				if (currentAction == action::fire)
					audio.playPannedSoundEffect(audio.sfxDing);

				currentAction = action::stunned;

				//Set animation based off facing
				changeFacingAnimation(enemySprite, "StunnedUp", "StunnedDown", "StunnedLeft", "StunnedRight");
			}

			switch (currentAction)
			{
				//Idle state - Waiting for player to arrive
				//Transition:
				//alert (near enough)
				//stunned (stun timer > 0)
				//death (lost health)
			case (action::idle):
			{
				stunMultiplier = 1.0f;
				enemySprite.speed = 0.0f;
				//Wait for distance to be near enough to start
				if (distance < 800.0f)
				{
					currentAction = action::alert;
					changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
					actionTimeRemaining = 1.0f;
					break;
				}
				break;
			}

			//Alert state - Noted of player's presence
			//Transition:
			//chase (time passed)
			//stunned (stun timer > 0)
			//death (lost health)
			case (action::alert):
			{
				stunMultiplier = 1.0f;
				enemySprite.speed = 0.0f;
				//Decrease action time remaining.
				//If less than 0, begin action
				if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
					actionTimeRemaining -= dt;

				if (actionTimeRemaining < 0.0f)
				{
					currentAction = action::chase;
					actionTimeRemaining = flameBurstDelay;
					break;
				}

				//Face the player
				currentVectorDirection = playerTransform.pos - enemyTransform.pos;

				//Update facing direction
				updateFacingDirection();

				//If there's a change in the facing, reset animation
				if (currentFacingDirection != previousFacingDirection)
				{
					changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
				}
				break;
			}

			//Chase state - Get close enough to player. If too close, move back
			//Transition:
			//idle (too far)
			//prepare (time passed and close enough)
			//stunned (stun timer > 0)
			//death (lost health)
			case (action::chase):
			{
				stunMultiplier = 1.0f;
				enemySprite.speed = 8.0f;

				//Countdown action timer
				if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
					actionTimeRemaining -= dt;

				if (actionTimeRemaining <= 0.0f)
				{
					//Check if close enough, then prepare to fire
					if (distance < 300.0f)
					{
						audio.playPannedSoundEffect(audio.sfxTomatoBossGrowl, &audio.chSFX, 0.0f, 1.0f, 5.0f);
						currentAction = action::prepare;
						actionTimeRemaining = 1.0f;
						break;
					}
				}
			
				//If far enough from player, transition to idle
				if (distance > 1200.0f)
				{
					currentAction = action::idle;
					break;
				}

				//Movement
				currentVectorDirection = playerTransform.pos - enemyTransform.pos;
				Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(currentVectorDirection) * 10000.0f, dt);

				//If too close to the player, move back
				if (distance < 250.0f)
				{
					movementForce = Force(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(-currentVectorDirection) * 10000.0f, dt);
					enemySprite.speed = -8.0f;
				}

				//Zero out movement for if going faster than speed limit
				if (Mathf::Length(enemyRB.vel) > normalMovementSpeed)
					movementForce.setLinearForce(Mathf::vec2{});

				//Also zero out movement if in medium distance
				if (distance >= 250.0f && distance <= 290.0f)
					movementForce.setLinearForce(Mathf::Vector2{});

				enemyRB.forceManager.AddLinearForce("movement", movementForce);
				enemyRB.forceManager.setMaxVel(10000.0f);
				enemyRB.forceManager.setLinearDrag(0.25f);

				//Update facing direction
				updateFacingDirection();

				//If there is a change in the facing direction, reset animation
				if (currentFacingDirection != previousFacingDirection)
				{
					changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
				}

				break;
			}

			//Prepare state - Preparing to shoot a burst of flames
			//Transition:
			//fire (time passed)
			//stunned (stun timer > 0)
			//death (lost health)
			case (action::prepare):
			{
				//Alternate flashing
				if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				{
					flashTimer -= dt;
					if (flashTimer <= 0.0f)
					{
						flashOn = !flashOn;
						flashTimer += flashInterval;
					}
				}

				//Change colouration based off flashing
				if (ecs.entityHas<material>(id))
				{
					if (flashOn)
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 5.0f, 5.0f, 5.0f, 1.0f };
					}
					else
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
					}
				}

				stunMultiplier = 1.0f;
				enemySprite.speed = 0.0f;

				//Countdown action time
				if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
					actionTimeRemaining -= dt;

				if (actionTimeRemaining < 0.0f)
				{
					//Go to fire when timer is up
					currentAction = action::fire;
					flashOn = false;
					if (ecs.entityHas<material>(id))
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
					actionTimeRemaining = flameThrowerInterval;
					flameShotsRemaining = maxFlameShots;
					break;
				}

				break;
			}

			//Fire state - Firing a burst of flames
			//Is vulnerable in this state
			//Transition:
			//idle (out of shots)
			//stunned (stun timer > 0)
			//death (lost health)
			case (action::fire):
			{
				stunMultiplier = 5.0f;
				enemySprite.speed = 0.0f;

				//Count down to shoot another flame bolt
				if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
					actionTimeRemaining -= dt;

				if (actionTimeRemaining <= 0.0f)
				{
					//Shoot
					entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");

					Vector2 randomShotDirection = Rotate2D(currentVectorDirection, 0.5f * ((float)(rand()) / (float)(RAND_MAX)-0.5f));
					ecs.getComponent<transform2D>(newID).pos.x = enemyTransform.pos.x + 20.0f * Normalize(randomShotDirection).x;
					ecs.getComponent<transform2D>(newID).pos.y = enemyTransform.pos.y + 20.0f * Normalize(randomShotDirection).y;

					rigidBody2D eRigidBody;
					ecs.getComponent<transform2D>(newID).rotation = 360.f + 90.f - getVectorRotationRadian(randomShotDirection) * 180.f / 3.14f;
					Force fireBallForce(Force::ForceType::LINEAR_FORCE, 5000.0f * Normalize(randomShotDirection), 100.f);
					eRigidBody.forceManager.AddLinearForce("fireBallForce", fireBallForce, true);//adding linear force to the water droplet
					eRigidBody.forceManager.setMaxVel(500.f);

					ecs.addComponent<rigidBody2D>(newID, eRigidBody);//adding the rigidbody forces to the entity
					Com_TimeDestructor eTimeDestructor;
					eTimeDestructor.timeDelayDestructor = 5.f;//5 seconds self destruct
					ecs.addComponent<Com_TimeDestructor>(newID, eTimeDestructor);//adding the time destructor component to the entity

					Com_type eType;
					eType.type = Com_type::type::enemyBullet;//5 seconds self destruct
					ecs.addComponent<Com_type>(newID, eType);//adding the time destructor component to the entity

					AABB eAABB;
					ecs.addComponent<AABB>(newID, eAABB);//adding the time destructor component to the entity

					scriptComponent eScript;
					eScript.scriptContainer["ChiliShotScript"] = ScriptManager.createFromScriptDatabase("ChiliShotScript");
					ecs.addComponent<scriptComponent>(newID, eScript);

					--flameShotsRemaining;
					if (flameShotsRemaining > 0)
					{
						actionTimeRemaining = flameThrowerInterval;
					}
					else
					{
						//Transition out
						currentAction = action::idle;
						break;
					}
				}

				break;
			}
			//Stunned state - Damaged and trying to recover
			//Transition:
			//idle (time pased)
			//suction (inside suction radius)
			//death (lost health)
			case (action::stunned):
			{
				stunMultiplier = 1.0f;
				enemySprite.speed = 12.0f;
				//Check timing of stun. If stun time is over, transition into idle
				if (stunTimeRemaining <= 0.0f)
				{
					currentAction = action::idle;
					changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
					break;
				}

				//If caught in suction radius, get sucked in
				if (beingSucked)
				{
					currentAction = action::suction;
					changeFacingAnimation(enemySprite, "SuckedRight", "SuckedLeft", "SuckedLeft", "SuckedRight");
					break;
				}

				break;
			}
			//Death state - Lost all health
			//Does not transition to any other state
			case (action::death):
			{
				stunMultiplier = 0.0f;
				enemySprite.speed = 12.0f;
				//Check if last frame of animation reached
				//Delete enemy if so
				//TODO check the 0.5
				if (enemySprite.currentFrame == enemySprite.currentAnimation.size() - 1 &&
					enemySprite.frameLifetimeRemaining < 0.5f)
				{
					state = scriptBase::ScriptState::END;
				}

				break;
			}
			//Suction state - Within suction vacuum
			//Transition:
			//idle (out of suction)
			//death (out of health)
			case (action::suction):
			{
				//Set suctionSuccessful flag
				suctionSuccessful = true;

				stunMultiplier = 1.0f;
				enemySprite.speed = 12.0f;
				//Out of suction
				if (!beingSucked)
				{
					suctionSuccessful = false;
					currentAction = action::idle;
					//Resize back to initial scale
					enemyTransform.scale = initialScale;
					break;
				}

				//Shrink
				float maximumShrinkDistance = 250.0f;
				if (distance < maximumShrinkDistance)
				{
					enemyTransform.scale.x = initialScale.x * (distance / maximumShrinkDistance);
					enemyTransform.scale.y = initialScale.y * (distance / maximumShrinkDistance);
				}
				else
					enemyTransform.scale = initialScale;
				break;
			}
			}
		}
		beingSucked = false; //Reset at every frame.
	}
}

void ChilliCrackler::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> ChilliCrackler::createCopy()
{
	return std::make_shared<ChilliCrackler>();
}

std::string ChilliCrackler::getScriptName()
{
	return name;
}