// /******************************************************************************/
// /*!
// \file         KetchapGunScript.h
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        KetchapGunScript that set the gun
// \lines        47 (epis)
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

#include "KetchapGunScript.h"
#include <iostream>

#include "../../Engine/InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../Engine/Math/MathFunctions/Mathf.h"

using std::cout;
using std::endl;

KetchapGunScript::KetchapGunScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void KetchapGunScript::init(float dt, unsigned int idToBeSet)
{
	static_cast<void>(dt);
	static_cast<void>(dt);
	id = idToBeSet;
	//cout << "KetchapGunScript::init()" << endl;
	state = scriptBase::ScriptState::UPDATE;
}

void KetchapGunScript::update(float dt)
{
	static_cast<void>(dt);
	//player id
	for (auto playerID : AbilitiesSysPtr->getEntitiesRegistered())
	{
		auto& ptc = ecs.getComponent<transform2D>(playerID);
		//player to mouse pos
		vec2 playerToMouse = MouseManager.getWorldPosition() - ptc.pos.getVec2();
		//vec2 playerToMouseNormalised = Normalize(playerToMouse);
		//new position
		auto& etc = ecs.getComponent<transform2D>(id);
		auto playerToMouseNormalised = Normalize(playerToMouse);
		vec2 newPos = radius * playerToMouseNormalised;

		etc.pos.x = newPos.x;
		etc.pos.y = newPos.y;

		//Setting rotation
		etc.rotation = 360.f + 90.f - Mathf::getVectorRotationRadian(playerToMouseNormalised) * 180.f / 3.14f;
		if (etc.rotation > 360.f)
			etc.rotation -= 360.f;
	}
}

void KetchapGunScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> KetchapGunScript::createCopy()
{
	return std::make_shared <KetchapGunScript>();
}

std::string KetchapGunScript::getScriptName()
{
	return name;
}






