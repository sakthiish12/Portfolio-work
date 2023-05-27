// /******************************************************************************/
// /*!
// \file         Firebomb.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         10th Apr, 2022
// \brief        This file contains the definitions for the firebomb's logic
// \lines        108
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
#include "../../Party_Pantry/Scripts/EnemyBehaviour.h"//enemy behaviour script 
#include "Firebomb.h"
#include "FireShotScript.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"
#include <Sound/SoundManagerV2.h>
#include "MushroomBoss.h"

void Firebomb::explode()
{
	const float twopi = 6.283185307f;
	float direction = 0.0f;
	float initialSpeed = 500.0f;
	for (int i = 0; i < fragments; ++i)
	{
		entityID flameID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");

		//Initial positioning
		ecs.getComponent<transform2D>(flameID).pos.x = ecs.getComponent<transform2D>(id).pos.x; //Apparently not using oTransform fixes this? But the camera is still acting strange.
		ecs.getComponent<transform2D>(flameID).pos.y = ecs.getComponent<transform2D>(id).pos.y;

		//Set bullet direction
		ecs.getComponent<transform2D>(flameID).rotation = Mathf::RadianToDegree(direction);

		//Set force
		//Forces are inconsistent
		//Force flameForce(Force::ForceType::LINEAR_FORCE, 10000.0f * Mathf::vec2(cosf(shotAngle + variance), sinf(shotAngle + variance)), 1.0f);
		rigidBody2D flameRB{};
		flameRB.vel.x = initialSpeed * cosf(direction);
		flameRB.vel.y = initialSpeed * sinf(direction);
		Force fireBallForce(Force::ForceType::LINEAR_FORCE, 5000.0f * Normalize(Vec2{ cosf(direction), sinf(direction) }), 100.f);
		flameRB.forceManager.AddLinearForce("fireBallForce", fireBallForce, true);//adding linear force to the water droplet
		//flameRB.forceManager.AddLinearForce("FlameForce", flameForce, false);
		flameRB.forceManager.setMaxVel(initialSpeed);
		flameRB.forceManager.setLinearDrag(0.00f);
		ecs.addComponent<rigidBody2D>(flameID, flameRB);

		//Set decay timer
		Com_TimeDestructor flameTD;
		flameTD.timeDelayDestructor = 1.0f; //1 second lifetime
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

		direction += twopi / (float)fragments;
	}
}

Firebomb::Firebomb(const char* scriptName) :
	scriptBase(scriptName)
{

}

void Firebomb::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void Firebomb::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//Require this components
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<AABB>(id))
	{
		//It colliding into anything but bullets will cause the firebomb to explode
		std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
		if (!collidedEntities.empty())
		{
			entityID entityCollided = *(collidedEntities.begin());

			//Do not consider the player or other bullets
			if (!(ecs.entityHas<Com_type>(entityCollided) && (
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::player ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::bullet ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::enemyBullet)))
			{
				audio.playPannedSoundEffect(audio.sfxTeleporterAppear, &audio.chSFX, 0.0f, 4.0f, 0.3f);
				explode();

				//Boss shield denies damage
				bool damageDenied = false;
				if (ecs.entityHas<scriptComponent>(entityCollided))
				{
					auto& sc{ ecs.getComponent<scriptComponent>(entityCollided).scriptContainer };
					auto scriptItr{ sc.find("MushroomBoss") };
					if (scriptItr != sc.end())
					{
						MushroomBoss* boss = reinterpret_cast<MushroomBoss*>((*scriptItr).second.get());
						if (boss->currentAction == MushroomBoss::action::shield)
							damageDenied = true;
					}
				}

				if (damageDenied)
					audio.playPannedSoundEffect(audio.sfxLaserBoom, &audio.chSFX, 0.0f, 0.1f, 2.0f);

				//Damage if enemy hit
				if (!damageDenied && ecs.entityHas<Com_Health>(entityCollided))
				{
					ecs.getComponent<Com_Health>(entityCollided).health -= 5;
				}

				//TODO Explosion SFX here
				audio.playPannedSoundEffect(audio.sfxExplosion);

				state = scriptBase::ScriptState::END;
			}
		}
	}
}

void Firebomb::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	ecs.deleteEntity(id);
}

std::shared_ptr<scriptBase> Firebomb::createCopy()
{
	return std::make_shared<Firebomb>();
}

std::string Firebomb::getScriptName()
{
	return name;
}