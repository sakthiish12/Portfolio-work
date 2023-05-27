// /******************************************************************************/
// /*!
// \file         AbilitiesUIScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the abilities UI
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"

#include "AbilitiesUIScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

AbilitiesUIScript::AbilitiesUIScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void AbilitiesUIScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void AbilitiesUIScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<spriteCom>(id))
	{
		//auto& oSprite = ecs.getComponent<spriteCom>(id);//getting sprite component 
		
		//get the only registered player with abilities
		for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered())
		{
			auto eAbilities = &ecs.getComponent<Com_Abilities>(eInner);

			//check what type of abilities is activated and change the sprite accordingly 
			switch (eAbilities->typeAbilities)
			{
			case eAbilities->waterCannon:
				//oSprite.textureId = "waterdropletUI.png";
				break;
			case eAbilities->fireball:
				//oSprite.textureId = "fireballUI.png";
				break;
			case eAbilities->shield:
				//oSprite.textureId = "shieldUI.png";
				break;
			case eAbilities->superSpeed:
				//oSprite.textureId = "superspeedUI.png";
				break;
			default:
				break;
			}
		}
	}
}

void AbilitiesUIScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> AbilitiesUIScript::createCopy()
{
	return std::make_shared <AbilitiesUIScript>();
}

std::string AbilitiesUIScript::getScriptName()
{
	return name;
}






