// /******************************************************************************/
// /*!
// \file         MushroomBoss.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         13th Mar, 2022
// \brief        This file contains the definitions for mushroom boss logic
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
#include "../../Engine/AssetManager/UpgradeManager.h"
#include "MushroomBoss.h"
#include "MushroomMole.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"
#include "Level2/MushroomDynamicCameraScript.h"
#include "PlayerController.h"

#include "../../Engine/ECS/Systems/ParticleSystem/EmitterWalkingTrail.h"
#include <Sound/SoundManagerV2.h>


//Ctor
MushroomBoss::MushroomBoss(const char* scriptName) :
	EnemyBehaviour(scriptName, 50.0f, 1.0f)
{

}

void MushroomBoss::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "IdleBack", "IdleFront", "IdleFront", "IdleFront");
	state = scriptBase::ScriptState::UPDATE;

	if (ecs.entityHas<Com_Health>(id))
		lastFrameHealth = ecs.getComponent<Com_Health>(id).health;
}

void MushroomBoss::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	damageCooldownRemaining -= dt;

	//Update previous direction traker
	previousFacingDirection = currentFacingDirection;

	//Reduce stunned time remaining (does not actually stun the boss)
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
			//Cannot flash while idle or shielded
			if (redFlashOn && (currentAction != action::idle && currentAction != action::shield))
			{
				ecs.getComponent<material>(id).color = Mathf::Vector4(3.0f, 3.0f, 3.0f, 1.0f);
			}
			else
			{
				ecs.getComponent<material>(id).color = Mathf::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		
		stunTimeRemaining -= dt;
	}
	if (stunTimeRemaining < 0.0f)
	{
		redFlashOn = false;
		redFlashTimer = 0.0f;
		if (ecs.entityHas<material>(id))
		{
			ecs.getComponent<material>(id).color = Mathf::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		stunTimeRemaining = 0.0f;
	}
		
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

		switch (currentAction)
		{
			//Idle state - waiting for player to come close enough, cannot be hurt
			//Can transition to:
			//reveal (near enough to player)
		case (action::idle):
		{
			stunMultiplier = 0.0f;
			enemySprite.speed = 6.0f;

			//Deny all incoming damage
			if (enemyHealth.health < lastFrameHealth)
				enemyHealth.health = lastFrameHealth;

			//Near enough, reveal
			if (distance < 600.0f)
			{
				//std::cout << "BOSS REVEAL\n";
				changeFacingAnimation(enemySprite, "Reveal", "Reveal", "Reveal", "Reveal");
				currentAction = action::reveal;
				audio.playPannedSoundEffect(audio.sfxMushroomBossEmerge);
			}
			break;
		}
			
			//Reveal state - Revealing itself before starting to move.
			//Transitions to:
			//walk (After animation)
			//stunned (some how health is low enough at this point)
		case (action::reveal):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;

			//Animation finished, go to next state
			if (enemySprite.currentFrame == enemySprite.currentAnimation.size() - 1 &&
				enemySprite.frameLifetimeRemaining < 0.2f)
			{
				//activating the dynamic camera for the mushroom 
				for (auto x : ecs.getEntityNames()) {
					if (ecs.entityHas<scriptComponent>(x.first)) {
						auto script = ecs.getComponent<scriptComponent>(x.first).scriptContainer.find("MushroomDynamicCameraScript");
						//check if it's within the map 
						if (script != ecs.getComponent<scriptComponent>(x.first).scriptContainer.end()) {
							MushroomDynamicCameraScript* camera = reinterpret_cast<MushroomDynamicCameraScript*>((*script).second.get());
							camera->isActivated = true;
						}
					}
				}
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
				actionTimeRemaining = 5.0f;
				currentAction = action::walk;
			}
			break;
		}

			//Walk state - Walking slowly towards the player
			//Can damage player upon colliding with him
			//Transition to:
			//summon (after some time has passed)
			//stunned (low health)
		case (action::walk):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;

			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			//Time has passed, go summon
			if (actionTimeRemaining < 0.0f)
			{
				//std::cout << "BOSS START SUMMONING\n";
				changeFacingAnimation(enemySprite, "SummonMushrooms", "SummonMushrooms", "SummonMushrooms", "SummonMushrooms");

				if (std::rand() < RAND_MAX / 2)
					audio.playPannedSoundEffect(audio.voMushroomTalkingVariation1);
				else
					audio.playPannedSoundEffect(audio.voMushroomTalkingVariation2);

				currentAction = action::summon;
				break;
			}

			//Walk to player
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
							playerHealth.health -= 12;
							damageCooldownRemaining = damageInterval;
							//SoundClass::getInstance()->PlayAgain(bitten);
							audio.playPannedSoundEffect(audio.sfxMushroomEmerge, &audio.chSFX, 0.0f, 2.0f, 0.5f);
						}
					}
				}
			}

			break;
		}

			//Summon state - Summoning a mushroom minion. Cannot move
			//Cannot damange the player
			//Transition to:
			//Shield (Animation finished)
			//stunned (low health)
		case (action::summon):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;

			//Animation finished, go to next state
			//AND summon a mushroom
			//AND show shield
			if (enemySprite.currentFrame == enemySprite.currentAnimation.size() - 1 &&
				enemySprite.frameLifetimeRemaining < 0.2f)
			{
				//std::cout << "BOSS SUMMON OK, START SHIELD\n";

				changeFacingAnimation(enemySprite, "SummonShield", "SummonShield", "SummonShield", "SummonShield");
				actionTimeRemaining = 5.0f;
				currentAction = action::shield;
				audio.playPannedSoundEffect(audio.sfxGunClick);

				//Summon a mushroom at the boss's position
				entityID newMushroom = ecs.createEntityFromFile("Resources/SavedEntities/MushroomMole.json");
				if (ecs.entityHas<transform2D>(newMushroom))
					ecs.getComponent<transform2D>(newMushroom).pos = enemyTransform.pos;

				//Let the mushroom immediately act
				if (ecs.entityHas<scriptComponent>(newMushroom))
				{
					auto& sc{ ecs.getComponent<scriptComponent>(newMushroom).scriptContainer };
					auto scriptItr{ sc.begin() };
					if (scriptItr != sc.end())
					{
						MushroomMole* mushroom = reinterpret_cast<MushroomMole*>((*scriptItr).second.get());
						mushroom->currentAction = MushroomMole::action::active;
					}
				}

				//A shadow appears for the mushroom
				entityID mushroomShadow = ecs.createEntityFromFile("Resources/SavedEntities/CharacterShadow.json");
				ecs.setEntityParent(mushroomShadow, newMushroom);

				//Rename the mushroom as such so that it registers as a mushroom when vacuuming in and gaining materials
				ecs.renameEntity(newMushroom, "MushroomMole");

				//std::cout << "New Mushroom " << newMushroom << ", shadow " << mushroomShadow << std::endl;

				//Load a shield
				shieldID = ecs.createEntityFromFile("Resources/PresetEntities/shield.json");
				ecs.setEntityParent(shieldID, id); //Shield tracks boss
				//std::cout << "Shield " << shieldID << std::endl;
				
				if (ecs.entityHas<spriteCom>(shieldID))	
					ecs.getComponent<spriteCom>(shieldID).isVisible = true;
				break;
			}

			break;
		}

			//Shield state - Shielding from incoming attacks. Cannot move
			//Denies all incoming damage
			//Can damage the player upon colliding with him
			//Transition to:
			//Walk (time passed)
		case (action::shield):
		{
			stunMultiplier = 0.0f;
			enemySprite.speed = 6.0f;

			//Decrease action time remaining
			//If less than 0, go walk again
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				//std::cout << "BOSS FINISH SHIELD\n";
				actionTimeRemaining = 5.0f;
				currentAction = action::walk;
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");

				//Retract shield
				ecs.deleteEntity(shieldID);

				break;
			}

			//Deny all incoming damage
			if (enemyHealth.health < lastFrameHealth)
				enemyHealth.health = lastFrameHealth;

			//Shimmer shield
			//The higher the multiplier in the sin equation, the faster the quiver
			if (ecs.entityHas<material>(shieldID))
			{
				float brightness = 0.75f + (sinf(25.0f * actionTimeRemaining)) / 4.0f;
				material& shieldMat = ecs.getComponent<material>(shieldID);
				shieldMat.color.x = brightness;
				shieldMat.color.y = brightness;
				shieldMat.color.z = brightness;
			}

			//TODO deflect incoming projectils

			//Damage player upon contact
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
							damageCooldownRemaining = damageInterval;
							playerHealth.health -= 18;
							//TODO play a shocking sound effect instead
							audio.playPannedSoundEffect(audio.sfxLaserBoom);
						}
					}
				}
			}

			break;
		}

			//Stunned state - Defeated. Low health.
			//Cannot do anything
			//Transition to:
			//suction (within vacuum)
		case (action::stunned):
		{
			//activating the dynamic camera for the mushroom 
			for (auto x : ecs.getEntityNames()) {
				if (ecs.entityHas<scriptComponent>(x.first)) {
					auto script = ecs.getComponent<scriptComponent>(x.first).scriptContainer.find("MushroomDynamicCameraScript");
					//check if it's within the map 
					if (script != ecs.getComponent<scriptComponent>(x.first).scriptContainer.end()) {
						MushroomDynamicCameraScript* camera = reinterpret_cast<MushroomDynamicCameraScript*>((*script).second.get());
						camera->mushroomBossDead = true;
					}
				}
			}

			if (!dialogueActivated && !UpgradeManager::GetInstance().level2Completed) {
				UpgradeManager::GetInstance().level2Completed = true;//set that level 2 is completed 
				entityID dialogueID = ecs.createEntityFromFile("Resources/PresetEntities/dialogueMushroomBoss.json");
				//setting the new position 
				ecs.getComponent<transform2D>(dialogueID).pos.x = ecs.getComponent<transform2D>(playerEntity).pos.x;
				ecs.getComponent<transform2D>(dialogueID).pos.y = ecs.getComponent<transform2D>(playerEntity).pos.y;
				UpgradeManager::GetInstance().upgradeTutorial = true;//telling dialogue manager that it is upgrading dialogue 
				dialogueActivated = true;
			}

			stunMultiplier = 0.0f;
			enemySprite.speed = 6.0f;

			if (enemyHealth.health < 1) enemyHealth.health = 1;

			//If caught in suction radius, get sucked in
			if (beingSucked)
			{
				currentAction = action::suction;
				//std::cout << "BOSS IN SUCTION\n";
				changeFacingAnimation(enemySprite, "SuckedRight", "SuckedLeft", "SuckedLeft", "SuckedRight");
			}
			
			break;
		}

			//Suction state - Being sucked in.
			//Cannot do anything
			//Transition to:
			//stunned (out of vacuum)
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
				changeFacingAnimation(enemySprite, "Stunned", "Stunned", "Stunned", "Stunned");
				//std::cout << "BOSS OUT SUCTION\n";
				currentAction = action::stunned;
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

		//Cannot transition into death
		//Only will be stunned when at one health
		//Cannot go below one health
		if (currentAction != action::stunned &&
			currentAction != action::suction &&
			enemyHealth.health <= 1)
		{
			currentAction = action::stunned;

			//std::cout << "BOSS STUNNED\n";

			//Set animation based off facing
			changeFacingAnimation(enemySprite, "Stunned", "Stunned", "Stunned", "Stunned");
		}
	}

	beingSucked = false; //Reset at every frame.
	//Update last frame health for invulnerability
	if (ecs.entityHas<Com_Health>(id))
		lastFrameHealth = ecs.getComponent<Com_Health>(id).health;
}

void MushroomBoss::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> MushroomBoss::createCopy()
{
	return std::make_shared<MushroomBoss>();
}

std::string MushroomBoss::getScriptName()
{
	return name;
}

//#include "../../Engine/ECS/component.h"
//#include "../../Engine/ECS/ECS.h"
//
//#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
//#include "../../Engine/ECS/Systems/Logic.h"
//#include "../../Engine/InputMgr/InputMgr.h"
//
//#include "MushroomBoss.h"
//
//MushroomBoss::MushroomBoss(const char* scriptName) :
//	scriptBase(scriptName)
//{
//}
//
//void MushroomBoss::init(float dt, unsigned int idToSet)
//{
//	UNREFERENCED_PARAMETER(dt);
//	id = idToSet;
//	state = scriptBase::ScriptState::UPDATE;
//}
//
//void MushroomBoss::update(float dt)
//{
//	UNREFERENCED_PARAMETER(dt);
//	int SetActive2 = 0;
//	entityID playerEntity{ 0 };
//	bool playerFound{ false };
//
//	//(attempt to) Get the player entity
//	for (auto const& e : ecs.getEntityNames())
//	{
//		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
//		{
//			playerFound = true;
//			playerEntity = e.first;
//			break;
//		}
//	}
//
//	if (playerFound &&
//		ecs.entityHas<transform2D>(id) &&
//		ecs.entityHas<AABB>(id) &&
//		ecs.entityHas<rigidBody2D>(id) &&
//		ecs.entityHas<spriteCom>(id) &&
//		ecs.entityHas<Com_Health>(id) &&
//		ecs.entityHas<transform2D>(playerEntity) &&
//		ecs.entityHas<AABB>(playerEntity) &&
//		ecs.entityHas<Com_Health>(playerEntity))
//	{
//		transform2D& enemyTransform = ecs.getComponent<transform2D>(id);
//		AABB& enemyAABB = ecs.getComponent<AABB>(id);
//		rigidBody2D& enemyRB = ecs.getComponent<rigidBody2D>(id);
//		spriteCom& enemySprite = ecs.getComponent<spriteCom>(id);
//		Com_Health& enemyHealth = ecs.getComponent<Com_Health>(id);
//		enemyAABB;
//		//Check enemy's health before continuing
//		if (enemyHealth.health <= 0)
//		{
//			state = scriptBase::ScriptState::END;
//			return;
//		}
//
//		transform2D& playerTransform = ecs.getComponent<transform2D>(playerEntity);
//		AABB& playerCollider = ecs.getComponent<AABB>(playerEntity);
//		Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);
//		playerCollider;
//		
//		float distance = Mathf::Length(enemyTransform.pos - playerTransform.pos);
//		if (alert) 
//		{
//			//CHASE PLAYER
//			Mathf::vec2 enemyToPlayer{ playerTransform.pos - enemyTransform.pos };
//
//			Force newForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(enemyToPlayer) * 500.0f, dt);
//
//
//			if (ecs.getNameOfEntity(id) == "MushroomBoss") 
//			{
//				newForce = Force(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(-enemyToPlayer) * 500.0f, dt);
//				/*if ()
//				{         shields up if all the monsters in the map still up
//				}*/
//			}
//			enemyRB.forceManager.setMaxVel(1000.0f);
//			enemyRB.forceManager.setLinearDrag(0.08f);
//			enemyRB.forceManager.AddLinearForce("movement", newForce);
//
//		}
//
//
//	}
//}
//
//void MushroomBoss::end(float dt)
//{
//	UNREFERENCED_PARAMETER(dt);
//	if (ecs.entityHas<scriptComponent>(id)) ecs.getComponent<scriptComponent>(id).scriptContainer.erase("MushroomBoss");
//	if (ecs.entityHas<spriteCom>(id)) ecs.getComponent<spriteCom>(id).isVisible = false;
//	if (ecs.entityHas<AABB>(id)) ecs.removeComponent<AABB>(id);
//	LogicSystemsPtr->sceneIsChanging = true; //HACK
//}
