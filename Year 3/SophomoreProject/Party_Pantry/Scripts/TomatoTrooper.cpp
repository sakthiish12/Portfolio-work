// /******************************************************************************/
// /*!
// \file         TomatoTrooper.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         28th Feb, 2022
// \brief        This file contains the definitions for tomato trooper enemy's
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

#include "TomatoTrooper.h"
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/Sound/SoundManagerV2.h"

#include "PlayerController.h"

//Dialogue helpers
static bool firstStunnedInstructions;
static bool firstDialogueLoaded;
static entityID firstDialogueID;

constexpr float TomatoTrooper::playerKnockback_cooldown_max = 5.f; //doesnt seems to matter???
float TomatoTrooper::playerKnockback_cooldown = TomatoTrooper::playerKnockback_cooldown_max;
Force knockbackPlayerForce(Force::ForceType::LINEAR_FORCE, vec2{ 0.f, 0.f }, 1.f);
constexpr float KNOCKBACK_SPEED = 1500.f; //set this for the knock back speed
constexpr float SPEED_LIMIT = 10000.f; //original force added to movement in active state
constexpr float SPEED_LIMIT_TWICED = SPEED_LIMIT * 2.f;
constexpr float SPEED_LIMIT_HALFED = SPEED_LIMIT / 2.f;
constexpr float MAX_SPEED_LIMIT = 2000.f; //original force added to movement in active state
constexpr float MAX_SPEED_LIMIT_TWICED = MAX_SPEED_LIMIT * 2.f;
constexpr float MAX_SPEED_LIMIT_HALFED = MAX_SPEED_LIMIT / 2.f;

//Ctor
TomatoTrooper::TomatoTrooper(const char* scriptName) :
	EnemyBehaviour(scriptName, 150.0f, 1.0f)
{

}

void TomatoTrooper::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	//if (ecs.entityHas<rigidBody2D>(id))
	//	ecs.getComponent<rigidBody2D>(id).forceManager.setLinearDrag(0.25f);
	if (ecs.entityHas<spriteCom>(id))
		changeFacingAnimation(ecs.getComponent<spriteCom>(id), "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
	state = scriptBase::ScriptState::UPDATE;
}

void TomatoTrooper::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	//End glow
	if (ecs.entityHas<material>(id))
	{
		ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	}

	previousFacingDirection = currentFacingDirection; //Update previous direction tracker
	
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
		rigidBody2D& enemyRB = ecs.getComponent<rigidBody2D>(id);
		spriteCom& enemySprite = ecs.getComponent<spriteCom>(id);
		Com_Health& enemyHealth = ecs.getComponent<Com_Health>(id);
		
		transform2D& playerTransform = ecs.getComponent<transform2D>(playerEntity);
		Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);

		//Distance between player and enemy
		float distance = Mathf::Length(enemyTransform.pos - playerTransform.pos); 
		
		if (playerKnockback_cooldown > 0.f)
		{
			if (ecs.entityHas<rigidBody2D>(playerEntity))
			{
				rigidBody2D& playerRB = ecs.getComponent<rigidBody2D>(playerEntity);
				playerRB.forceManager.AddLinearForce("knockbackPlayer", knockbackPlayerForce);
				playerRB.forceManager.setLinearDrag(0.9f);
			}
			playerKnockback_cooldown -= dt;
		}
		else
			knockbackPlayerForce.setLinearForce(vec2{ 0.f, 0.f });

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
			changeFacingAnimation(enemySprite, "DeathUp", "DeathDown", "DeathLeft", "DeathRight");
		}

		//Transition into Stun state if not dying and stun timer > 0
		//Transitionable from any state BUT death
		if (currentAction != action::death &&
			currentAction != action::stunned &&
			currentAction != action::suction &&
			stunTimeRemaining > 0.0f)
		{
			if (currentAction == action::charge)
				audio.playPannedSoundEffect(audio.sfxDing);

			currentAction = action::stunned;
			
			//Set animation based off facing
			changeFacingAnimation(enemySprite, "StunnedBack", "StunnedForward", "StunnedLeft", "StunnedRight");
		}

		switch (currentAction)
		{
		//Idle state - Waiting for player to arrive
		//Can transition to:
		//Alert
		//Stun
		//Death
		case (action::idle):
		{
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
			
		//Alert state - Noted presence, preparing to act
		//Can transition to:
		//Active
		//Stun
		//Death
		case (action::alert):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 0.0f;
			//Decrease action time remaining.
			//If it is less than 0, begin acting
			actionTimeRemaining -= dt;
			if (actionTimeRemaining < 0.0f)
			{
				currentAction = action::active;
				break;
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
			
		//Active state - Follow player until near or far enough
		//Can transition to:
		//Idle
		//Prepare
		//Stun
		//Death
		case (action::active):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 12.0f;
			//If far enough from player, transition to idle
			if (distance > 1200.0f)
			{
				currentAction = action::idle;
				break;
			}
			//If close enough to player, transition to prepare to attack
			if (distance < 200.0f)
			{
				currentAction = action::prepare;
				actionTimeRemaining = 1.0f; //Prepare for this long
				break;
			}

			//Follow the player otherwise
			currentVectorDirection = playerTransform.pos - enemyTransform.pos; //from enemy to player
			Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(currentVectorDirection) * SPEED_LIMIT, dt);

			//Zero out movement force if going faster than speed limit
			if (Mathf::Length(enemyRB.vel) > normalMovementSpeed)
				movementForce.setLinearForce(Mathf::vec2{});

			enemyRB.forceManager.AddLinearForce("movement", movementForce);
			enemyRB.forceManager.setMaxVel(MAX_SPEED_LIMIT);
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
			
		//Prepare state - Move back a little before charging ahead
		//Can transition to:
		//Charge
		//Stun
		//Death
		case (action::prepare):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = -6.0f; //Play the animation but in reverse
			//Decrease action time remaining.
			//If it is less than 0, begin the charge
			actionTimeRemaining -= dt;
			if (actionTimeRemaining < 0.0f)
			{
				currentAction = action::charge;
				//TODO leave a trail of smoke, play charging SFX
				audio.playPannedSoundEffect(audio.sfxWhoosh1);
				actionTimeRemaining = 1.0f; //Charge for this long
				break;
			}

			//Walk backwards
			currentVectorDirection = playerTransform.pos - enemyTransform.pos; // enemy to player
			Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(-currentVectorDirection) * SPEED_LIMIT, dt);

			//Zero out movement force if going faster than speed limit
			//Walking back at half the normal speed
			if (Mathf::Length(enemyRB.vel) > normalMovementSpeed * 0.5f)
				movementForce.setLinearForce(Mathf::vec2{});

			enemyRB.forceManager.AddLinearForce("movement", movementForce);
			enemyRB.forceManager.setMaxVel(MAX_SPEED_LIMIT_HALFED);
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
			
		//Charge state - Quickly moving ahead in an attempt to hurt player
		//The tomato can actually hurt the player like this
		//The tomato is very vulnerable in this state,
		//being able to be sucked in and stunned for longer while charging

		//Can transition to:
		//Recover
		//(NOT) Suction
		//Stun
		//Death
		case (action::charge):
		{
			stunMultiplier = 5.0f;
			enemySprite.speed = 24.0f;
			//Decrease action time remaining.
			//If it is less than 0, stop the charge
			actionTimeRemaining -= dt;
			if (actionTimeRemaining < 0.0f)
			{
				currentAction = action::recover;
				actionTimeRemaining = 1.0f; //Recover for this long
				break;
			}

			//Charge STRAIGHT ahead

			//Glow
			if (ecs.entityHas<material>(id))
			{
				ecs.getComponent<material>(id).color = Mathf::Vector4{ 2.0f, 2.0f, 2.0f, 1.0f };
			}

			//To give the player the opportunity to dodge easily,
			//the tomato only charges in a straight line
			//currentVectorDirection = playerTransform.pos - enemyTransform.pos;
			Force movementForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(currentVectorDirection) * SPEED_LIMIT, dt);

			//Zero out movement force if going faster than speed limit
			//Charging at double speed
			if (Mathf::Length(enemyRB.vel) > normalMovementSpeed * 2.0f)
				movementForce.setLinearForce(Mathf::vec2{});

			enemyRB.forceManager.AddLinearForce("movement", movementForce);
			enemyRB.forceManager.setMaxVel(MAX_SPEED_LIMIT_TWICED);
			enemyRB.forceManager.setLinearDrag(0.05f);

			//Update facing direction
			updateFacingDirection();

			//If there is a change in the facing direction, reset animation
			if (currentFacingDirection != previousFacingDirection)
			{
				changeFacingAnimation(enemySprite, "WalkUp", "WalkDown", "WalkLeft", "WalkRight");
			}

			//If collided with anything, stop and recover
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
							playerHealth.health -= 9;
							//SoundClass::getInstance()->PlayAgain(bitten);
							audio.playPannedSoundEffect(audio.sfxEnemyBite);
						}
						//Enemy gets knocked back, stops and recovers
						//TODO add knocked sound effect and particle effects	
						enemyRB.vel += Mathf::Normalize(-currentVectorDirection) * 1000.0f;
						//enemyRB.vel += Mathf::Normalize(-currentVectorDirection) * 60000.f * dt;
						currentAction = action::recover;
						break;
					}
				}
			}
			
			//If caught in suction radius, get sucked in
			//Made the game too easy
			/*if (beingSucked)
			{
				currentAction = action::suction;
				changeFacingAnimation(enemySprite, "DraggedRight", "DraggedLeft", "DraggedLeft", "DraggedRight");
				break;
			}*/

			break;
		}
			
		//Recover state - Stopping for a bit after charging
		//Can transition to:
		//Active
		//Stun
		//Death
		case (action::recover):
		{
			stunMultiplier = 1.0f;
			enemySprite.speed = 6.0f;
			//Decrease action time remaining.
			//If it is less than 0, get active again
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
				actionTimeRemaining -= dt;

			if (actionTimeRemaining < 0.0f)
			{
				currentAction = action::active;
				break;
			}

			break;
		}
			

		//Suction state - Being sucked in by the player's vacuum
		//Can transition to:
		//Idle
		//Death
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

		//Stunned state - Stunned by attacks and airblasts
		//Displays stun tutorial dialogue if the first enemy stunned
		//Transitions into:
		//Idle
		//Suction
		//Death
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

			//Handle tutorial dialogue
			if (!firstStunnedInstructions && stunTimeRemaining > 0.0f)
			{
				//If the dialogue is not loaded and the dialogue to show is true
				if (!firstDialogueLoaded)
				{
					firstDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/dialogueStunned.json");
					//setting the new position 
					ecs.getComponent<transform2D>(firstDialogueID).pos.x = ecs.getComponent<transform2D>(playerEntity).pos.x;
					ecs.getComponent<transform2D>(firstDialogueID).pos.y = ecs.getComponent<transform2D>(playerEntity).pos.y;
					firstDialogueLoaded = true;
				}
				else
				{
					//check if the player clicked
					UiLayer::GetInstance().dialoguePause = true;//the dialogue is pause
					UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game

					//if left clicked
					if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON)) {
						//ecs.deleteEntity(firstDialogueID);//delete the entity
						firstStunnedInstructions = true;
						UiLayer::GetInstance().dialoguePause = false;//the dialogue is pause
						UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing
					}
				}
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
			
		//Death state - Getting destroyed. Breaks out of update loop
		//Cannot transition to another state
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

		}
		//std::cout << enemyRB.vel.x << " , " << enemyRB.vel.y << std::endl;
	}
	beingSucked = false; //Reset at every frame.
}

void TomatoTrooper::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> TomatoTrooper::createCopy()
{
	return std::make_shared<TomatoTrooper>();
}

std::string TomatoTrooper::getScriptName()
{
	return name;
}