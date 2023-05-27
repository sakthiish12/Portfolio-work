// /******************************************************************************/
// /*!
// \file         MushroomMole.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Mar, 2022
// \brief        This file contains the definitions for mushroom mole enemy's
//               logic
// \lines        ??? Lines
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

#include "MushroomMole.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"

#include "PlayerController.h"
#include <iostream>
#include <random>
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterDig.h"


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

//Ctor
MushroomMole::MushroomMole(const char* scriptName) :
	EnemyBehaviour(scriptName, 100.0f, 1.0f)
{

}

void MushroomMole::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "disguise", "disguise", "disguise", "disguise");
	state = scriptBase::ScriptState::UPDATE;

	//Create emitter
	EmitterComponent EC{};
	EC.emitterContainer.insert(std::make_pair("EmitterDig", std::make_shared<EmitterDig>()));
	ecs.addComponent<EmitterComponent>(id, EC);

	spriteCom& enemySprite = ecs.getComponent<spriteCom>(id);
	enemySprite.setAnimation("walkUp");
}

void MushroomMole::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	//Update previous direction tracker
	previousFacingDirection = currentFacingDirection;

	//Reduce stunned time remaining
	if (stunTimeRemaining >= 0.0f && UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
		stunTimeRemaining -= dt;
	if (stunTimeRemaining < 0.0f)
		stunTimeRemaining = 0.0f;

	//To check if the player if found
	entityID playerEntity{ 0 };
	bool playerFound{ false };

	//Attempt to get the player entity
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
		//ecs.entityHas<AABB>(id) && LEAP OF FAITH HERE
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
		Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);

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
			changeFacingAnimation(enemySprite, "deathBack", "deathFront", "deathFront", "deathFront");
		}

		//Transition into Stun state if not dying and stun timer > 0
		//Transitionable from any state BUT death
		if (currentAction != action::death &&
			currentAction != action::stunned &&
			currentAction != action::suction &&
			stunTimeRemaining > 0.0f)
		{
			if (currentAction == action::digin)
				audio.playPannedSoundEffect(audio.sfxDing);

			currentAction = action::stunned;

			//Set animation based off facing
			changeFacingAnimation(enemySprite, "stunnedBack", "stunnedFront", "stunnedSide", "stunnedSide");
			//There actually is no stunnedLeft animation. Flip horizontally the stunnedRight animation
			if (currentFacingDirection == facing::left)
			{
				enemyTransform.scale.x = -initialScale.x; //HACK Easy flipping
			}
			else
			{
				enemyTransform.scale.x = initialScale.x;
			}
		}

		switch (currentAction)
		{

		//Idle state - Remain disguised and stay in place. Can be damaged
		//Can transition to:
		//Alert (Very near player)
		//Stunned (Stun timer > 0)
		//Death (out of health)
		case (action::idle):
		{
			stunMultiplier = 0.5f;
			enemySprite.speed = 12.0f;

			//Near enough, reveal out of disguise
			if (distance < 500.0f)
			{
				changeFacingAnimation(enemySprite, "reveal", "reveal", "reveal", "reveal");
				currentAction = action::alert;
			}
			break;
		}

		//Alert state - Take the time to reveal self to the player.
		//Can transition to:
		//Active (Animation finished)
		//Stunned (Stun timer > 0)
		//Death (out of health)
		case (action::alert):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;

			//Check if the last frame of animation reached
			//Go to the next state if so
			if (enemySprite.currentFrame == enemySprite.currentAnimation.size() - 1 &&
				enemySprite.frameLifetimeRemaining < 0.2f)
			{
				changeFacingAnimation(enemySprite, "walkUp", "walkDown", "walkLeft", "walkRight");
				currentAction = action::active;
			}
			break;
		}

		//Active state - Chasing the player. Can damage the player like this.
		//Can transition to:
		//Digin (Far enough)
		//Stunned (Stun timer > 0)
		//Death (Out of health)
		case (action::active):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;

			//Do not act when damaged player
			if (currentDamageDelay > 0.0f)
			{
				currentDamageDelay -= dt;
			}
			else
			{
				//If far enough, transition to dig
				if (distance > 600.0f)
				{
					audio.playPannedSoundEffect(audio.sfxMushroomEmerge);

					changeFacingAnimation(enemySprite, "suckedRight", "suckedRight", "suckedRight", "suckedRight");
					enemyTransform.rotation = -90.0f; //Make them look like they're digging downwards
					actionTimeRemaining = 1.0f;
					currentAction = action::digin;
					break;
				}

				//Follow the player and try to hurt him otherwise

				currentVectorDirection = playerTransform.pos - enemyTransform.pos;
				Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(currentVectorDirection) * 10000.0f, dt);

				//Zero out movement force if going faster than speed limit
				if (Mathf::Length(enemyRB.vel) > normalMovementSpeed)
					movementForce.setLinearForce(Mathf::vec2{});

				enemyRB.forceManager.AddLinearForce("movement", movementForce);
				enemyRB.forceManager.setMaxVel(10000.0f);
				enemyRB.forceManager.setLinearDrag(0.25f);

				//Update facing direction
				updateFacingDirection();

				//If there is a change in the facing direction, reset animation
				if (currentFacingDirection != previousFacingDirection)
				{
					changeFacingAnimation(enemySprite, "walkUp", "walkDown", "walkLeft", "walkRight");
				}

				//If collided with player, damage player
				std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
				if (!collidedEntities.empty())
				{
					for (entityID e : collidedEntities)
					{
						if (e == playerEntity)
						{
							//Shield denies damage
							//Player charging ahead at 4 pressure (ultra speed) also denies damage
							//Check for damage denial
							bool damageDenied = false;
							if (ecs.entityHas<scriptComponent>(e))
							{
								auto& sc{ ecs.getComponent<scriptComponent>(e).scriptContainer };
								auto scriptItr{ sc.find("PlayerController") };
								if (scriptItr != sc.end())
								{
									PlayerController* player = reinterpret_cast<PlayerController*>((*scriptItr).second.get());
									if (player->currentSkill == PlayerController::skills::shield ||
										(player->currentSkill == PlayerController::skills::speed) && player->pressureCurrent == 4)
										damageDenied = true;
								}
							}

							//(16 Mar) Deny damage if paused
							if (UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Play)
								damageDenied = true;

							//This is what happens when Kettle Knight does not have shields on
							if (!damageDenied)
							{
								//Push player away
								if (ecs.entityHas<rigidBody2D>(playerEntity))
								{
									rigidBody2D& playerRB = ecs.getComponent<rigidBody2D>(playerEntity);
									playerRB.vel += Mathf::Normalize(Mathf::Vec2(playerTransform.pos.x - enemyTransform.pos.x, playerTransform.pos.y - enemyTransform.pos.y)) * 1000.0f;
								}
								//Hurt and play sound effects
								playerHealth.health -= 6;
								//SoundClass::getInstance()->PlayAgain(bitten);
								audio.playPannedSoundEffect(audio.sfxEnemyBite);
							}

							currentDamageDelay = setDamageDelay;
						}
					}
				}
			}

			break;
		}

		//Digging in state - Digging into the ground. Extremely vulnerable in this state, cannot move.
		//Can transition to:
		//Burrow (Time passed)
		//Stunned (Stun timer > 0)
		//Death (Out of health)
		case (action::digin):
		{
			//TODO keep generating particles
			emitterPos = enemyTransform.pos;
			emitterPos.z = 1.5;
			emitterPos.x = randomnumgenfloat(enemyTransform.pos.x - emitterRangeX, enemyTransform.pos.x + emitterRangeX);
			emitterPos.y = randomnumgenfloat(enemyTransform.pos.y, enemyTransform.pos.y + emitterRangeY);
			if (ecs.entityHas<EmitterComponent>(id))
			{
				EmitterDig* emi = dynamic_cast<EmitterDig*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterDig"].get());
				if (emi)
					emi->setTriggered(true, emitterVelocity, emitterPos);
			}

			//if (ecs.doesEntityHaveChildren(id)) {}

			stunMultiplier = 5.0f;
			enemySprite.speed = 24.0f;

			//Decrease action time remaining
			//If less than 0, go burrow
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				//TODO hide shadow
				if (ecs.doesEntityHaveChildren(id, kids)) {
					for (entityID& i : kids) {
						if (ecs.entityHas<spriteCom>(i))
							ecs.getComponent<spriteCom>(i).isVisible = false;
					}
				}
				currentAction = action::burrow;
				enemyTransform.rotation = 0.0f;//Reset rotation
				//Deactivate AABB
				ecs.removeComponent<AABB>(id);
				//Deactivate sprite
				ecs.getComponent<spriteCom>(id).isVisible = false;
				break;
			}

			break;
		}

		//Burrowing state - Digging through the ground. Cannot be touched. Moves three times as fast
		//Can transition to:
		//Digout (Very near the player)
		case (action::burrow):
		{
			stunMultiplier = 0.0f;
			enemySprite.speed = 0.0f;

			//Move towards the player quickly
			currentVectorDirection = playerTransform.pos - enemyTransform.pos;
			Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(currentVectorDirection) * 10000.0f, dt);

			//Zero out movement force if going faster than speed limit
			//Charging at triple speed
			if (Mathf::Length(enemyRB.vel) > normalMovementSpeed * 3.0f)
				movementForce.setLinearForce(Mathf::vec2{});

			enemyRB.forceManager.AddLinearForce("movement", movementForce);
			enemyRB.forceManager.setMaxVel(10000.0f);
			enemyRB.forceManager.setLinearDrag(0.05f);

			//If very near to the player, go to digout state
			if (distance < 50.0f)
			{
				currentAction = action::digout;
				actionTimeRemaining = 1.0f;
				break;
			}

			//Generate Particles
			if (ecs.entityHas<EmitterComponent>(id))
			{
				EmitterDig* emi = dynamic_cast<EmitterDig*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterDig"].get());
				if (emi)
					emi->setTriggered(true, enemyRB.vel, enemyTransform.pos);
			}

			break;
		}

		//Digging out state - Digging out of the ground. Cannot be touched, cannot move
		//Can transition to:
		//Active (Time passed)
		case (action::digout):
		{
			stunMultiplier = 0.0f;
			enemySprite.speed = 0.0f;

			//Decrease action time remaining
			//If less than 0, get active again
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				//Change to walking animation
				changeFacingAnimation(enemySprite, "walkUp", "walkDown", "walkLeft", "walkRight");
				audio.playPannedSoundEffect(audio.sfxMushroomEmerge);
				currentAction = action::active;
				//Reactivate AABB
				AABB newAABB{};
				newAABB.scale.x = 0.25f;
				newAABB.scale.y = 0.25f;
				ecs.addComponent<AABB>(id, newAABB);
				//Reactive sprite
				ecs.getComponent<spriteCom>(id).isVisible = true;

				if (ecs.doesEntityHaveChildren(id, kids)) {
					for (entityID& i : kids) {
						if (ecs.entityHas<spriteCom>(i))
							ecs.getComponent<spriteCom>(i).isVisible = true;
					}
				}

				//TODO emit dirt explosion to show that he digged out
				break;
			}
			
			//Generate Particles
			if (ecs.entityHas<EmitterComponent>(id))
			{
				EmitterDig* emi = dynamic_cast<EmitterDig*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterDig"].get());
				if (emi)
					emi->setTriggered(true, enemyRB.vel, enemyTransform.pos);
			}

			break;
		}

		//Suction state - Being sucked in by the player's vacuum
		//Can transition to:
		//Alert (Out of suction)
		//Death (Out of health)
		case (action::suction):
		{
			//Set flag
			suctionSuccessful = true;
			
			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;
			
			//Out of suction
			if (!beingSucked)
			{
				suctionSuccessful = false;
				changeFacingAnimation(enemySprite, "walkUp", "walkDown", "walkLeft", "walkRight");
				currentAction = action::alert;
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

		//Stunned state - Stunned by attacks
		//Transitions into:
		//Suction (In suction range)
		//Digin (After getting stunned, and close enough)
		//Idle (After getting stunned, but too far)
		//Death
		case (action::stunned):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;
			//Check remaining time of stun
			if (stunTimeRemaining <= 0.0f)
			{
				//Unflip
				enemyTransform.scale.x = initialScale.x;

				//Too far, go idle
				if (distance > 1200.0f)
				{
					changeFacingAnimation(enemySprite, "disguise", "disguise", "disguise", "disguise");
					currentAction = action::idle;
					break;
				}
				else
				{
					changeFacingAnimation(enemySprite, "suckedRight", "suckedRight", "suckedRight", "suckedRight");
					enemyTransform.rotation = -90.0f; //Make them look like they're digging downwards
					actionTimeRemaining = 1.0f;
					currentAction = action::digin;
					break;
				}
			}

			//If caught in suction radius, get sucked in
			if (beingSucked)
			{
				currentAction = action::suction;
				changeFacingAnimation(enemySprite, "suckedRight", "suckedLeft", "suckedLeft", "suckedRight");
			}
			break;
		}

		//Death state - Getting destroyed. Breaks out of update loop
		//Cannot transition to another state
		case (action::death):
		{
			stunMultiplier = 1.0f;
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

		}
	}
		beingSucked = false; //Reset at every frame.
}

void MushroomMole::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> MushroomMole::createCopy()
{
	return std::make_shared<MushroomMole>();
}

std::string MushroomMole::getScriptName()
{
	return name;
}