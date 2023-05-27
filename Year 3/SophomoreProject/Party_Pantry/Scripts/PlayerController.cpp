// /******************************************************************************/
// /*!
// \file         PlayerController.cpp
// \project name Engine
// \author(s)    Ryan Wang (70%)
//               Wilfred Ng (30%)
// \date         12th Apr, 2022
// \brief        This file contains the definitions for player's controller logic
// \lines        1115
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
#include "../../Engine/InputMgr/InputMgr.h"
#include "../../Engine/AssetManager/AssetManager.h"
#include "../../Engine/Debugging/PhysicsDebugging.h"
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/ECS/Systems/Physics/physicsSystem.h" //Code reuse
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing 

#include "EnemyBehaviour.h"
#include "WaterShotScript.h"
#include "HUD/TomatoCounterScript.h"
#include "HUD/MushroomCounterScript.h"
#include "HUD/HUDUIScript.h"

#include "PlayerController.h"
#include <Sound/SoundManagerV2.h>
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterSuction.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterWalkingTrail.h"
#include "BushScript.h"
#include "Level2/BlockageScript.h"
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../Engine/AssetManager/UpgradeManager.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"


//Movement force constants
constexpr float normalMovingForce{ 10000.0f };
constexpr float superSpeedForce{ 20000.0f };
constexpr float ultraSpeedForce{ 100000.0f };

//Enemies hit while player is in ultra speed or in melee
//To prevent same enemies from being hit again
std::vector<entityID> enemiesHit;

//DO NOT USE STATICS
//static float frameTime = 0.0f;

//Local helper function, adapapted form physicsSystem.cpp
//playerT describes source transform
//entityT describes other's transform
//target describes the direction and distance of the arc
//arc angle describes the half angle of coverage by the arc
bool checkIfInSuctionArc(transform2D& playerT, transform2D& entityT, vec2 target, float arc_angle)
{
	//2 conditions:
	//Make sure the target entity is closed enough to be considered
	//Make sure the target entity is actually being pointed at
	vec2 dist = entityT.pos - playerT.pos;
	if (Dot(dist, dist) >= Dot(target, target))
		return false;
	if (Dot(target, dist) / (Length(target) * Length(dist)) <= cos(arc_angle))
		return false;
	return true;
}

//Shoot a single airblast
//Solely to help shootAirblasts()
void PlayerController::singleAirblast(float direction, float initialSpeed, int d, float sm)
{
	entityID airblastID = ecs.createEntityFromFile("Resources/PresetEntities/waterdroplet.json");

	//Initial positioning
	ecs.getComponent<transform2D>(airblastID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 20.0f * cosf(direction); //Apparently not using oTransform fixes this? But the camera is still acting strange.
	ecs.getComponent<transform2D>(airblastID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 20.0f * sinf(direction);
	ecs.getComponent<transform2D>(airblastID).pos.z = 1.5f;//setting z layering

	//std::cout << "Kettle Knight is at " << ecs.getComponent<transform2D>(id).pos.x << ", " << ecs.getComponent<transform2D>(id).pos.y << std::endl;
	//std::cout << "Shot beginning at " << ecs.getComponent<transform2D>(airblastID).pos.x << ", " << ecs.getComponent<transform2D>(airblastID).pos.y << std::endl;

	//Set bullet direction
	ecs.getComponent<transform2D>(airblastID).rotation = Mathf::RadianToDegree(direction);

	//Set force
	//Forces for some reason are inconsistent if they have a short lifetime
	//Force airblastForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(direction), sinf(direction)), 0.01f);
	rigidBody2D airblastRB{};
	airblastRB.vel.x = initialSpeed * cosf(direction) + ecs.getComponent<rigidBody2D>(id).vel.x;
	airblastRB.vel.y = initialSpeed * sinf(direction) + ecs.getComponent<rigidBody2D>(id).vel.y;
	//airblastRB.forceManager.AddLinearForce("AirblastForce", airblastForce, false);
	airblastRB.forceManager.setMaxVel(initialSpeed + 500.0f);
	airblastRB.forceManager.setLinearDrag(0.05f);
	ecs.addComponent<rigidBody2D>(airblastID, airblastRB);

	//Set decay timer
	Com_TimeDestructor airblastTD;
	airblastTD.timeDelayDestructor = 1.0f; //1 second lifetime
	ecs.addComponent<Com_TimeDestructor>(airblastID, airblastTD);

	//Set typing
	Com_type airblastType;
	airblastType.type = Com_type::type::bullet;
	ecs.addComponent<Com_type>(airblastID, airblastType);

	//Set AABB
	AABB airblastAABB;
	ecs.addComponent<AABB>(airblastID, airblastAABB);

	//Set script and parameters
	scriptComponent airblastScript;
	//(It used to be a water shot)
	airblastScript.scriptContainer["WaterShotScript"] = ScriptManager.createFromScriptDatabase("WaterShotScript");
	ecs.addComponent<scriptComponent>(airblastID, airblastScript);

	if (ecs.entityHas<scriptComponent>(airblastID))
	{
		auto& sc{ ecs.getComponent <scriptComponent>(airblastID).scriptContainer };
		auto scriptItr{ sc.find("WaterShotScript") };
		if (scriptItr != sc.end())
		{
			WaterShotScript* water = reinterpret_cast<WaterShotScript*>((*scriptItr).second.get());
			water->damage = d;
			water->stunMultiplier = sm;
		}
	}
}

//Shoot n airblasts in a certain spread (radians) and initial speed
void PlayerController::shootAirblasts(size_t n, float spread, float initialSpeed, int d, float sm)
{
	const float piovertwo = 1.57079632679f;
	float shotAngle = piovertwo - mouseAngle;
	//Odd number fired
	if (n % 2)
	{
		//First shoot the central projectile
		singleAirblast(shotAngle, initialSpeed, d, sm);
		float directionalDeviation = 0.0f;
		for (size_t pair = 0; pair < n / 2; ++pair)
		{
			directionalDeviation = spread / 2.0f * (float)((n /	2) - pair) / (float)(n / 2);
			singleAirblast(shotAngle + directionalDeviation, initialSpeed, d, sm);
			singleAirblast(shotAngle - directionalDeviation, initialSpeed, d, sm);
		}
	}
	else //Even number fired
	{
		float directionalDeviation;
		for (size_t p = 0; p < n; ++p)
		{
			directionalDeviation = spread * (-0.5f * (float)p / (float)n);
			singleAirblast(shotAngle + directionalDeviation, initialSpeed,d, sm);
		}
	}
}

//Shoot a fireball
//Direction is altered by variance
void PlayerController::shootFlame(float variance, float initialSpeed)
{
	const float piovertwo = 1.57079632679f;
	float shotAngle = piovertwo - mouseAngle;

	entityID flameID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");

	//Initial positioning
	ecs.getComponent<transform2D>(flameID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 20.0f * cosf(shotAngle + variance); //Apparently not using oTransform fixes this? But the camera is still acting strange.
	ecs.getComponent<transform2D>(flameID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 20.0f * sinf(shotAngle + variance);

	//Set bullet direction
	ecs.getComponent<transform2D>(flameID).rotation = Mathf::RadianToDegree(shotAngle + variance);

	//Set force
	//Forces are inconsistent
	//Force flameForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(shotAngle + variance), sinf(shotAngle + variance)), 1.0f);
	rigidBody2D flameRB{};
	flameRB.vel.x = initialSpeed * cosf(shotAngle + variance) + ecs.getComponent<rigidBody2D>(id).vel.x;
	flameRB.vel.y = initialSpeed * sinf(shotAngle + variance) + ecs.getComponent<rigidBody2D>(id).vel.y;
	Force fireBallForce(Force::ForceType::LINEAR_FORCE, 5000.0f * Normalize(Vec2{cosf(shotAngle + variance), sinf(shotAngle + variance)}), 100.f);
	flameRB.forceManager.AddLinearForce("fireBallForce", fireBallForce, true);//adding linear force to the water droplet
	flameRB.forceManager.setMaxVel(initialSpeed);
	flameRB.forceManager.setLinearDrag(0.00f);
	ecs.addComponent<rigidBody2D>(flameID, flameRB);

	//Set decay timer
	Com_TimeDestructor flameTD;
	flameTD.timeDelayDestructor = 5.0f; //1 second lifetime
	ecs.addComponent<Com_TimeDestructor>(flameID, flameTD);

	//Set typing
	Com_type flameType;
	flameType.type = Com_type::type::bullet;
	ecs.addComponent<Com_type>(flameID, flameType);

	//Set AABB
	AABB flameAABB;
	ecs.addComponent<AABB>(flameID, flameAABB);

	//Set script
	scriptComponent flameScript;
	//(It used to be a water shot)
	flameScript.scriptContainer["FireShotScript"] = ScriptManager.createFromScriptDatabase("FireShotScript");
	ecs.addComponent<scriptComponent>(flameID, flameScript);
}

//Shoot a tornado out
void PlayerController::shootTornado()
{
	const float piovertwo = 1.57079632679f;
	float direction = piovertwo - mouseAngle;

	const float initialSpeed = 200.0f;

	entityID tornadoID = ecs.createEntityFromFile("Resources/PresetEntities/Tornado.json");

	//Initial positioning
	ecs.getComponent<transform2D>(tornadoID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 20.0f * cosf(direction); //Apparently not using oTransform fixes this? But the camera is still acting strange.
	ecs.getComponent<transform2D>(tornadoID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 20.0f * sinf(direction);
	ecs.getComponent<transform2D>(tornadoID).pos.z = 1.5f;//setting z layering

	//std::cout << "Kettle Knight is at " << ecs.getComponent<transform2D>(id).pos.x << ", " << ecs.getComponent<transform2D>(id).pos.y << std::endl;
	//std::cout << "Shot beginning at " << ecs.getComponent<transform2D>(airblastID).pos.x << ", " << ecs.getComponent<transform2D>(airblastID).pos.y << std::endl;

	//Set bullet direction
	//ecs.getComponent<transform2D>(tornadoID).rotation = Mathf::RadianToDegree(direction);

	//Set force
	//Forces for some reason are inconsistent if they have a short lifetime
	//Force airblastForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(direction), sinf(direction)), 0.01f);
	rigidBody2D airblastRB{};
	airblastRB.vel.x = initialSpeed * cosf(direction) + ecs.getComponent<rigidBody2D>(id).vel.x;
	airblastRB.vel.y = initialSpeed * sinf(direction) + ecs.getComponent<rigidBody2D>(id).vel.y;
	Mathf::Vec2 tornadoVec{ 10000.0f * cosf(direction), 10000.0f * sinf(direction) };
	Force tornadoForce(Force::ForceType::LINEAR_FORCE, tornadoVec, 100.0f);
	airblastRB.forceManager.AddLinearForce("AirblastForce", tornadoForce, true);
	airblastRB.forceManager.setMaxVel(initialSpeed);
	airblastRB.forceManager.setLinearDrag(0.001f);
	ecs.addComponent<rigidBody2D>(tornadoID, airblastRB);

	//Set typing
	Com_type airblastType;
	airblastType.type = Com_type::type::bullet;
	ecs.addComponent<Com_type>(tornadoID, airblastType);

	//Set script and parameters
	scriptComponent airblastScript;
	//(It used to be a water shot)
	airblastScript.scriptContainer["Tornado"] = ScriptManager.createFromScriptDatabase("Tornado");
	ecs.addComponent<scriptComponent>(tornadoID, airblastScript);
}

//Shoot a firebomb, that explodes into many fireballs upon collision
void PlayerController::shootFirebomb()
{
	const float initialSpeed = 200.0f;
	const float piovertwo = 1.57079632679f;
	float shotAngle = piovertwo - mouseAngle;

	entityID flameID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");

	//Sizing
	ecs.getComponent<transform2D>(flameID).scale.x = 100.0f;
	ecs.getComponent<transform2D>(flameID).scale.y = 100.0f;

	//Initial positioning
	ecs.getComponent<transform2D>(flameID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 20.0f * cosf(shotAngle); //Apparently not using oTransform fixes this? But the camera is still acting strange.
	ecs.getComponent<transform2D>(flameID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 20.0f * sinf(shotAngle);

	//Set bullet direction
	ecs.getComponent<transform2D>(flameID).rotation = Mathf::RadianToDegree(shotAngle);

	//Set force
	//Forces are inconsistent
	//Force flameForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(shotAngle + variance), sinf(shotAngle + variance)), 1.0f);
	rigidBody2D flameRB{};
	flameRB.vel.x = initialSpeed * cosf(shotAngle) + ecs.getComponent<rigidBody2D>(id).vel.x;
	flameRB.vel.y = initialSpeed * sinf(shotAngle) + ecs.getComponent<rigidBody2D>(id).vel.y;
	Mathf::Vec2 tornadoVec{ 10000.0f * cosf(shotAngle), 10000.0f * sinf(shotAngle) };
	Force tornadoForce(Force::ForceType::LINEAR_FORCE, tornadoVec, 100.0f);
	flameRB.forceManager.AddLinearForce("AirblastForce", tornadoForce, true);
	//flameRB.forceManager.AddLinearForce("FlameForce", flameForce, false);
	flameRB.forceManager.setMaxVel(initialSpeed + 500.0f);
	flameRB.forceManager.setLinearDrag(0.00f);
	ecs.addComponent<rigidBody2D>(flameID, flameRB);

	//Set typing
	Com_type flameType;
	flameType.type = Com_type::type::bullet;
	ecs.addComponent<Com_type>(flameID, flameType);

	//Set AABB
	AABB flameAABB;
	ecs.addComponent<AABB>(flameID, flameAABB);

	//Set script
	scriptComponent flameScript;
	//(It used to be a water shot)
	flameScript.scriptContainer["Firebomb"] = ScriptManager.createFromScriptDatabase("Firebomb");
	ecs.addComponent<scriptComponent>(flameID, flameScript);
}

//Do a melee attack
//Similar to airblast
void PlayerController::melee()
{
	const float piovertwo = 1.57079632679f;
	float direction = piovertwo - mouseAngle;

	const float initialSpeed = 600.0f;

	entityID airblastID = ecs.createEntityFromFile("Resources/PresetEntities/MeleeSpatula.json");

	//Initial positioning
	ecs.getComponent<transform2D>(airblastID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 20.0f * cosf(direction); //Apparently not using oTransform fixes this? But the camera is still acting strange.
	ecs.getComponent<transform2D>(airblastID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 20.0f * sinf(direction);
	ecs.getComponent<transform2D>(airblastID).pos.z = 2.0f;//setting z layering

	//Set bullet direction
	ecs.getComponent<transform2D>(airblastID).rotation = Mathf::RadianToDegree(direction);

	//Set force
	//Forces for some reason are inconsistent if they have a short lifetime
	//Force airblastForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(direction), sinf(direction)), 0.01f);
	rigidBody2D airblastRB{};
	airblastRB.vel.x = initialSpeed * cosf(direction) + ecs.getComponent<rigidBody2D>(id).vel.x;
	airblastRB.vel.y = initialSpeed * sinf(direction) + ecs.getComponent<rigidBody2D>(id).vel.y;
	//airblastRB.forceManager.AddLinearForce("AirblastForce", airblastForce, false);
	airblastRB.forceManager.setMaxVel(initialSpeed + 500.0f);
	airblastRB.forceManager.setLinearDrag(0.05f);
	ecs.addComponent<rigidBody2D>(airblastID, airblastRB);

	//Set decay timer
	Com_TimeDestructor airblastTD;
	airblastTD.timeDelayDestructor = 1.0f; //1.0 second lifetime
	ecs.addComponent<Com_TimeDestructor>(airblastID, airblastTD);

	//Set typing
	Com_type airblastType;
	airblastType.type = Com_type::type::bullet;
	ecs.addComponent<Com_type>(airblastID, airblastType);

	//Set AABB
	AABB airblastAABB;
	ecs.addComponent<AABB>(airblastID, airblastAABB);

	//Set script and parameters
	scriptComponent airblastScript;
	//(It used to be a water shot)
	airblastScript.scriptContainer["MeleeSpatula"] = ScriptManager.createFromScriptDatabase("MeleeSpatula");
	ecs.addComponent<scriptComponent>(airblastID, airblastScript);
}

//Damage and stun a certain enemy by a certain amount of health and time
//Stun time addition formula is this:
//stunTime * stunMultiplier / max(enemyHealth after damage, 1)
void PlayerController::damageAndStun(entityID target, int damage, float stunTime)
{
	//Attempt to damage
	if (ecs.entityHas<Com_Health>(target))
	{
		ecs.getComponent<Com_Health>(target).health -= damage;

		//Attempt to stun
		if (ecs.entityHas<scriptComponent>(target))
		{
			auto& sc{ ecs.getComponent <scriptComponent>(target).scriptContainer };
			auto scriptItr{ sc.begin() }; //HACK Hopefully they only have one script
			if (scriptItr != sc.end())
			{
				EnemyBehaviour* enemy = reinterpret_cast<EnemyBehaviour*>((*scriptItr).second.get());
				enemy->stunTimeRemaining += stunTime * enemy->stunMultiplier / max(ecs.getComponent<Com_Health>(target).health, 1);
			}
		}
	}

	//Apply particle effect===============
					// //TODO: set to destory emitter and componet itself to save on mem space
					//if no EC, add EC
	if (!ecs.entityHas<EmitterComponent>(target))
	{
		EmitterComponent EC{};
		EC.emitterContainer.insert(std::make_pair("EmitterExplosionDamaged", std::make_shared<EmitterExplosionDamaged>()));
		ecs.addComponent<EmitterComponent>(target, EC);
	}
	EmitterExplosionDamaged* emi = dynamic_cast<EmitterExplosionDamaged*>(ecs.getComponent<EmitterComponent>(target).emitterContainer["EmitterExplosionDamaged"].get());
	if (emi)
		emi->setTriggered(true);
}

PlayerController::PlayerController(const char* scriptName) :
	scriptBase(scriptName)
{}

void PlayerController::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;

	//create EmitterComponent
	EmitterComponent EC{};
	EC.emitterContainer.insert(std::make_pair("EmitterSuction", std::make_shared<EmitterSuction>()));
	EC.emitterContainer.insert(std::make_pair("EmitterWalkingTrail", std::make_shared<EmitterWalkingTrail>()));
	ecs.addComponent<EmitterComponent>(id, EC);

	//init animation
	auto& sprite = ecs.getComponent<spriteCom>(id);
	sprite.setAnimation("walkRight");
}

int SetActive = 1; //Invulnerability Cheat. Toggle by pressing [G].
void PlayerController::update(float dt)
{
	//Keep attempting to load animations again if couldn't load for the first time
	if (ecs.entityHas<spriteCom>(id) && !ecs.getComponent<spriteCom>(id).animationLoaded)
	{
		ecs.getComponent<spriteCom>(id).setAnimation(ecs.getComponent<spriteCom>(id).animationToLoad);
	}

	//Only process update loop when the game is playing
	if (UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Play)
	{
		return;
	}

	//Update previous direction tracker
	previousFacingDirection = currentFacingDirection;

	//Control Invulnerability
	if (KeyManager.isKeyReleased('G'))
	{
		SetActive = 0;
	}

	//Require these components in the entity for it to even run
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<spriteCom>(id) &&
		ecs.entityHas<Com_Abilities>(id))
	{
		auto& rigidbody = ecs.getComponent<rigidBody2D>(id);
		auto& transform = ecs.getComponent<transform2D>(id);
		auto& sprite = ecs.getComponent<spriteCom>(id);
		auto& ability = ecs.getComponent<Com_Abilities>(id);
		UNREFERENCED_PARAMETER(ability);
		//CHECK DEFEAT
		//if hp of player is lesser than 0, lose
		if (ecs.entityHas<Com_Health>(id) && ecs.getComponent<Com_Health>(id).health <= 0 &&
			currentSkill != skills::death) 
		{
			currentSkill = skills::death;//death animation
			const float pos[] = { 0.f,0.f,0.f };
			//play the corresponding death animation 
			//if (death == false) {
			
			/*for (int i = 0; i < 35; i++)
			{
				SoundClass::getInstance()->Pause(i);
			}*/
			//Sprite to display defeat
			audio.playPannedSoundEffect(audio.sfxLose);
			audio.killChannel(audio.chBGM);
			sprite.speed = 6.0f;
			sprite.setAnimation("death");
		}
		//HANDLE NOT BEING DEFEATED
		if (currentSkill != skills::death)
		{
			//GET MOUSE
			Mathf::vec2 playerToMouse = MouseManager.getWorldPosition() - Mathf::vec2{ transform.pos.x, transform.pos.y };

			//MOVEMENT BY KEYS
			float movementForce{ 0.0f };

			//Change movement force based off current skills
			if (currentSkill == skills::speed)
			{
				//Ultra speed
				if (pressureCurrent >= 4)
					movementForce = ultraSpeedForce;
				else //Super speed
					movementForce = superSpeedForce;
			}
			else
				movementForce = normalMovingForce;

			Mathf::vec2 normalMvtForce{ 0.0f, 0.0f };

			//Testing Rotation
			/*if (KeyManager.isKeyPressed(VK_CONTROL))
			{
				Force finalRotationForce(Force::ForceType::ROTATIONAL_FORCE, { 0.f, 0.f }, dt);
				if (KeyManager.isKeyPressed('J'))
				{
					finalRotationForce.SetTorque(0.5f);
				}
				else if (KeyManager.isKeyPressed('L'))
				{
					finalRotationForce.SetTorque(-0.5f);
				}
				rigidbody.isRotatable = true;
				rigidbody.forceManager.AddRotationalForce("Rotate", finalRotationForce);

				//print data
				//std::cout << transform.rotation << std::endl;
			}*/

			if (currentSkill != skills::death) {
				if (KeyManager.isKeyPressed('W') ||
					KeyManager.isKeyPressed('A') ||
					KeyManager.isKeyPressed('S') ||
					KeyManager.isKeyPressed('D'))
				{
					if (KeyManager.isKeyPressed('W'))
					{
						if (KeyManager.isKeyPressed('A') || KeyManager.isKeyPressed('D'))
							normalMvtForce.y += 0.707f * movementForce; //0.707 ~= sqrt(2)
						else
							normalMvtForce.y += movementForce;
					}
					if (KeyManager.isKeyPressed('S'))
					{
						if (KeyManager.isKeyPressed('A') || KeyManager.isKeyPressed('D'))
							normalMvtForce.y -= 0.707f * movementForce;
						else
							normalMvtForce.y -= movementForce;
					}if (KeyManager.isKeyPressed('A'))
					{
						if (KeyManager.isKeyPressed('W') || KeyManager.isKeyPressed('S'))
							normalMvtForce.x -= 0.707f * movementForce;
						else
							normalMvtForce.x -= movementForce;
					}if (KeyManager.isKeyPressed('D'))
					{
						if (KeyManager.isKeyPressed('W') || KeyManager.isKeyPressed('S'))
							normalMvtForce.x += 0.707f * movementForce;
						else
							normalMvtForce.x += movementForce;
					}

					//particle for walkingTrail
					if (ecs.entityHas<EmitterComponent>(id))
					{
						EmitterWalkingTrail* emi = dynamic_cast<EmitterWalkingTrail*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterWalkingTrail"].get());
						if (emi)
							emi->setTriggered(true, rigidbody.vel, transform.pos);
					}
				}
				else
				{
					//it is not moving, idling 

					//if it entered the current loop
					/*if (prevAction != action::idle) {
						//change animation to idle
						sprite.setAnimation("idle"); //TODO there should be idling in multiple directions
						sprite.speed = 6.0f;
						prevAction = action::idle;
					}*/
					sprite.speed = 0.0f;
					
				}
			}

			if (currentSkill != skills::death)
			{
				//Movement resultant from player's legs
				Force finalMovementForce(Force::ForceType::LINEAR_FORCE, normalMvtForce, dt);
				rigidbody.forceManager.setMaxVel(10000.0f); //Hard limit
				rigidbody.forceManager.setLinearDrag(0.25f);
				//Zero out movement force if going faster than speed limit
				//Speed limit is halved when using certain skills
				float speedLimit = maximumNormalSpeed;
				if (currentSkill == skills::speed)
				{
					if (pressureCurrent == 4) speedLimit = maximumNormalSpeed * 10.0f;
					else speedLimit = maximumNormalSpeed * 2.0f;
				}
				else if (currentSkill == skills::none)
					speedLimit = maximumNormalSpeed;
				else
					speedLimit = maximumNormalSpeed * 0.5f;

				if (Mathf::Length(rigidbody.vel) > speedLimit)
					finalMovementForce.setLinearForce(Mathf::vec2{});

				rigidbody.forceManager.AddLinearForce("movement", finalMovementForce);

				//ANIMATION
			}

			//Facing based off mouse position
			if (playerToMouse.y > 0.0f)
			{
				if (abs(playerToMouse.y) > abs(playerToMouse.x))
					currentFacingDirection = facing::up;
				else if (playerToMouse.x > 0.0f)
					currentFacingDirection = facing::right;
				else
					currentFacingDirection = facing::left;
			}
			else
			{
				if (abs(playerToMouse.y) > abs(playerToMouse.x))
					currentFacingDirection = facing::down;
				else if (playerToMouse.x > 0.0f)
					currentFacingDirection = facing::right;
				else
					currentFacingDirection = facing::left;
			}


			//Sprite to display based off facing
			//Only replay when direction has changed
			//OR if character was previously idle
			if (currentFacingDirection != previousFacingDirection &&
				currentSkill != skills::death)
			{
				switch (currentFacingDirection)
				{
				case (facing::up):
					sprite.setAnimation("walkUp");
					break;
				case (facing::right):
					sprite.setAnimation("walkRight");
					break;
				case (facing::left):
					sprite.setAnimation("walkLeft");
					break;
				case (facing::down):
					sprite.setAnimation("walkDown");
					break;
				}
			}

			if (!KeyManager.isKeyPressed('W') &&
				!KeyManager.isKeyPressed('A') &&
				!KeyManager.isKeyPressed('S') &&
				!KeyManager.isKeyPressed('D'))
			{
				//No movement, reset animation
				//sprite.currentFrame = 0;
				sprite.speed = 0.0f;
			}
			else
			{
				//Change framerate based on how fast the character is going
				sprite.speed = Mathf::Length(rigidbody.vel) * 0.1f;

				float pos[] = { 0,0,0 };
				//Footsteps sound effect
				if (sprite.currentFrame != 0)
				{
					footstepSounded = false;
				}
				else
				{
					if (footstepSounded == false)
					{
						if (std::rand() < RAND_MAX / 2)
						{
							audio.playPannedSoundEffect(audio.sfxPlayerFootstep1);
							audio.playPannedSoundEffect(audio.sfxMetalCrunch, &audio.chSFX, 0.0f, 0.1f, 0.5f);
						}
						else
						{
							audio.playPannedSoundEffect(audio.sfxPlayerFootstep2);
							audio.playPannedSoundEffect(audio.sfxMetalCrunch, &audio.chSFX, 0.0f, 0.1f, 0.5f);
						}
						footstepSounded = true;
					}
				}
			}

			//GET MOUSE POSITIONING////////////////////////////////////////////////
			//Mathf::vec2 up = { 0.0f, 1.0f };

			//Get mouse mouseAngle wrt player (radians)
			if (playerToMouse.x > 0.0f)
			{
				if (playerToMouse.y >= 0.0f) //Top right
					mouseAngle = atan(playerToMouse.x / playerToMouse.y);
				else //Bottom right
					mouseAngle = 3.141592654f - atan(playerToMouse.x / -playerToMouse.y);
			}
			else
			{
				if (playerToMouse.y >= 0.0f) //Top left
					mouseAngle = 6.283185307f - atan(-playerToMouse.x / playerToMouse.y);
				else //Bottom left
					mouseAngle = 3.141592654f + atan(playerToMouse.x / playerToMouse.y);
			}

			//HANDLE SKILL INPUTS//////////////////////////////////////////////////
			//Keybinds:
			//LMB    = Vacuum in. Gain Pressure
			//         (Upgrades from 1 to 4 Pressure)
			//V      = Melee attack. No Pressure change
			//RMB    = Airblast out. Use all Pressure
			//         (1 Pressure = 1 very slow shot of wind, 1 damage, short stun)
			//         (2 Pressure = 2 faster shots of wind, 2 damage each, medium stun)
			//         (3 Pressure = 3 very fast shots of wind, 3 damage each, long stun)
			//         (4 Pressure = A tornado that lasts for a few seconds)
			//LShift = Super Speed. Use all Pressure
			//         (1 Pressure = 0.5s, weak burst of speed)
			//         (2 Pressure = 1.2s, weak burst of speed)
			//         (3 Pressure = 2.0s, weak burst of speed)
			//         (4 Pressure = 0.2s, very strong burst of speed, pass through enemies)
			//Q      = Energy Shield. Use all Pressure
			// 	       (1 Pressure = 0.5s, deflects contacted projectiles & enemies)
			// 	       (2 Pressure = 1.2s, deflects contacted projectiles & enemies)
			// 	       (3 Pressure = 2.0s, deflects contacted projectiles & enemies)
			// 	       (4 Pressure = No shield, deflects NEARBY projectiles & enemies)
			//RMB(E) = Flamethrower. Use all Pressure
			//         (1 Pressure = 2 shots of flames)
			//         (2 Pressure = 5 shots of flames)
			//         (3 Pressure = 9 shots of flames)
			//         (4 Pressure = A firebomb that bursts into 18 flames around)

			//updates the ui based on the pressure 
			for (auto const& e : ecs.getEntityNames()) {
				if (ecs.entityHas<scriptComponent>(e.first)) {
					auto& UIscript{ ecs.getComponent<scriptComponent>(e.first).scriptContainer };
					auto UIIter{ UIscript.find("HUDUIScript") };
					if (UIIter != UIscript.end()) {
						//increment the count of the number of tomatos 
						HUDUIScript* pressureCount = reinterpret_cast<HUDUIScript*>((*UIIter).second.get());
						switch (pressureCurrent)
						{
						case 0:
							pressureCount->stage = HUDUIScript::airBlastStages::none;
							break;
						case 1:
							pressureCount->stage = HUDUIScript::airBlastStages::first;
							break;
						case 2:
							pressureCount->stage = HUDUIScript::airBlastStages::second;
							break;
						case 3:
							pressureCount->stage = HUDUIScript::airBlastStages::third;
							break;
						case 4:
							pressureCount->stage = HUDUIScript::airBlastStages::fourth;
							break;
						default:
							pressureCount->stage = HUDUIScript::airBlastStages::none;
							break;
						}
					}
				}
			}

			//Handle glowing flash
			if (ecs.entityHas<material>(id))
			{
				if (flashTimeRemaining > 0.0f)
				{
					flashTimeRemaining -= dt;

					if (trueAirblastFalseFlamethrower) //Using air blast
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{1.0f + flashTimeRemaining * 3.0f, 1.0f + flashTimeRemaining * 3.0f, 1.0f + flashTimeRemaining * 3.0f, 1.0f};
					}
					else //Using flamethrower
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{1.0f + flashTimeRemaining * 5.0f, 1.0f + flashTimeRemaining * 1.0f, 1.0f, 1.0f };
					}
				}
			}
			

			//Skill input can only be done if no other skill is being done
			//AND if not paused by dialogue or pause menu
			if (currentSkill == skills::none &&
				UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
			{
				//Countdown for free dash cooldown
				freeDashCooldownRemaining -= dt;

				//E - Toggle between airblast and flamethrower
				/*if (!UpgradeManager::GetInstance().upgradeFireBall)
				{
					//Prevent player from actually having the flamethrower equipped if not unlocked
					trueAirblastFalseFlamethrower = true;
				}*/
				if (UpgradeManager::GetInstance().upgradeFireBall &&
					KeyManager.isKeyTriggered('E'))
				{
					audio.playPannedSoundEffect(audio.sfxGunClick, &audio.chSFX, 0.0f, 3.0f, 1.0f + (float)trueAirblastFalseFlamethrower);
					audio.playPannedSoundEffect(audio.sfxSwitch, &audio.chSFX, 0.0f, 3.0f, 1.0f + (float)trueAirblastFalseFlamethrower);
					trueAirblastFalseFlamethrower = !trueAirblastFalseFlamethrower;
					flashTimeRemaining = flashTime;
				}

				//HOLD LMB - Vacuum in (0.5 seconds for every successful pressure)
				//Requires pressure to be less than maximum
				//(7 Apr) It was suggested by lecturer JY that you should
				//        sill be able to operate the vacuum at maximum pressure
				if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON))
				{
					currentSkill = skills::vacuum;

					audio.playPannedSoundEffect(audio.sfxPlayerVacuumStart);

					skillTimeRemaining = 0.5f; //TODO Variable skill time?
				}
				//V - Melee attack
				//Can be continuous, hence KeyPressed
				else if (KeyManager.isKeyPressed('V'))
				{
					currentSkill = skills::melee;
					skillTimeRemaining = 1.0f; //TODO Variable
					audio.playPannedSoundEffect(audio.sfxPlayerMelee);
					melee();
				}
				//RMB - Airblast out
				//Requires pressure to be more than 0
				else if (trueAirblastFalseFlamethrower &&
					MouseManager.isButtonTriggered(MouseButton::RIGHT_BUTTON) &&
					pressureCurrent > 0)
				{
					currentSkill = skills::airblast;
					//Four different pressures for different effects
					audio.playPannedSoundEffect(audio.sfxAirblastNew, &audio.chSFX, 0.0f, 0.3f * (float)pressureCurrent, 4.0f - 0.8f * (float)pressureCurrent);
					switch (pressureCurrent)
					{
					case 1:
					{
						// (1 Pressure = 1 very slow shot of wind, 1 damage, short stun)
						shootAirblasts(1, 0.0f, 1000.0f, 1, 2.0f);
						skillTimeRemaining = 1.0f;
						//Push player around
						//rigidbody.vel.x -= 500.0f * cosf(1.570796327f - mouseAngle);
						//rigidbody.vel.y -= 500.0f * sinf(1.570796327f - mouseAngle);
						break;
					}
					case 2:
						// (2 Pressure = 3 faster shots of wind, 1 damage each, medium stun)
						shootAirblasts(3, 0.1f, 1600.0f, 1, 3.0f);
						skillTimeRemaining = 1.0f;
						//rigidbody.vel.x -= 1000.0f * cosf(1.570796327f - mouseAngle);
						//rigidbody.vel.y -= 1000.0f * sinf(1.570796327f - mouseAngle);
						break;
					case 3:
						// (3 Pressure = 5 very fast shots of wind, 2 damage each, long stun)
						shootAirblasts(5, 0.2f, 2400.0f, 1, 4.0f);
						skillTimeRemaining = 1.0f;
						//rigidbody.vel.x -= 1500.0f * cosf(1.570796327f - mouseAngle);
						//rigidbody.vel.y -= 1500.0f * sinf(1.570796327f - mouseAngle);
						break;
					case 4:
						// (4 Pressure = Summon a tornado that lasts for a few seconds)
						// (Said tornado pushes enemies back and stuns them for long)
						// (Said tornado ALSO shoots out fast shots of wind in random directions)
						//TODO add tornado soon
						shootTornado();
						skillTimeRemaining = 1.0f;
						//rigidbody.vel.x -= 2000.0f * cosf(1.570796327f - mouseAngle);
						//rigidbody.vel.y -= 2000.0f * sinf(1.570796327f - mouseAngle);
						break;
					}
					//Set pressure to 0
					pressureCurrent = 0;
					//std::cout << "(517) PRESSURE: " << pressureCurrent << " / " << pressureLimit << std::endl;
					skillTimeRemaining = 1.0f; //TODO Variable
				}
				//LShift - Super speed
				//(Note if pressure is 0, a very short dash of 0.25s is done, with 1.0s of interval)
				//Requires pressure to be more than 0
				//Requires the ability to be unlocked
				//TODO Left shift doesn't seem to register?
				else if (UpgradeManager::GetInstance().upgradeSuperSpeed &&
					KeyManager.isKeyTriggered(VK_SHIFT))
				{
					currentSkill = skills::speed;
					//std::cout << "Begin charging\n";
					//TODO four different pressures for different effects
					switch (pressureCurrent)
					{
					case 0:
						// (0 Pressure = 0.2s of small speed burst, 1 second of interval)
						if (freeDashCooldownRemaining <= 0.0f)
						{
							audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.5f + (float)pressureCurrent, 1.0f + 0.2f * (float)pressureCurrent);
							skillTimeRemaining = 0.2f;
							freeDashCooldownRemaining = freeDashDelay;
						}
						break;
					case 1:
						// (1 Pressure = 0.5s of small speed burst)
						audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.5f + (float)pressureCurrent, 1.0f + 0.2f * (float)pressureCurrent);
						skillTimeRemaining = 0.5f;
						break;
					case 2:
						// (2 Pressure = 1.2s of small speed burst)
						audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.5f + (float)pressureCurrent, 1.0f + 0.2f * (float)pressureCurrent);
						skillTimeRemaining = 1.2f;
						break;
					case 3:
						// (3 Pressure = 2.0s of small speed burst)
						audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.5f + (float)pressureCurrent, 1.0f + 0.2f * (float)pressureCurrent);
						skillTimeRemaining = 2.0f;
						break;
					case 4:
						// (4 Pressure = 0.2s of ULTRA speed burst)
						// (Kettle Knight will charge, greatly damage and stun whoever is in the way without stopping)
						audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.5f + (float)pressureCurrent, 1.0f + 0.2f * (float)pressureCurrent);
						audio.playPannedSoundEffect(audio.sfxLaserBoom);
						skillTimeRemaining = 0.2f;
						break;
					}
					//DO NOT Set pressure to 0
					//Current pressure value needs to be used for determining if
					//super speed or ULTRA speed is being used
					//Pressure will be set to 0 at the end of skill
					//End of skill being:
					//Skill time runs out OR
					//Collision is made (at 3 or less pressure)
					//pressureCurrent = 0;

					//Reset vector
					enemiesHit = std::vector<entityID>();
				}
				//Q - Energy Shield
				//Requires pressure to be more than 0
				//Requires the ability to be unlocked
				else if (UpgradeManager::GetInstance().upgradeSuperShield &&
					KeyManager.isKeyTriggered('Q') &&
					pressureCurrent > 0)
				{
					currentSkill = skills::shield;
					//TODO four different pressures for different effects
					switch (pressureCurrent)
					{
					case 1:
						// (1 Pressure = 0.5s of a deflecting shield)
						audio.playPannedSoundEffect(audio.sfxTeleporterDrone, &audio.chSFX, 0.0f, 1.0f, 4.0f);
						skillTimeRemaining = 0.5f;
						break;
					case 2:
						// (2 Pressure = 1.2s of a deflecting shield)
						audio.playPannedSoundEffect(audio.sfxTeleporterDrone, &audio.chSFX, 0.0f, 1.0f, 1.67f);
						skillTimeRemaining = 1.2f;
						break;
					case 3:
						// (3 Pressure = 2.0s of a deflecting shield)
						audio.playPannedSoundEffect(audio.sfxTeleporterDrone, &audio.chSFX, 0.0f, 1.0f, 1.0f);
						skillTimeRemaining = 2.0f;
						break;
					case 4:
						// (4 Pressure = An instant flash of energy)
						// (Vaporises all nearby projectiles, damages and stuns all nearby enemies)
						audio.playPannedSoundEffect(audio.sfxLaserBoom);
						flashTimeRemaining = flashTime * 5.0f;
						
						//Stun nearby enemies & destroy enemy projectiles
						std::vector<entityID> toDelete{};
						for (auto e : ecs.getEntityNames())
						{
							if (ecs.entityHas<Com_type>(e.first))
							{
								//Enemies
								if (ecs.getComponent<Com_type>(e.first).type == Com_type::type::enemy || ecs.getComponent<Com_type>(e.first).type == Com_type::type::enemyrange)
								{
									if (ecs.entityHas<transform2D>(e.first))
									{
										if (Mathf::Length(ecs.getComponent<transform2D>(e.first).pos - transform.pos) < 600.0f)
										{
											damageAndStun(e.first, 3, 3.0f);
											if (ecs.entityHas<rigidBody2D>(e.first))
											{
												ecs.getComponent<rigidBody2D>(e.first).vel.x += 300.0f * (ecs.getComponent<transform2D>(e.first).pos.x - transform.pos.x);
												ecs.getComponent<rigidBody2D>(e.first).vel.y += 300.0f * (ecs.getComponent<transform2D>(e.first).pos.y - transform.pos.y);
											}
										}
									}
								}
								//Enemy projectiles
								if (ecs.getComponent<Com_type>(e.first).type == Com_type::type::enemyBullet)
								{
									if (ecs.entityHas<transform2D>(e.first))
									{
										if (Mathf::Length(ecs.getComponent<transform2D>(e.first).pos - transform.pos) < 600.0f)
										{
											toDelete.emplace_back(e.first);
										}
									}
								}
							}
						}
						for (auto e : toDelete)
							ecs.deleteEntity(e);
						break;
					}
					//Set pressure to 0
					pressureCurrent = 0;
					//std::cout << "(592) PRESSURE: " << pressureCurrent << " / " << pressureLimit << std::endl;
				}
				//RMB - Flamethrower
				//Requires pressure to be more than 0
				//Requires the ability to be unlocked
				else if (UpgradeManager::GetInstance().upgradeFireBall &&
					!trueAirblastFalseFlamethrower &&
					MouseManager.isButtonTriggered(MouseButton::RIGHT_BUTTON) &&
					pressureCurrent > 0)
				{
					currentSkill = skills::flamethrower;
					//TODO four different pressures for different effects
					switch (pressureCurrent)
					{
					case 1:
						// (1 Pressure = 2 shots over 1 second)
						flamethrowerShotsRemaining = 2;
						skillTimeRemaining = 1.0f;
						break;
					case 2:
						// (2 Pressure = 5 shots over 1 second)
						flamethrowerShotsRemaining = 5;
						skillTimeRemaining = 1.0f;
						break;
					case 3:
						// (3 Pressure = 9 shots over 1 second)
						flamethrowerShotsRemaining = 9;
						skillTimeRemaining = 1.0f;
						break;
					case 4:
						// (4 Pressure = A firebomb)
						// (This firebomb explodes into 18 shots upon contact)
						audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 1.0f, 0.3f);
						shootFirebomb();
						flashTimeRemaining = flashTime;
						skillTimeRemaining = 1.0f;
						break;
					}
					//Don't set pressure to 0
					//pressureCurrent = 0;
					//Prepare to shoot
					timeTillNextFlamethrowerShot = 0.0f;
				}
			}

			//HANDLE ONGOING SKILLS////////////////////////////////////////////////

			//Check if character was damaged over this last frame
			//Double confirm that if using shield, deny any incoming damage
			//If damaged, stop skill
			if (ecs.entityHas<Com_Health>(id))
			{
				if (ecs.getComponent<Com_Health>(id).health < lastFrameHealth)
				{
					currentSkill = skills::none;
					skillTimeRemaining = 0.0f;
				}
				//Update health checker
				lastFrameHealth = ecs.getComponent<Com_Health>(id).health;
			}

			//Vacuuming in (Hold)
			//Every one second of successful sucking, accumulate, pressure.
			if (currentSkill == skills::vacuum)
			{

				//Stop when LMB off or pressure maxed
				if (!MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON))
				{
					currentSkill = skills::none;
					audio.playPannedSoundEffect(audio.sfxPlayerVacuumEnd);
				}

				float pos[] = { 0,0,0 };
				skillTimeRemaining -= dt;
				//Countdown skill timer
				if (skillTimeRemaining <= 0.0f)
				{
					skillTimeRemaining = 0.5f; //TODO variable suction time
					++pressureCurrent;
					if (pressureCurrent > pressureLimit) pressureCurrent = pressureLimit;
					flashTimeRemaining = flashTime * 0.5f * pressureCurrent; //Blink VFX
					//play "accumulating" sound
					audio.playPannedSoundEffect(audio.sfxPlayerVacuumLoop, &audio.chSFX, 0.0f, 1.0f, 1.0f);
					audio.playPannedSoundEffect(audio.sfxPew, &audio.chSFX, 0.0f, 1.0f, 0.75f + 0.25f * (float)pressureCurrent);
				}

				//Constants
				//TODO have them be variable
				constexpr float arc_angle = 3.141592654f * 0.125f; //for one side of the coverage arc
				constexpr float suctionLimit = 250.0f; //Maximum range at which suction takes effect
				constexpr float suctionKillLimit = 120.0f; //Maximum range at which suction can suck in enemies
				constexpr float suctionStrength = 50.0f;

				//Apply suction
				//PhysicsSystem->suction(mouseAngle, tc, e, arc_angle, suctionLimit, suctionKillLimit, suctionStrenth);
				float cosRot = cos(mouseAngle);
				float sinRot = sin(mouseAngle);
				Mathf::vec3 targetVector{ 0.0f, suctionLimit, 0.0f }; //Need to form a vec towards orientation
				Mathf::Matrix3 rotate{
					cosRot, sinRot, 0.0f,
					-sinRot, cosRot, 0.0f,
					0.0f, 0.0f, 1.0f
				};
				targetVector = rotate * targetVector;
				Mathf::vec3 targetPosTemp = transform.pos + targetVector;
				Mathf::vec2 targetPos = { targetPosTemp.x, targetPosTemp.y };

				Mathf::vec3 killVector{ 0.0f, suctionKillLimit, 0.0f }; //Form a vec towards orientation
				killVector = rotate * killVector;
				Mathf::vec3 killPosTemp = transform.pos + killVector;
				Mathf::vec2 killPos = { killPosTemp.x, killPosTemp.y };

				//adds sucking in particle effect
				if (ecs.entityHas<EmitterComponent>(id))
				{
					EmitterSuction* emi = dynamic_cast<EmitterSuction*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterSuction"].get());
					if (emi)
						emi->setSuctionTriggered(true, mouseAngle, arc_angle, suctionLimit);
				}

				std::vector<entityID> toDelete{};
				for (auto const& e : ecs.getEntityNames())
				{
					if (e.first == id) //do not consider self
						continue;
					//May check other entities in the future

					//Check if it's an enemy in suction
					if (ecs.entityHas<transform2D>(e.first) &&
						ecs.entityHas<Com_Health>(e.first) &&
						ecs.entityHas<rigidBody2D>(e.first))
					{
						auto& targetTransform = ecs.getComponent<transform2D>(e.first);
						auto& targetHealth = ecs.getComponent<Com_Health>(e.first);
						auto& targetRigidBody = ecs.getComponent<rigidBody2D>(e.first);

						if (!checkIfInSuctionArc(transform, targetTransform, targetVector, arc_angle))
							continue;

						if (targetHealth.isActive == false)
							continue;

						//An enemy is being sucked in. Make them look like they are sucked in
						if (ecs.entityHas<scriptComponent>(e.first))
						{
							auto& sc{ ecs.getComponent <scriptComponent>(e.first).scriptContainer };
							auto scriptItr{ sc.begin() }; //HACK hope that the enemy script is the only script
							if (scriptItr != sc.end())
							{
								//Set flag that the enemy is being sucked in.
								//Whether they can get sucked in is dependant on enemy's current state (see respective enemy logic file)
								EnemyBehaviour* enemy = reinterpret_cast<EnemyBehaviour*>((*scriptItr).second.get());
								enemy->beingSucked = true;

								if (enemy->suctionSuccessful)
								{
									//Check if in range to vacuum in
									Mathf::Vec2 suckingVector = targetTransform.pos - transform.pos;
									if (checkIfInSuctionArc(transform, targetTransform, killVector, arc_angle))
									{
										//Mushroom boss constitutes to five mushrooms
										if (e.second == "MushroomBoss")
										{
											if (ecs.entityHas<controller>(id) && ecs.entityHas<AABB>(e.first)) {
												ecs.getComponent<controller>(id).numOfMushroomsCaptured += 5;//incrementing the number of mushrooms captured
												AssetManager::GetInstance().mushroomsCollected += 5;
											}
										}

										//if chillies are sucked in 
										if (e.second == "ChilliNewer") {
											if (ecs.entityHas<controller>(id) && ecs.entityHas<AABB>(e.first)) {
												AssetManager::GetInstance().chilliesCollected++;//increment the number of chillies collected 
											}
										}

										if (e.second == "MushroomMole") {
											//if the player entity has the abilities component
											//HACK bandaid fix to check whether the enemy's AABB still exist
											if (ecs.entityHas<controller>(id) && ecs.entityHas<AABB>(e.first)) {
												ecs.getComponent<controller>(id).numOfMushroomsCaptured++;//incrementing the number of mushrooms captured
												AssetManager::GetInstance().mushroomsCollected++;
											}
										}


										//If tomato sucked in, add to score
										if (e.second == "TomatoNew") {
											//if the player entity has the abilities component
											//HACK bandaid fix to check whether the enemy's AABB still exist
											if (ecs.entityHas<controller>(id) && ecs.entityHas<AABB>(e.first)) {
												ecs.getComponent<controller>(id).numOfTomatoesCaptured++;//incrementing the number of tomatoes captured
												AssetManager::GetInstance().tomatoesCollected++;
											}
										}

										//Push enemy in the to destroy vector
										toDelete.emplace_back(e.first);

										audio.playPannedSoundEffect(audio.sfxPop);
										audio.playPannedSoundEffect(audio.sfxSuction1);

										continue;
									}

									//TODO ray casting to not pull enemies blocked by obstacles

									//Pull in
									Force suctionForce{ Force::ForceType::LINEAR_FORCE, -suckingVector * suctionStrength, 1000 };
									targetRigidBody.forceManager.AddLinearForce("Suction", suctionForce);
								}
							}
						}
					}

					if (ecs.entityHas<Com_type>(e.first))
					{
						//Bush -> suck in
						if (Com_type::bush == ecs.getComponent<Com_type>(e.first).type)
						{
							auto& targetTransform = ecs.getComponent<transform2D>(e.first);

							if (!checkIfInSuctionArc(transform, targetTransform, targetVector, arc_angle))
								continue;

							//set bush to be sucked mode
							if (ecs.entityHas<scriptComponent>(e.first))
							{
								auto script = ecs.getComponent<scriptComponent>(e.first).scriptContainer.find("BushScript");
								dynamic_cast<BushScript*>(script->second.get())->InSuctionRange = true;
								//dynamic_cast<BushScript*>(script->second.get())->_state = BushScript::BUSH_STATE::BeingSucked_Floating;
								dynamic_cast<BushScript*>(script->second.get())->BushToPlayer = ecs.getComponent<transform2D>(id).pos - ecs.getComponent<transform2D>(e.first).pos;
								if (ecs.entityHas<rigidBody2D>(e.first))
								{
									ecs.getComponent<rigidBody2D>(e.first).Type = rigidBody2D::Types::Dynamics;
								}
							}
						}
						else if (Com_type::blockage == ecs.getComponent<Com_type>(e.first).type)
						{
							auto& targetTransform = ecs.getComponent<transform2D>(e.first);

							if (!checkIfInSuctionArc(transform, targetTransform, targetVector, arc_angle))
								continue;

							//set bush to be sucked mode
							if (ecs.entityHas<scriptComponent>(e.first))
							{
								auto script = ecs.getComponent<scriptComponent>(e.first).scriptContainer.find("BlockageScript");
								dynamic_cast<BlockageScript*>(script->second.get())->InSuctionRange = true;
								dynamic_cast<BlockageScript*>(script->second.get())->BushToPlayer = ecs.getComponent<transform2D>(id).pos - ecs.getComponent<transform2D>(e.first).pos;
								if (ecs.entityHas<rigidBody2D>(e.first))
								{
									ecs.getComponent<rigidBody2D>(e.first).Type = rigidBody2D::Types::Dynamics;
								}
							}
						}

					}
				}
				for (entityID e : toDelete)
				{
					ecs.deleteEntity(e);
				}
			}
			else
			{
				//SoundClass::getInstance()->Pause(suction);
			}

			//Melee
			if (currentSkill == skills::melee)
			{
				skillTimeRemaining -= dt;
				//Countdown skill timer
				if (skillTimeRemaining <= 0.0f)
				{
					currentSkill = skills::none;
				}

				//Check melee hitbox if it hits enemy
			}
			else
			{
				//Immediately delete melee hitbox
			}

			//Airblast
			if (currentSkill == skills::airblast)
			{
				skillTimeRemaining -= dt;
				if (skillTimeRemaining <= 0.0f)
				{
					currentSkill = skills::none;
				}
			}

			//Super Speed
			if (currentSkill == skills::speed)
			{
				skillTimeRemaining -= dt;
				if (skillTimeRemaining <= 0.0f)
				{
					if (ecs.entityHas<material>(id))
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
					currentSkill = skills::none;
					pressureCurrent = 0;
					//std::cout << "(838) PRESSURE: " << pressureCurrent << " / " << pressureLimit << std::endl;
				}

				//Glow when charging
				if (pressureCurrent == 4)
				{
					if (ecs.entityHas<material>(id))
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 2.0f, 2.0f, 2.0f, 1.0f }; //brighter glow
					}
				}
				else if (pressureCurrent > 0)
				{
					if (ecs.entityHas<material>(id))
					{
						ecs.getComponent<material>(id).color = Mathf::Vector4{ 1.5f, 1.5f, 1.5f, 1.0f };
					}
				}

				//Only start checking for super speed conditions if above
				//normal speed limit
				//AND not using the free charge
				if (Mathf::Length(rigidbody.vel) > maximumNormalSpeed && pressureCurrent > 0)
				{
					//TODO add a trail behind Kettle Knight

					//Should the player collide into anything...
					std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
					if (!collidedEntities.empty())
					{
						for (auto entityCollided : collidedEntities)
						{
							//If collided with enemy, damage AND stun them
							if (ecs.entityHas<Com_type>(entityCollided) &&
								ecs.entityHas<Com_Health>(entityCollided) &&
								ecs.entityHas<rigidBody2D>(entityCollided) &&
								ecs.entityHas<transform2D>(entityCollided) &&
								ecs.entityHas<scriptComponent>(entityCollided))
							{
								if (std::find(enemiesHit.begin(), enemiesHit.end(), entityCollided) == enemiesHit.end())
								{
									if (pressureCurrent == 4)
									{
										audio.playPannedSoundEffect(audio.sfxExplosion, &audio.chSFX, 0.0f, 0.5f, 3.0f);
										damageAndStun(entityCollided, 5, 5.0f);
										Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
										ecs.getComponent<rigidBody2D>(entityCollided).vel += diffVec * 5000.0f / ecs.getComponent<rigidBody2D>(entityCollided).Mass; //HACK use mass directly instead of adding a force.
									}
									else
									{
										audio.playPannedSoundEffect(audio.sfxExplosion, &audio.chSFX, 0.0f, 0.5f, 3.0f);
										//Push them back
										Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
										ecs.getComponent<rigidBody2D>(entityCollided).vel += diffVec * 3000.0f / ecs.getComponent<rigidBody2D>(entityCollided).Mass; //HACK use mass directly instead of adding a force.
										damageAndStun(entityCollided, 1, 1.0f);
									}
									enemiesHit.emplace_back(entityCollided);
								}
							}
						}

						//Stop charging if not 4 pressure
						if (pressureCurrent < 4)
						{
							currentSkill = skills::none;
							skillTimeRemaining = 0.0f;
							pressureCurrent = 0;
							//std::cout << "(876) PRESSURE: " << pressureCurrent << " / " << pressureLimit << std::endl;
						}
					}
				}
			}

			//Energy Shield
			//Only displays the shield
			//Try making damage prevention code in parts where damage would have been dealt otherwise
			if (currentSkill == skills::shield)
			{
				skillTimeRemaining -= dt;
				//Countdown skill timer
				if (skillTimeRemaining <= 0.0f)
				{
					currentSkill = skills::none;
				}

				//Display a shield
				if (!shieldLoaded)
				{
					shieldID = ecs.createEntityFromFile("Resources/PresetEntities/shield.json");
					ecs.setEntityParent(shieldID, id); //Shield tracks player
					shieldLoaded = true;
				}
				else
				{
					if (ecs.entityHas<spriteCom>(shieldID) &&
						ecs.entityHas<material>(shieldID))
					{
						//Display
						ecs.getComponent<spriteCom>(shieldID).isVisible = true;

						//Quiver in brightness
						//The higher the multiplier in the sin equation, the faster the quiver
						float brightness = 0.75f + (sinf(25.0f * skillTimeRemaining)) / 4.0f;
						material& shieldMat = ecs.getComponent<material>(shieldID);
						shieldMat.color.x = brightness;
						shieldMat.color.y = brightness;
						shieldMat.color.z = brightness;
						shieldMat.color.w = skillTimeRemaining;
					}
				}

				//TODO display shielding particles

				//Deflect and stun contact enemies
				std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
				if (!collidedEntities.empty())
				{
					for (auto entityCollided : collidedEntities)
					{
						//If collided with enemy, push, damage AND stun them
						if (ecs.entityHas<Com_type>(entityCollided) &&
							ecs.entityHas<Com_Health>(entityCollided) &&
							ecs.entityHas<rigidBody2D>(entityCollided) &&
							ecs.entityHas<transform2D>(entityCollided) &&
							ecs.entityHas<scriptComponent>(entityCollided))
						{
							//Push
							Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
							ecs.getComponent<rigidBody2D>(entityCollided).vel += diffVec * 1000.0f / ecs.getComponent<rigidBody2D>(entityCollided).Mass; //HACK use mass directly instead of adding a force.

							audio.playPannedSoundEffect(audio.sfxLaserBoom, &audio.chSFX, 0.0f, 0.5f, 2.0f);
							damageAndStun(entityCollided, (int)skillTimeRemaining, skillTimeRemaining);
							skillTimeRemaining = 0.0f;
						}
					}
				}
			}
			else
			{
				//Hide shield when not active
				if (shieldLoaded && ecs.entityHas<spriteCom>(shieldID))
				{
					ecs.getComponent<spriteCom>(shieldID).isVisible = false;
				}
			}

			//Flamethrower
			if (currentSkill == skills::flamethrower)
			{
				skillTimeRemaining -= dt;
				//Countdown skill timer
				if (skillTimeRemaining <= 0.0f)
				{
					currentSkill = skills::none;
					pressureCurrent = 0;
					//std::cout << "(957) PRESSURE: " << pressureCurrent << " / " << pressureLimit << std::endl;
				}

				//Countdown flamethrower shot
				timeTillNextFlamethrowerShot -= dt;

				//Shoot out a burst of flame
				if (timeTillNextFlamethrowerShot <= 0.0f)
				{
					switch (pressureCurrent)
					{
						//2 shots, spaced 0.3 seconds apart
					case 1:
					{
						if (flamethrowerShotsRemaining > 0)
						{
							shootFlame(0.05f - 0.1f * (float)std::rand() / (float)RAND_MAX, 500.0f);
							flashTimeRemaining = flashTime * 0.2f;
							audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.8f, 0.6f);
							--flamethrowerShotsRemaining;
							timeTillNextFlamethrowerShot += 0.3f;
						}
						break;
					}
					//5 shots, spaced 0.15 seconds apart
					case 2:
					{
						if (flamethrowerShotsRemaining > 0)
						{
							shootFlame(0.1f - 0.2f * (float)std::rand() / (float)RAND_MAX, 700.0f);
							flashTimeRemaining = flashTime * 0.2f;
							audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.8f, 0.6f);
							--flamethrowerShotsRemaining;
							timeTillNextFlamethrowerShot += 0.15f;
						}
						break;
					}
					//9 shots, spaced 0.08 seconds apart
					case 3:
					{
						if (flamethrowerShotsRemaining > 0)
						{
							shootFlame(0.2f - 0.4f * (float)std::rand() / (float)RAND_MAX, 900.0f);
							flashTimeRemaining = flashTime * 0.2f;
							audio.playPannedSoundEffect(audio.sfxWhoosh1, &audio.chSFX, 0.0f, 0.8f, 0.6f);
							--flamethrowerShotsRemaining;
							timeTillNextFlamethrowerShot += 0.08f;
						}
						break;
					}
					//Do note that 4 power version does not involve shooting flames directly
					}
				}
			}

			//HEALING CHEAT
			if (KeyManager.isKeyTriggered('O'))
			{
				if (ecs.entityHas<Com_Health>(id))
					ecs.getComponent<Com_Health>(id).health = 100;
			}

			//WIN CHEAT
			if (KeyManager.isKeyTriggered('I'))
			{
				ecs.loadLevel("Resources/SavedLevels/EndPart1Level.json");
				LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
			}

			//SKIP CURRENT LEVEL 
			if (KeyManager.isKeyTriggered('J'))
			{
				//check current level 
				if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level1 && !UpgradeManager::GetInstance().level1Completed) {
					UpgradeManager::GetInstance().level1Completed = true;
				}else if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level2 && !UpgradeManager::GetInstance().level2Completed) {
					UpgradeManager::GetInstance().level2Completed = true;
				}
				AssetManager::GetInstance().tomatoesCollected += 5;
				AssetManager::GetInstance().mushroomsCollected += 5;
				AssetManager::GetInstance().chilliesCollected += 5;
				ecs.loadLevel("Resources/SavedLevels/ThroneRoomLevel.json");
				LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
			}

			//LOSE CHEAT
			if (KeyManager.isKeyTriggered('L'))
			{
				if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level1 && UpgradeManager::GetInstance().level1Completed) {
					ecs.loadLevel("Resources/SavedLevels/LoseScreenPart2Level1Level.json");
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
				}
				else if(UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level1 && !UpgradeManager::GetInstance().level1Completed) {
					ecs.loadLevel("Resources/SavedLevels/LoseScreenLevel.json");
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
				}

				if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level2 && UpgradeManager::GetInstance().level2Completed) {
					ecs.loadLevel("Resources/SavedLevels/LoseScreenPart2Level2Level.json");
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
				}
				else if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level2 && !UpgradeManager::GetInstance().level2Completed) {
					ecs.loadLevel("Resources/SavedLevels/LoseScreen2Level.json");
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
				}
			}

			//god mode
			static bool godMode = false;
			if (KeyManager.isKeyTriggered('G')) {
				godMode = !godMode;
			}
			if (godMode) {
				//always full health 
				if (ecs.entityHas<Com_Health>(id))
					ecs.getComponent<Com_Health>(id).health = 100;

				//always on full skill mode 
				pressureCurrent = 4;
			}

		}
		else //HANDLE DEFEAT
		{
			//Time down before proceeding to lose screen
			defeatDuration -= dt;
			//std::cout << "Defeat Duration " << defeatDuration << std::endl;
			if (defeatDuration <= 0.0f)
			{
				//check if anything has changed to as not to unnecessary change 
				if (PostProcessing::GetInstance().currentEffects != PostProcessing::Effects::effectNormal) {
					PostProcessing::GetInstance().toggleEffects = true;
					PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectNormal;//setting to normal
				}
				if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level1) {
					if (!UpgradeManager::GetInstance().level1Completed) {
						ecs.loadLevel("Resources/SavedLevels/LoseScreenLevel.json");//lose scene
					}
					else {
						ecs.loadLevel("Resources/SavedLevels/LoseScreenPart2Level1Level.json");//lose scene
					}
				}
				else if (UiLayer::GetInstance().m_currentLevelState == UiLayer::LevelState::level2) {
					if (!UpgradeManager::GetInstance().level2Completed) {
						ecs.loadLevel("Resources/SavedLevels/LoseScreen2Level.json");//lose scene 2
					}
					else {
						ecs.loadLevel("Resources/SavedLevels/LoseScreenPart2Level2Level.json");//lose scene 2
					}

				}
				LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
			}

			//Halt animation if last frame reached
			if (sprite.currentFrame == sprite.currentAnimation.size() - 1)
				sprite.speed = 0.0f;
		}
	}
}

void PlayerController::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

std::shared_ptr<scriptBase> PlayerController::createCopy()
{
	return std::make_shared<PlayerController>();
}

std::string PlayerController::getScriptName()
{
	return name;
}