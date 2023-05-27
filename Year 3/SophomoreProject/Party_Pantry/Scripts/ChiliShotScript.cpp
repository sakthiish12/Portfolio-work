// /******************************************************************************/
// /*!
// \file         ChiliShotScript.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         6th Dec, 2021
// \brief        This file contains the definitions for enemy chili shot's logic
// \lines        69 Lines
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
#include "../../Engine/InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"

#include "PlayerController.h"
#include "ChiliShotScript.h"
#include <Sound/SoundManagerV2.h>
#include <iostream>

ChiliShotScript::ChiliShotScript(const char* scriptName) :
	scriptBase(scriptName)
{}

void ChiliShotScript::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;

	state = scriptBase::ScriptState::UPDATE;
}

void ChiliShotScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	//Discolouration to distinguish
	if (ecs.entityHas<material>(id))
	{
		ecs.getComponent<material>(id).color.x = 1.0f;
		ecs.getComponent<material>(id).color.y = 0.5f;
		ecs.getComponent<material>(id).color.z = 0.5f;
	}

	//Require these components
	if (ecs.entityHas<transform2D>(id) &&
		ecs.entityHas<rigidBody2D>(id) &&
		ecs.entityHas<AABB>(id))
	{
		std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
		if (!collidedEntities.empty())
		{
			entityID entityCollided = *(collidedEntities.begin());

			//Do not consider enemies or other bullets
			if (!(ecs.entityHas<Com_type>(entityCollided) && (
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::enemy ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::enemyrange ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::bullet ||
				ecs.getComponent<Com_type>(entityCollided).type == Com_type::type::enemyBullet)))
			{
				//Damage player by 10 and push him
				if (ecs.entityHas<Com_type>(entityCollided) &&
					ecs.entityHas<Com_Health>(entityCollided) &&
					ecs.entityHas<rigidBody2D>(entityCollided) &&
					ecs.entityHas<transform2D>(entityCollided))
				{
					Com_type& entityType{ ecs.getComponent<Com_type>(entityCollided) };
					Com_Health& entityHealth{ ecs.getComponent<Com_Health>(entityCollided) };
					rigidBody2D& entityRB{ ecs.getComponent<rigidBody2D>(entityCollided) };
					entityType;
					//Shield denies damage
					//Player charging ahead at 4 pressure (ultra speed) also denies damage
					//Check for damage denial
					bool damageDenied = false;

					//Play hit sound effect
					audio.playPannedSoundEffect(audio.sfxMetalCrunch, &audio.chSFX, 0.0f, 3.0f, 2.0f);

					if (ecs.entityHas<scriptComponent>(entityCollided))
					{
						auto& sc{ ecs.getComponent<scriptComponent>(entityCollided).scriptContainer };
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

					//What happens when Kettle Knight is not ready
					if (!damageDenied)
					{
						entityHealth.health -= 3;
						Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
						entityRB.vel += diffVec * 500.0f;
					}
				}

				audio.playPannedSoundEffect(audio.sfxTeleporterAppear, &audio.chSFX, 0.0f, 1.0f, 0.5f);

				//Handle destruction upon collision
				state = scriptBase::ScriptState::END;
			}
		}
	}
}

void ChiliShotScript::end(float dt)
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

std::shared_ptr<scriptBase> ChiliShotScript::createCopy()
{
	return std::make_shared<ChiliShotScript>();
}

std::string ChiliShotScript::getScriptName()
{
	return name;
}