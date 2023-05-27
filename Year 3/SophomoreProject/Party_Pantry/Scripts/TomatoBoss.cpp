// /******************************************************************************/
// /*!
// \file         TomatoBoss.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Mar, 2022
// \brief        This file contains the definitions for tomato boss enemy's
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

#include "TomatoBoss.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"

#include "PlayerController.h"

#include <Sound/SoundManagerV2.h>

//ctor
TomatoBoss::TomatoBoss(const char* scriptName) :
	EnemyBehaviour(scriptName, 50.0f, 1.0f)
{

}

void TomatoBoss::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
	state = scriptBase::ScriptState::UPDATE;
}

void TomatoBoss::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	//Damage countdowns
	damageCooldownRemaining -= dt;

	previousFacingDirection = currentFacingDirection; //Update previous direction

	//Reduce stunned time remaining
	//Flash red if there is stun time remaining
	if (stunTimeRemaining >= 0.0f && UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
	{
		redFlashTimer -= dt;
		if (redFlashTimer < 0.0f)
		{
			redFlashOn = !redFlashOn;
			redFlashTimer += redFlashInterval;
		}

		if (ecs.entityHas<material>(id))
		{
			if (redFlashOn)
				ecs.getComponent<material>(id).color = Mathf::Vector4(3.0f, 3.0f, 3.0f, 1.0f);
			else
				ecs.getComponent<material>(id).color = Mathf::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		stunTimeRemaining -= dt;
	}
		
	if (stunTimeRemaining < 0.0f)
	{
		redFlashOn = false;
		redFlashTimer = 0.0f;
		if (ecs.entityHas<material>(id))
			ecs.getComponent<material>(id).color = Mathf::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		stunTimeRemaining = 0.0f;
	}
		

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

		//std::cout << enemyHealth.health << std::endl;
		//Transition into fuse state upon health being depleted
		if (currentAction != action::fuse &&
			currentAction != action::explode &&
			enemyHealth.health <= 1)
		{
			//Reset flipping
			enemyTransform.scale.x = initialScale.x;

			currentAction = action::fuse;
			//std::cout << "TOMATO BOSS FUSE\n";

			actionTimeRemaining = 3.0f; //FUSE TIMER OF THREE SECONDS
		}

		switch (currentAction)
		{
			//Idle State - waiting for player to come close enough
			//Can transition to:
			//Active (near enough)
			//Fuse (health depleted)
		case (action::idle):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 0.0f;

			//Near enough, start moving
			if (distance < 600.0f)
			{
				currentAction = action::active;
				//std::cout << "TOMATO BOSS CHASE\n";
				actionTimeRemaining = 1.0f + 0.5f * enemyHealth.health; //Throw bombs after 1 + 0.5 * health seconds of moving
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
				break;
			}
			break;
		}

		//Active state - Chase player 
		//Can transition to:
		//Throwing (time passed)
		//Fuse (health depleted)
		case (action::active):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;

			//Decrease action time remaining
			//If it is less than 0, begin throwing
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				audio.playPannedSoundEffect(audio.voTomotoAngry);
				currentAction = action::throwing;
				//std::cout << "TOMATO BOSS PLANTING\n";
				actionTimeRemaining = 1.0f; //1 second to throw

				//Set animation based off facing
				changeFacingAnimation(enemySprite, "AttackSide", "AttackSide", "AttackSide", "AttackSide");
				//There actually is no stunnedLeft animation. Flip horizontally the stunnedRight animation
				if (currentFacingDirection == facing::left)
				{
					enemyTransform.scale.x = -initialScale.x; //HACK Easy flipping
				}
				else
				{
					enemyTransform.scale.x = initialScale.x;
				}
				break;
			}

			//Follow the player
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

						if (damageCooldownRemaining > 0.0f)
							damageDenied = true;

						//This is what happens when Kettle Knight does not have shields on
						if (!damageDenied)
						{
							//Push player away
							if (ecs.entityHas<rigidBody2D>(playerEntity))
							{
								rigidBody2D& playerRB = ecs.getComponent<rigidBody2D>(playerEntity);
								playerRB.vel += Mathf::Normalize(Mathf::Vec2(playerTransform.pos.x - enemyTransform.pos.x, playerTransform.pos.y - enemyTransform.pos.y)) * 2000.0f;
							}
							//Hurt and play sound effects
							playerHealth.health -= 15;
							damageCooldownRemaining = damageInterval;
							//SoundClass::getInstance()->PlayAgain(bitten);
							audio.playPannedSoundEffect(audio.sfxEnemyBite, &audio.chSFX, 0.0f, 1.0f, 0.7f);
						}
					}
				}
			}

			break;
		}

		//Throwing state - Prepare to throw bomb enemies
		//Transitions to:
		//active (time passed)
		//fuse (health depleted)
		case (action::throwing):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;

			//Decrease action time remaining
			//If less than 0, throw a bomb tomato and go back to active state
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				//Reset flipping
				enemyTransform.scale.x = initialScale.x;

				currentAction = action::active;
				//std::cout << "TOMATO BOSS CHASE\n";
				actionTimeRemaining = 1.0f + 0.5f * enemyHealth.health; //Throw bombs after 1 + 0.5 * health seconds of moving
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");

				audio.playPannedSoundEffect(audio.sfxGunClick);

				//Throw a bomb enemy
				entityID newBomb = ecs.createEntityFromFile("Resources/SavedEntities/TimebombTomato.json");
				if (ecs.entityHas<transform2D>(newBomb)) {
					ecs.getComponent<transform2D>(newBomb).pos = enemyTransform.pos + Mathf::vec3(Mathf::Normalize(currentVectorDirection).x, Mathf::Normalize(currentVectorDirection).y, 0.0f) * 100.0f;
					ecs.getComponent<transform2D>(newBomb).pos.z = 1.2f;
				}

				ecs.renameEntity(newBomb, "TomatoNew");

				//A shadow appears for the bomb
				entityID shadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(shadow, newBomb);

				break;
			}

			break;
		}

		//Fuse state - Preparing to explode!
		//Transitions to
		//exploding (fuse timer over)
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
					ecs.getComponent<material>(id).color = Mathf::Vector4{ 3.0f, 3.0f, 3.0f, 1.0f };
				}
				else
				{
					ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
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
				currentAction = action::explode;
				//std::cout << "TOMATO BOSS EXPLODE\n";

				audio.playPannedSoundEffect(audio.sfxExplosion);

				//Break into four exploding enemies
				entityID north = ecs.createEntityFromFile("Resources/SavedEntities/TimebombTomato.json");
				if (ecs.entityHas<transform2D>(north))
					ecs.getComponent<transform2D>(north).pos = enemyTransform.pos + Mathf::vec3(0.0f, 100.0f, 0.0f);
				if (ecs.entityHas<rigidBody2D>(north))
					ecs.getComponent<rigidBody2D>(north).vel.y = 500.0f;
				
				ecs.renameEntity(north, "TomatoNew");
				
				entityID northShadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(northShadow, north);

				entityID south = ecs.createEntityFromFile("Resources/SavedEntities/TimebombTomato.json");
				if (ecs.entityHas<transform2D>(south))
					ecs.getComponent<transform2D>(south).pos = enemyTransform.pos + Mathf::vec3(0.0f, -100.0f, 0.0f);
				if (ecs.entityHas<rigidBody2D>(south))
					ecs.getComponent<rigidBody2D>(south).vel.y = -500.0f;

				ecs.renameEntity(south, "TomatoNew");

				entityID southShadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(southShadow, south);

				entityID east = ecs.createEntityFromFile("Resources/SavedEntities/TimebombTomato.json");
				if (ecs.entityHas<transform2D>(east))
					ecs.getComponent<transform2D>(east).pos = enemyTransform.pos + Mathf::vec3(100.0f, 0.0f, 0.0f);
				if (ecs.entityHas<rigidBody2D>(east))
					ecs.getComponent<rigidBody2D>(east).vel.x = 500.0f;

				ecs.renameEntity(east, "TomatoNew");

				entityID eastShadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(eastShadow, east);

				entityID west = ecs.createEntityFromFile("Resources/SavedEntities/TimebombTomato.json");
				if (ecs.entityHas<transform2D>(west))
					ecs.getComponent<transform2D>(west).pos = enemyTransform.pos + Mathf::vec3(-100.0f, 0.0f, 0.0f);
				if (ecs.entityHas<rigidBody2D>(west))
					ecs.getComponent<rigidBody2D>(west).vel.x = -500.0f;

				ecs.renameEntity(west, "TomatoNew");

				entityID westShadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(westShadow, west);

				//TODO play explosion sound effect

			//TODO explosion particles

			//Damage player if he is close enough
			//TODO may damage nearby enemies too
				if (distance < 240.0f)
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
						playerHealth.health -= 18;
					}
				}

				//Set animation based off facing
				changeFacingAnimation(enemySprite, "DeathUp", "DeathDown", "DeathLeft", "DeathRight");
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
		}
	}
}

void TomatoBoss::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> TomatoBoss::createCopy()
{
	return std::make_shared<TomatoBoss>();
}

std::string TomatoBoss::getScriptName()
{
	return name;
}