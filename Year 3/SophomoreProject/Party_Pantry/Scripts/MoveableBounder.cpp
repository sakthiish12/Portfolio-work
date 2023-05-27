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

#include "MoveableBounder.h"

MoveableBounderScript::MoveableBounderScript(const char* scriptName) :
	scriptBase(scriptName)
{}

void MoveableBounderScript::init(float dt, unsigned int idToSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToSet;
	state = scriptBase::ScriptState::UPDATE;
}

void MoveableBounderScript::update(float dt)
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

			
			if (ecs.entityHas<rigidBody2D>(entityCollided)) 
			{
			rigidBody2D& entityRB{ ecs.getComponent<rigidBody2D>(entityCollided) };

			Mathf::vec2 diffVec = Mathf::Normalize(ecs.getComponent<transform2D>(entityCollided).pos - ecs.getComponent<transform2D>(id).pos);
			entityRB.vel += diffVec * 300.0f;
			}
			
		}
	}
}

void MoveableBounderScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//Generate particles upon collision

	//Projectile is gone
	//ecs.deleteEntity(id);

	ecs.removeComponent<AABB>(id);
	ecs.removeComponent<spriteCom>(id);
	ecs.getComponent<scriptComponent>(id).scriptContainer.clear();
	LogicSystemsPtr->sceneIsChanging = true; //HACK
}

std::shared_ptr<scriptBase> MoveableBounderScript::createCopy()
{
	return std::make_shared<MoveableBounderScript>();
}

std::string MoveableBounderScript::getScriptName()
{
	return name;
}