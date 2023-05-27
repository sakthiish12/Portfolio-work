// /******************************************************************************/
// /*!
// \file         Tornado.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         9th Apr, 2022
// \brief        This file contains the definitions for the tornado's logic
// \lines        181
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
#include "WaterShotScript.h"
#include "Tornado.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"
#include <Sound/SoundManagerV2.h>
#include "MushroomBoss.h"

Tornado::Tornado(const char* scriptName) :
	scriptBase(scriptName)
{}

void Tornado::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void Tornado::update(float dt)
{
	//Only process update loop when the game is playing
	if (UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Play)
	{
		return;
	}

	//Flip about itself to simulate animation
	timeTillNextFlip -= dt;
	if (timeTillNextFlip <= 0.0f)
	{
		if (ecs.entityHas<transform2D>(id))
			ecs.getComponent<transform2D>(id).scale.x = -ecs.getComponent<transform2D>(id).scale.x;
		timeTillNextFlip += flipInterval;
	}

	//Reduce lifetime remaining
	remainingLifetime -= dt;
	if (remainingLifetime <= 0.0f)
	{
		state = scriptBase::ScriptState::END;
	}

	//Require these components
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<AABB>(id))
	{
		//Shoot out airblasts at random directions at random intervals
		timeTillNextShot -= dt;
		if (timeTillNextShot <= 0.0f)
		{
			//Shoot out a airblast in a random direction
			const float initialSpeed = 1600.0f;
			const int d = 1;
			const float sm = 3.0f;
			float direction = 6.283185307f * ((float)std::rand() / (float)RAND_MAX);

			entityID airblastID = ecs.createEntityFromFile("Resources/PresetEntities/waterdroplet.json");

			//Initial positioning
			ecs.getComponent<transform2D>(airblastID).pos.x = ecs.getComponent<transform2D>(id).pos.x + 50.0f * cosf(direction); //Apparently not using oTransform fixes this? But the camera is still acting strange.
			ecs.getComponent<transform2D>(airblastID).pos.y = ecs.getComponent<transform2D>(id).pos.y + 50.0f * sinf(direction);
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

			//Prepare timer
			timeTillNextShot = shotDelay + shotDelay * ((float)std::rand() / (float)RAND_MAX);
		}

		//Stun and damage enemy collided with. Cause the tornado to die out
		std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
		if (!collidedEntities.empty())
		{
			entityID entityCollided = *(collidedEntities.begin());

			//Do not consider player or other bullets or enemy bullets
			if (!(ecs.entityHas<Com_type>(entityCollided) && (
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::player ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::bullet ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::enemyBullet)))
			{
				//Damage enemies, unless their health is down to 1
				//If health is down to 1, stun them instead
				if (ecs.entityHas<Com_type>(entityCollided) &&
					ecs.entityHas<Com_Health>(entityCollided) &&
					ecs.entityHas<rigidBody2D>(entityCollided) &&
					ecs.entityHas<transform2D>(entityCollided))
				{
					Com_type& entityType{ ecs.getComponent<Com_type>(entityCollided) };
					entityType;
					Com_Health& entityHealth{ ecs.getComponent<Com_Health>(entityCollided) };
					rigidBody2D& entityRB{ ecs.getComponent<rigidBody2D>(entityCollided) };

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

					if (!damageDenied)
					{
						//Do not defeat enemy
						if (damage >= entityHealth.health)
							entityHealth.health = 1;
						else
							entityHealth.health -= damage;

						//Stun
						if (ecs.entityHas<scriptComponent>(entityCollided))
						{
							auto& sc{ ecs.getComponent <scriptComponent>(entityCollided).scriptContainer };
							auto scriptItr{ sc.begin() }; //HACK Hopefully they only have one script in them
							if (scriptItr != sc.end())
							{
								//std::cout << "Stunning enemy\n";
								EnemyBehaviour* enemy = reinterpret_cast<EnemyBehaviour*>((*scriptItr).second.get());
								//std::cout << enemy->stunTimeRemaining << " -> ";
								enemy->stunTimeRemaining += stunMultiplier * enemy->stunMultiplier / max(entityHealth.health, 1);
								//std::cout << enemy->stunTimeRemaining << std::endl;
							}
						}

						//if the entity's health is lesser than 0, invoke stun animation 
						/*if (entityHealth.health <= 3) {
							if (ecs.entityHas<scriptComponent>(entityCollided)) {
								auto& sc{ ecs.getComponent<scriptComponent>(entityCollided).scriptContainer };
								auto scripIter{ sc.find("EnemyBehaviour") };
								if (scripIter != sc.end()) {
									//update the script to indicate that it is stunned
									EnemyBehaviour* enemy = reinterpret_cast<EnemyBehaviour*>((*scripIter).second.get());
									enemy->isStunned = true;//setting the is stunned to true
								}
							}
						}*/

						//KNOCK THEM BACK
						Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
						entityRB.vel += diffVec * 5000.0f / entityRB.Mass; //HACK use mass directly instead of adding a force.
						//There are enemies of varying masses, simply adding velocity won't do no more
						//Force projectileForce{ Force::ForceType::LINEAR_FORCE, diffVec * 1000.0f, 100.0f};
						//entityRB.forceManager.AddLinearForce("projectileKB", projectileForce);

						//Apply particle effect===============
						// //TODO: set to destory emitter and componet itself to save on mem space
						//if no EC, add EC
						if (!ecs.entityHas<EmitterComponent>(entityCollided))
						{
							EmitterComponent EC{};
							EC.emitterContainer.insert(std::make_pair("EmitterExplosionDamaged", std::make_shared<EmitterExplosionDamaged>()));
							ecs.addComponent<EmitterComponent>(entityCollided, EC);
						}
						EmitterExplosionDamaged* emi = dynamic_cast<EmitterExplosionDamaged*>(ecs.getComponent<EmitterComponent>(entityCollided).emitterContainer["EmitterExplosionDamaged"].get());
						if (emi)
							emi->setTriggered(true);
						//Apply particle effect===============END
					}
				}

				//Handle destruction upon collision
				state = scriptBase::ScriptState::END;
			}
		}
	}
}

void Tornado::end(float dt)
{
	//Flip about itself to simulate animation
	timeTillNextFlip -= dt;
	if (timeTillNextFlip <= 0.0f)
	{
		if (ecs.entityHas<transform2D>(id))
			ecs.getComponent<transform2D>(id).scale.x = -ecs.getComponent<transform2D>(id).scale.x;
		timeTillNextFlip += flipInterval;
	}

	//Remove collider if found
	if (ecs.entityHas<AABB>(id))
	{
		ecs.removeComponent<AABB>(id);
	}

	//Fade out to deletion
	if (ecs.entityHas<material>(id))
	{
		ecs.getComponent<material>(id).color.w -= 3.0f * dt;
		if (ecs.getComponent<material>(id).color.w <= 0.0f)
			ecs.deleteEntity(id);
	}
	else
	{
		//If for some reason the component wasn't found
		ecs.deleteEntity(id);
	}
}

std::shared_ptr<scriptBase> Tornado::createCopy()
{
	return std::make_shared<Tornado>();
}

std::string Tornado::getScriptName()
{
	return name;
}