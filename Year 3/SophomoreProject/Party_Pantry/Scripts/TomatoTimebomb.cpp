// /******************************************************************************/
// /*!
// \file         TomatoTimebomb.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Mar, 2022
// \brief        This file contains the definitions for tomato timebomb enemy's
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

#include "TomatoTimebomb.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"

#include "PlayerController.h"

//ctor
TomatoTimebomb::TomatoTimebomb(const char* scriptName) :
	EnemyBehaviour(scriptName, 200.0f, 1.0f)
{

}

void TomatoTimebomb::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
	state = scriptBase::ScriptState::UPDATE;
}

void TomatoTimebomb::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	previousFacingDirection = currentFacingDirection; //Update previous direction

	//Reduce stunned time remaining
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
		Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);

		//Distance between player and enemy
		float distance = Mathf::Length(enemyTransform.pos - playerTransform.pos);

		//STATE HANDLING///////////////////////////////////////////////////////

		//Transition into exploding state
		if (currentAction != action::explode &&
			enemyHealth.health <= 0)
		{
			currentAction = action::explode;
			audio.playPannedSoundEffect(audio.sfxExplosion, &audio.chSFX, 0.0f, 0.5f, 2.0f);

			//TODO explosion particles
			
			//Damage player if he is close enough
			//TODO may damage nearby enemies too
			if (distance < 180.0f)
			{
				//Only Shield denies damage
				//Check for damage denial
				bool damageDenied = false;
				if (ecs.entityHas<scriptComponent>(playerEntity))
				{
					auto& sc{ ecs.getComponent<scriptComponent>(playerEntity).scriptContainer };
					auto scriptItr{ sc.find("PlayerController") };
					if (scriptItr != sc.end())
					{
						PlayerController* player = reinterpret_cast<PlayerController*>((*scriptItr).second.get());
						if (player->currentSkill == PlayerController::skills::shield)
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
					playerHealth.health -= 12;
				}
			}

			//Set animation based off facing
			changeFacingAnimation(enemySprite, "DeathUp", "DeathDown", "DeathLeft", "DeathRight");
		}

		//Transition into Stun state if not dying and stun timer > 0
		//Transitionable from any state BUT death
		if (currentAction != action::explode &&
			currentAction != action::stunned &&
			currentAction != action::suction &&
			stunTimeRemaining > 0.0f)
		{
			currentAction = action::stunned;

			//Set animation based off facing
			changeFacingAnimation(enemySprite, "StunnedBack", "StunnedForward", "StunnedLeft", "StunnedRight");
		}

		//List of states
		//idle    - waiting for the player to be close enough
		//alert   - made aware of the player, preparing to chase
		//chase   - chasing the player
		//stunned - stunned as a result of getting hit
		//fuse    - preparing to explode and damage the player
		//explode - exploding, prepare to delete as animation finishes
		//suction - being sucked in
		switch (currentAction)
		{
			//Idle state - Waiting for player to be close enough
			//Transitions into:
			//Alert (near enough)
			//Stunned (stun timer > 0)
			//Explode (HP <= 0)
		case (action::idle):
		{
			//Alternate colouration
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = colouration;

			stunMultiplier = 1.0f;
			enemySprite.speed = 0.0f;
			//Wait for distance to be near enough to start alert
			if (distance < 800.0f)
			{
				currentAction = action::alert;
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
				//TODO play noticing SFX & have a '!' particle
				actionTimeRemaining = 1.0f;
				break;
			}
			break;
		}

		//Alert state - Made aware of player, prepare to chase
		//Can transition to:
		//Chase (time passed)
		//Stunned (stun timer > 0)
		//Explode (HP <= 0)
		case (action::alert):
		{
			//Alternate colouration
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = colouration;

			stunMultiplier = 1.0f;
			enemySprite.speed = 0.0f;
			//Decrease action time remaining
			//If it is less than 0, begin acting
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				currentAction = action::chase;
			}

			//Face the player
			currentVectorDirection = playerTransform.pos - enemyTransform.pos;

			//Update facing direction
			updateFacingDirection();

			//If there is a change in the facing direction, reset animation
			if (currentFacingDirection != previousFacingDirection)
			{
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
			}

			break;
		}

		//Chase state - Follow player until near enough
		//Transition
		//Idle (too far)
		//Fuse (near enough, prepare)
		//Stunned (stun timer > 0)
		//Explode (HP <= 0)
		case (action::chase):
		{
			//Alternate colouration
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = colouration;

			stunMultiplier = 1.0f;
			enemySprite.speed = 18.0f;
			//If far enough from player, transition to idle
			if (distance > 1200.0f)
			{
				currentAction = action::idle;
				break;
			}
			//If close enough to player, transition to prepare to attack
			if (distance < 100.0f)
			{
				currentAction = action::fuse;
				actionTimeRemaining = 2.0f; //Prepare for this long
				//TODO play fuse SFX
				break;
			}

			//Follow the player otherwise
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
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
			}

			break;
		}

		//Fuse state - Prepare to explode! Cannot be stunned, but can be pushed away
		//Transition
		//Explode (Time passed or HP <= 0)
		case (action::fuse):
		{
			//Alternate flashing
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
			{
				flashTimer -= dt;
				if (flashTimer <= 0.0f)
				{
					audio.playPannedSoundEffect(audio.sfxClick1);
					flashOn = !flashOn;
					flashTimer += actionTimeRemaining * 0.1f;
				}
			}

			//Change colouration based off flashing
			if (ecs.entityHas<material>(id))
			{
				if (flashOn)
				{
					ecs.getComponent<material>(id).color = Mathf::Vector4{ 2.0f, 10.0f, 5.0f, 1.0f };
				}
				else
				{
					ecs.getComponent<material>(id).color = colouration;
				}
			}
			
			//Stuck on the fourth frame.
			enemySprite.currentFrame = 3;
			//Sanity check for frame data
			if (!enemySprite.frameData.empty() && enemySprite.currentFrame < enemySprite.currentAnimation.size())
				enemySprite.currentTexCoords = enemySprite.frameData[enemySprite.currentAnimation[enemySprite.currentFrame]];

			stunMultiplier = 0.0f;
			enemySprite.speed = 0.0f;

			//Countdown action time
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				//Deplete all enemy health so that they explode
				enemyHealth.health = 0;
				break;
			}
			break;
		}

		//Exploding state
		//Cannot transition into any other state
		case (action::explode):
		{
			//Change colouration to make them look like they are exploding
			if (ecs.entityHas<material>(id))
			{
				//TODO fiddle around the colouration
				ecs.getComponent<material>(id).color = Mathf::Vector4{ 2.0f, 1.0f, 1.0f, 1.0f };
			}

			stunMultiplier = 0.0f;
			enemySprite.speed = 12.0f;

			//Check if last frame of animation reached
			//Delete enemy if so
			if (enemySprite.currentFrame == enemySprite.currentAnimation.size() - 1 &&
				enemySprite.frameLifetimeRemaining < 0.5f)
			{
				state = scriptBase::ScriptState::END;
			}
			break;
		}

		//Stunned state - Got hit and is now unable to act
		//Transitions to
		//idle (Stun time over)
		//suction (inside the suction radius)
		//explode (lost health)
		case (action::stunned):
		{
			//Alternate colouration
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = colouration;

			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;
			//Check timing of stun. If stun time is over, transition into idle
			if (stunTimeRemaining <= 0.0f)
			{
				currentAction = action::idle;
				break;
			}

			//If caught in suction radius, get sucked in
			if (beingSucked)
			{
				currentAction = action::suction;
				changeFacingAnimation(enemySprite, "DraggedRight", "DraggedLeft", "DraggedLeft", "DraggedRight");
				break;
			}

			break;
		}

		//Suction state - getting sucked in by player's vacuum
		//Transitions to
		//Idle (out of suction)
		//Explode (out of health)
		case (action::suction):
		{
			//Alternate colouration
			if (ecs.entityHas<material>(id))
				ecs.getComponent<material>(id).color = colouration;

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
				//Switch animations
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
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

void TomatoTimebomb::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> TomatoTimebomb::createCopy()
{
	return std::make_shared<TomatoTimebomb>();
}

std::string TomatoTimebomb::getScriptName()
{
	return name;
}