// /******************************************************************************/
// /*!
// \file         EnemyBehaviour.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Mar, 2021
// \brief        This file contains the definitions for a generic enemy's logic
// \lines        0 Lines
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

#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/InputMgr/InputMgr.h"

#include "EnemyBehaviour.h"
#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"

//BE CAREFUL WHEN USING STATIC VARS
//They are shared among all instances of this script
/*static bool alert = false; //Whether enemy is alert of player and chasing
static float frameTime = 0.0f; //Animation
static float shotDelay = 2.0f;*/

//Facing direction helper function
void EnemyBehaviour::updateFacingDirection()
{
	if (currentVectorDirection.y > 0.0f)
	{
		if (abs(currentVectorDirection.y) > abs(currentVectorDirection.x))
			currentFacingDirection = facing::up;
		else if (currentVectorDirection.x > 0.0f)
			currentFacingDirection = facing::right;
		else
			currentFacingDirection = facing::left;
	}
	else
	{
		if (abs(currentVectorDirection.y) > abs(currentVectorDirection.x))
			currentFacingDirection = facing::down;
		else if (currentVectorDirection.x > 0.0f)
			currentFacingDirection = facing::right;
		else
			currentFacingDirection = facing::left;
	}
}

//Facing animation helper function
void EnemyBehaviour::changeFacingAnimation
(spriteCom& sprite,
	std::string const& up,
	std::string const& down,
	std::string const& left,
	std::string const& right)
{
	switch (currentFacingDirection)
	{
	case (facing::up):
		sprite.setAnimation(up);
		break;
	case(facing::down):
		sprite.setAnimation(down);
		break;
	case(facing::left):
		sprite.setAnimation(left);
		break;
	case(facing::right):
		sprite.setAnimation(right);
		break;
	}
}

EnemyBehaviour::EnemyBehaviour(const char* scriptName, float mv, float sm) :
	scriptBase(scriptName),
	normalMovementSpeed(mv),
	stunMultiplier(sm)
{}

void EnemyBehaviour::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void EnemyBehaviour::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	state = scriptBase::ScriptState::END;
}

void EnemyBehaviour::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

#if 0

void EnemyBehaviour::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	if (ecs.entityHas<transform2D>(id))
		initialScale = ecs.getComponent<transform2D>(id).scale;
	state = scriptBase::ScriptState::UPDATE;
}
int SetActive2 = 1;
void EnemyBehaviour::update(float dt)
{
	entityID playerEntity{ 0 };
	bool playerFound{ false };

	//for the dialogue of first stunned
	static bool firstStunnedInstructions;
	static bool firstDialogueLoaded;
	static entityID firstDialogueID;

	if (KeyManager.isKeyReleased('G'))
	{
		SetActive2 = 0;
	}

	//(attempt to) Get the player entity
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerFound = true;
			playerEntity = e.first;
			break;
		}
	}

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
		transform2D& enemyTransform = ecs.getComponent<transform2D>(id);
		AABB& enemyAABB = ecs.getComponent<AABB>(id);
		rigidBody2D& enemyRB = ecs.getComponent<rigidBody2D>(id);
		spriteCom& enemySprite = ecs.getComponent<spriteCom>(id);
		Com_Health& enemyHealth = ecs.getComponent<Com_Health>(id);
		enemyAABB;

		transform2D& playerTransform = ecs.getComponent<transform2D>(playerEntity);
		AABB& playerCollider = ecs.getComponent<AABB>(playerEntity);
		Com_Health& playerHealth = ecs.getComponent<Com_Health>(playerEntity);
		playerCollider;
		float distance = Mathf::Length(enemyTransform.pos - playerTransform.pos);

		//Make them look like they are being sucked in when they are
		if (beingSucked)
		{
			//Shrink to make them look as if they are becoming smaller
			float maximumShrinkDistance = 250.0f;
			if (distance < maximumShrinkDistance)
			{
				enemyTransform.scale.x = initialScale.x * (distance / maximumShrinkDistance);
				enemyTransform.scale.y = initialScale.y * (distance / maximumShrinkDistance);
			}
			else
				enemyTransform.scale = initialScale;

			currentAction = action::suction; //Change their animation
		}
		else
		{
			enemyTransform.scale = initialScale; //Resize to original size
			currentAction = action::walking; //Default action
		}

		//Check enemy's health before continuing
		if (enemyHealth.health <= 0)
		{
			currentAction = action::death;//death animation
			SoundClass::getInstance()->PlayAgain(enemyDeath);

			//Sprite to display based off facing
			switch (facingDirection)
			{
			case (facing::up):
				//sanity check
				if (enemySprite.animationData.find("DeathUp") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("DeathUp")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("DeathUp")->second[0];//setting the start frame
				}
				break;
			case (facing::left):
				if (enemySprite.animationData.find("DeathLeft") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("DeathLeft")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("DeathLeft")->second[0];//setting the start frame
				}
				break;
			case (facing::right):
				if (enemySprite.animationData.find("DeathRight") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("DeathRight")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("DeathRight")->second[0];//setting the start frame
				}
				break;
			case (facing::down):
				if (enemySprite.animationData.find("DeathDown") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("DeathDown")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("DeathDown")->second[0];//setting the start frame
				}
				break;
			}

			//continue the animation until the end frame
			frameTime -= dt * deathAnimationFrameTime;
			if (frameTime <= 0.0f)
			{
				//Go to this animation if not within the range of the current animation
				if (!(enemySprite.currentFrame >= enemySprite.startFrame && enemySprite.currentFrame <= enemySprite.endFrame))
				{
					enemySprite.currentFrame = enemySprite.startFrame;
				}

				if (enemySprite.currentFrame == enemySprite.endFrame) {
					state = scriptBase::ScriptState::END;
				}
				else {
					enemySprite.currentFrame++;
				}
				frameTime = 0.3f;
			}

			//setting the corresponding sprite current tex coords
			if (enemySprite.frameData.size() != 0) {
				enemySprite.currentTexCoords = enemySprite.frameData[enemySprite.currentFrame];//getting the specific frame data
			}
			return;
		}

		//show the dialogue
		if (firstStunnedInstructions == false && isStunned == true) {
			for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
				//auto& eInnerTransform = ecs.getComponent<transform2D>(eInner);//getting the transform of the player

				//if the dialogue is not loaded and the dialogue to show is true
				if (firstDialogueLoaded == false) {
					//creating the entity right below the player pos
					//firstDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/ChatBubbleStunned.json");
					firstDialogueID = ecs.createEntityFromFile("Resources/PresetEntities/dialogueStunned.json");
					//setting the new position 
					ecs.getComponent<transform2D>(firstDialogueID).pos.x = ecs.getComponent<transform2D>(playerEntity).pos.x;
					ecs.getComponent<transform2D>(firstDialogueID).pos.y = ecs.getComponent<transform2D>(playerEntity).pos.y;
					//ecs.getComponent<transform2D>(firstDialogueID).pos.x = 0.f;//setting the dialogue position x with the offset
					//ecs.getComponent<transform2D>(firstDialogueID).pos.y = -1.3f;//setting the dialogue position y with the offset

					//ecs.setEntityParent(firstDialogueID, eInner);//setting the parent to the player
					//ecs.getComponent<spriteCom>(firstDialogueID).isVisible = true;//setting it to be visible
					firstDialogueLoaded = true;
				}
			}

			if (firstDialogueLoaded == true) {
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

		//checking if it's stunned
		if (isStunned == true && beingSucked == false) {
			currentAction = action::stunned;//stun animation

			//Sprite to display based off facing
			switch (facingDirection)
			{
			case (facing::up):
				if (enemySprite.animationData.find("StunnedForward") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("StunnedForward")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("StunnedForward")->second[0];//setting the start frame
				}
				break;
			case (facing::left):
				if (enemySprite.animationData.find("StunnedLeft") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("StunnedLeft")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("StunnedLeft")->second[0];//setting the start frame
				}
				break;
			case (facing::right):
				if (enemySprite.animationData.find("StunnedRight") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("StunnedRight")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("StunnedRight")->second[0];//setting the start frame
				}
				break;
			case (facing::down):
				if (enemySprite.animationData.find("StunnedBack") != enemySprite.animationData.end()) {
					enemySprite.endFrame = enemySprite.animationData.find("StunnedBack")->second.back();//getting the last frame
					enemySprite.startFrame = enemySprite.animationData.find("StunnedBack")->second[0];//setting the start frame
				}
				break;
			}


			//continue the animation until the end frame
			frameTime -= dt * deathAnimationFrameTime;
			if (frameTime <= 0.0f)
			{
				//Go to this animation if not within the range of the current animation
				if (!(enemySprite.currentFrame >= enemySprite.startFrame && enemySprite.currentFrame <= enemySprite.endFrame))
				{
					enemySprite.currentFrame = enemySprite.startFrame;
				}

				if (enemySprite.currentFrame == enemySprite.endFrame) {
					enemySprite.currentFrame = enemySprite.startFrame;//replay the animation
				}
				else {
					enemySprite.currentFrame++;
				}
				frameTime = 3.5f;
			}

			//setting the corresponding sprite current tex coords
			enemySprite.currentTexCoords = enemySprite.frameData[enemySprite.currentFrame];//getting the specific frame data
			return;
		}

		if (alert)
		{
			//CHASE PLAYER
			Mathf::vec2 enemyToPlayer{ playerTransform.pos - enemyTransform.pos };

			Force newForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(enemyToPlayer) * 500.0f, dt);

			//For chilli enemies, attempt to flee if distance less than 300
			if (ecs.getNameOfEntity(id) == "ChiliNew" && distance < 300.0f)
				newForce = Force(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(-enemyToPlayer) * 500.0f, dt);

			enemyRB.forceManager.AddLinearForce("movement", newForce);

			enemyRB.forceManager.setMaxVel(1000.0f);
			enemyRB.forceManager.setLinearDrag(0.08f);

			//if player is 300 animate popping up then chases the player
			//if (ecs.getNameOfEntity(id) == "Mushroom" && distance < 300.0f )
				//{animate player popping out}
				//once player pops out he chases player
				//

			//Animation
			if (enemyToPlayer.y > 0.0f)
			{
				if (abs(enemyToPlayer.y) > abs(enemyToPlayer.x))
					facingDirection = facing::up;
				else if (enemyToPlayer.x > 0.0f)
					facingDirection = facing::right;
				else
					facingDirection = facing::left;
			}
			else
			{
				if (abs(enemyToPlayer.y) > abs(enemyToPlayer.y))
					facingDirection = facing::down;
				else if (enemyToPlayer.x > 0.0f)
					facingDirection = facing::right;
				else
					facingDirection = facing::left;
			}

			//Sprite to display based off facing
			switch (facingDirection)
			{
			case (facing::up):
				switch (currentAction)
				{
				case EnemyBehaviour::action::suction:
					if (enemySprite.animationData.find("DraggedLeft") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("DraggedLeft")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("DraggedLeft")->second[0];//setting the start frame
					}
					break;
				default:
					if (enemySprite.animationData.find("WalkUp") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("WalkUp")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("WalkUp")->second[0];//setting the start frame
					}
					break;
				}
				break;
			case (facing::left):
				switch (currentAction)
				{
				case EnemyBehaviour::action::suction:
					if (enemySprite.animationData.find("DraggedLeft") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("DraggedLeft")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("DraggedLeft")->second[0];//setting the start frame
					}
					break;
				default:
					if (enemySprite.animationData.find("WalkLeft") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("WalkLeft")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("WalkLeft")->second[0];//setting the start frame
					}
					break;
				}
				break;
			case (facing::right):
				switch (currentAction)
				{
				case EnemyBehaviour::action::suction:
					if (enemySprite.animationData.find("DraggedRight") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("DraggedRight")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("DraggedRight")->second[0];//setting the start frame
					}
					break;
				default:
					if (enemySprite.animationData.find("WalkRight") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("WalkRight")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("WalkRight")->second[0];//setting the start frame
					}
					break;
				}
				break;
			case (facing::down):
				switch (currentAction)
				{
				case EnemyBehaviour::action::suction:
					if (enemySprite.animationData.find("DraggedRight") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("DraggedRight")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("DraggedRight")->second[0];//setting the start frame
					}
					break;
				default:
					if (enemySprite.animationData.find("WalkDown") != enemySprite.animationData.end()) {
						enemySprite.endFrame = enemySprite.animationData.find("WalkDown")->second.back();//getting the last frame
						enemySprite.startFrame = enemySprite.animationData.find("WalkDown")->second[0];//setting the start frame
					}
					break;
				}
				break;
			}

			//Animate
			frameTime -= dt * Mathf::Length(enemyRB.vel);
			if (frameTime <= 0.0f)
			{
				//Go to this animation if not within the range of the current animation
				if (!(enemySprite.currentFrame >= enemySprite.startFrame && enemySprite.currentFrame <= enemySprite.endFrame))
				{
					enemySprite.currentFrame = enemySprite.startFrame;
				}
				//enemySprite.currentFrame = (enemySprite.currentFrame + 1) % 4;
				//changing of the current frame. TO DO: Abstract to own system to remove increment
				enemySprite.currentFrame == enemySprite.endFrame ? enemySprite.currentFrame = enemySprite.startFrame : enemySprite.currentFrame++;
				frameTime = 18.f;
			}

			//setting the corresponding sprite current tex coords
			if (enemySprite.frameData.size() != 0) {
				enemySprite.currentTexCoords = enemySprite.frameData[enemySprite.currentFrame];//getting the specific frame data
			}

			float pos[] = { 0,0,0 };
			//Damage player if they make contact
			std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
			for (entityID e : collidedEntities)
			{
				if (e == playerEntity)
				{
					//Shield denies attack
					//This is what happens when Kettle Knight does not have shields on
					if (!(ecs.entityHas<Com_Abilities>(e) && ecs.getComponent<Com_Abilities>(e).typeAbilities == Com_Abilities::abilities::shield && MouseManager.isButtonPressed(MouseButton::RIGHT_BUTTON)))
					{
						//Push
						if (ecs.entityHas<rigidBody2D>(playerEntity))
						{
							rigidBody2D& playerRB = ecs.getComponent<rigidBody2D>(playerEntity);
							playerRB.vel += Mathf::Normalize(enemyToPlayer) * 1000.0f;
						}
						//Hurt
						if (SetActive2 == 1)
						{
							playerHealth.health -= 10;
							SoundClass::getInstance()->PlayAgain(bitten);
						}
						if (SetActive2 == 0)
						{
							playerHealth.health;
						}

					}
				}
			}

			//Shoot fireballs if they are a chili and are far enough
			/*if (ecs.getNameOfEntity(id) == "ChiliNew" && distance > 300.0f)
			{
				shotDelay -= dt;
				if (shotDelay <= 0.0f)
				{
					entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");
					ecs.getComponent<transform2D>(newID).pos.x = enemyTransform.pos.x;
					ecs.getComponent<transform2D>(newID).pos.y = enemyTransform.pos.y;

					rigidBody2D eRigidBody;
					ecs.getComponent<transform2D>(newID).rotation = 360.f + 90.f - getVectorRotationRadian(enemyToPlayer) * 180.f / 3.14f;
					Force fireBallForce(Force::ForceType::LINEAR_FORCE, 400.0f * Normalize(enemyToPlayer), 100.f);
					eRigidBody.forceManager.AddLinearForce("fireBallForce", fireBallForce, true);//adding linear force to the water droplet
					eRigidBody.forceManager.setMaxVel(200.f);

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

					shotDelay = 2.0f;

					LogicSystemsPtr->sceneIsChanging = true; //HACK
				}
			}*/

			//Lose alert upon going far enough
			if (distance > 1200.0f)
				alert = false;
		}
		else
		{
			//if not alerted just do walk left animation 
			if (enemySprite.animationData.find("WalkLeft") != enemySprite.animationData.end()) {
				enemySprite.currentFrame = enemySprite.animationData.find("WalkLeft")->second[0];
				enemySprite.endFrame = enemySprite.animationData.find("WalkLeft")->second.back();//getting the last frame
				enemySprite.startFrame = enemySprite.animationData.find("WalkLeft")->second[0];//setting the start frame

				//setting the corresponding sprite current tex coords
				if (enemySprite.frameData.size() != 0) {
					enemySprite.currentTexCoords = enemySprite.frameData[enemySprite.currentFrame];//getting the specific frame data
				}
			}
			//Wait for distance to be near enough to start alert
			if (distance < 800.0f)
				alert = true;
		}
	}
	beingSucked = false; //Reset at every frame.
}

void EnemyBehaviour::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//Enemy lost all HP and is down
	//TODO outright deleting the enemy entity results in strange instability
	//in terms of gameplay. Disable enemy's behaviour and turn them invisible instead
	ecs.deleteEntity(id);
	//if (ecs.entityHas<scriptComponent>(id)) ecs.getComponent<scriptComponent>(id).scriptContainer.erase("EnemyBehaviour");
	//if (ecs.entityHas<spriteCom>(id)) ecs.getComponent<spriteCom>(id).isVisible = false;
	//if (ecs.entityHas<AABB>(id)) ecs.removeComponent<AABB>(id);
	//LogicSystemsPtr->sceneIsChanging = true; //HACK
}
#endif

std::shared_ptr<scriptBase> EnemyBehaviour::createCopy()
{
	return std::make_shared<EnemyBehaviour>();
}

std::string EnemyBehaviour::getScriptName()
{
	return name;
}