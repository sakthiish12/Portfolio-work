// /******************************************************************************/
// /*!
// \file         MeleeSpatula.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         10th Apr, 2022
// \brief        This file contains the definitions for melee spatula's logic
// \lines        123
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
#include "MeleeSpatula.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"
#include <Sound/SoundManagerV2.h>
#include "MushroomBoss.h"

MeleeSpatula::MeleeSpatula(const char* scriptName) :
	scriptBase(scriptName)
{}

void MeleeSpatula::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void MeleeSpatula::update(float dt)
{
	//std::cout << "Shot at " << ecs.getComponent<transform2D>(id).pos.x << ", " << ecs.getComponent<transform2D>(id).pos.y << std::endl;

	//Find the player
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

	UNREFERENCED_PARAMETER(dt);
	//Require these components
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<AABB>(id))
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
					audio.playPannedSoundEffect(audio.sfxEnemySquish, &audio.chSFX, 0.0f, 3.0f);

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
						entityHealth.health -= damage;

						//If managed to defeat enemy, player gains back some health
						if (entityHealth.health <= 0)
						{
							if (playerFound && ecs.entityHas<Com_Health>(playerEntity))
							{
								ecs.getComponent<Com_Health>(playerEntity).health += 20;
								if (ecs.getComponent<Com_Health>(playerEntity).health > 100) //Maximum limit
									ecs.getComponent<Com_Health>(playerEntity).health = 100;
							}
						}

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
								enemy->stunTimeRemaining += stunMultiplier * enemy->stunMultiplier / max(ecs.getComponent<Com_Health>(entityCollided).health, 1);
								//std::cout << enemy->stunTimeRemaining << std::endl;
							}
						}

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

				audio.playPannedSoundEffect(audio.sfxMetalCrunch);
				//Handle destruction upon collision
				state = scriptBase::ScriptState::END;
			}
		}
	}
}

void MeleeSpatula::end(float dt)
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

std::shared_ptr<scriptBase> MeleeSpatula::createCopy()
{
	return std::make_shared<MeleeSpatula>();
}

std::string MeleeSpatula::getScriptName()
{
	return name;
}