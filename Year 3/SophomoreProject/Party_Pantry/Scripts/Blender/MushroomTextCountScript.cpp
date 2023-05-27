// /******************************************************************************/
// /*!
// \file         MushroomTextCountScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the blender text count script 
// \lines        47
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
#include "MushroomTextCountScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

MushroomBlenderTextScript::MushroomBlenderTextScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void MushroomBlenderTextScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	startingCount = static_cast<int>(AssetManager::GetInstance().mushroomsCollected);//get number of tomatoes collected 
}

void MushroomBlenderTextScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<Com_Font>(id))
	{
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		oFont.text = std::to_string(AssetManager::GetInstance().mushroomsCollected);
	}
}

void MushroomBlenderTextScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> MushroomBlenderTextScript::createCopy()
{
	return std::make_shared <MushroomBlenderTextScript>();
}

std::string MushroomBlenderTextScript::getScriptName()
{
	return name;
}






