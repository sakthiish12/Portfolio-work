// /******************************************************************************/
// /*!
// \file         WaterShotScript.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         6th Dec, 2021
// \brief        This file contains the definitions for water shot's logic
// \lines        62 Lines
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
#include "../../Party_Pantry/Scripts/EnemyBehaviour.h"//enemy behaviour script 
#include "WaterShotScript.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"
#include <Sound/SoundManagerV2.h>
#include "MushroomBoss.h"

void WaterShotScript::collisionHandling()
{
	std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
	if (!collidedEntities.empty())
	{
		entityID entityCollided = *(collidedEntities.begin());

		//Do not consider player or other bullets
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

				//Play hit sound effect
				audio.playPannedSoundEffect(audio.sfxMetalCrunch, &audio.chSFX, 0.0f, 3.0f, 2.0f);

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
					entityRB.vel += diffVec * 500.0f / entityRB.Mass; //HACK use mass directly instead of adding a force.
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

			audio.playPannedSoundEffect(audio.sfxTeleporterAppear);

			//Handle destruction upon collision
			state = scriptBase::ScriptState::END;
		}
	}
}

WaterShotScript::WaterShotScript(const char* scriptName) :
	scriptBase(scriptName)
{}

void WaterShotScript::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
	collisionHandling();
}

void WaterShotScript::update(float dt)
{
	//std::cout << "Shot at " << ecs.getComponent<transform2D>(id).pos.x << ", " << ecs.getComponent<transform2D>(id).pos.y << std::endl;

	UNREFERENCED_PARAMETER(dt);
	//Require these components
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<AABB>(id))
	{
		collisionHandling();
	}
}

void WaterShotScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//Generate particles upon collision

	//Projectile is gone
	ecs.deleteEntity(id);

	//ecs.removeComponent<AABB>(id);
	//ecs.removeComponent<spriteCom>(id);
	//ecs.getComponent<scriptComponent>(id).scriptContainer.clear();
	//LogicSystemsPtr->sceneIsChanging = true; //HACK
}

std::shared_ptr<scriptBase> WaterShotScript::createCopy()
{
	return std::make_shared<WaterShotScript>();
}

std::string WaterShotScript::getScriptName()
{
	return name;
}