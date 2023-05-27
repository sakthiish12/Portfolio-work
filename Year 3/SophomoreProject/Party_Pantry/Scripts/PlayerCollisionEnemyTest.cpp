// /******************************************************************************/
// /*!
// \file         PlayerCollisionEnemyTest.cpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        PlayerCollisionEnemyTest for and show how collision works
// \lines        45 (epis)
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

#include "PlayerCollisionEnemyTest.h"
#include <iostream>
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"

using std::cout;
using std::endl;

PlayerCollisionScript::PlayerCollisionScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PlayerCollisionScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	cout << "PlayerCollisionScript::init()" << endl;
	state = scriptBase::ScriptState::UPDATE;
}

void PlayerCollisionScript::update(float dt)
{
	
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id))
	{
		std::vector<unsigned int> collidedContainer = collisionSystemsPtr->getEntityCollidedWith(id);
		//cout << "update(float dt)" << " called by : " << id << " count " << testCounter++ << endl;
		for (unsigned int x : collidedContainer)
		{
			UNREFERENCED_PARAMETER(x);
		//	cout << "entity " << id << " is colliding with: " << x << endl;
			
			//logic
			//etc can get type comp and see if it matches
		}
		if (collidedContainer.size() == 0)
		{
			//cout << "entity " << id << " is NOT colliding" << endl;
		}
	}
}

void PlayerCollisionScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PlayerCollisionScript::createCopy()
{
	return std::make_shared <PlayerCollisionScript>();
}

std::string PlayerCollisionScript::getScriptName()
{
	return name;
}






