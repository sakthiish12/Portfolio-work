// /******************************************************************************/
// /*!
// \file         FireShotScript.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         6th Dec, 2021
// \brief        This file contains the definitions for water shot's logic
// \lines        66 Lines
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
#include <Sound/SoundManagerV2.h>
#include "EnemyBehaviour.h"
#include "FireShotScript.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterExplosionDamaged.h"
#include "MushroomBoss.h"

FireShotScript::FireShotScript(const char* scriptName) :
	scriptBase(scriptName)
{}

void FireShotScript::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void FireShotScript::update(float dt)
{
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
			if (!(ecs.entityHas<Com_type>(entityCollided) && (ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::player || ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::bullet)))
			{
				//Damage enemies
				if (ecs.entityHas<Com_type>(entityCollided))
				{
					Com_type& entityType{ ecs.getComponent<Com_type>(entityCollided) };

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
						//Damage enemies
						if (entityType.type == Com_type::type::enemy || entityType.type == Com_type::type::enemyrange)
						{
							if (ecs.entityHas<Com_Health>(entityCollided))
							{
								Com_Health& entityHealth{ ecs.getComponent<Com_Health>(entityCollided) };
								entityHealth.health -= 1;
							}
						}

						//Stun
						if (ecs.entityHas<scriptComponent>(entityCollided) && ecs.entityHas<Com_Health>(entityCollided))
						{
							auto& sc{ ecs.getComponent <scriptComponent>(entityCollided).scriptContainer };
							auto scriptItr{ sc.begin() }; //HACK Hopefully they only have one script in them
							if (scriptItr != sc.end())
							{
								//std::cout << "Stunning enemy\n";
								EnemyBehaviour* enemy = reinterpret_cast<EnemyBehaviour*>((*scriptItr).second.get());
								//std::cout << enemy->stunTimeRemaining << " -> ";
								enemy->stunTimeRemaining += 0.5f * enemy->stunMultiplier / max(ecs.getComponent<Com_Health>(entityCollided).health, 1);
								//std::cout << enemy->stunTimeRemaining << std::endl;
							}
						}

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

				audio.playPannedSoundEffect(audio.sfxTeleporterAppear, &audio.chSFX, 0.0f, 1.0f, 0.5f);

				//Handle destruction upon collision
				state = scriptBase::ScriptState::END;
			}
		}
	}
}

void FireShotScript::end(float dt)
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

std::shared_ptr<scriptBase> FireShotScript::createCopy()
{
	return std::make_shared<FireShotScript>();
}

std::string FireShotScript::getScriptName()
{
	return name;
}