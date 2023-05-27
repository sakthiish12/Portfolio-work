// /******************************************************************************/
// /*!
// \file         ChilliesTextCountScript.cpp
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
#include "ChilliesTextCountScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

ChilliesBlenderTextScript::ChilliesBlenderTextScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void ChilliesBlenderTextScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	startingCount = static_cast<int>(AssetManager::GetInstance().chilliesCollected);//get number of tomatoes collected 
}

void ChilliesBlenderTextScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<Com_Font>(id))
	{
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		oFont.text = std::to_string(AssetManager::GetInstance().chilliesCollected);
	}
}

void ChilliesBlenderTextScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> ChilliesBlenderTextScript::createCopy()
{
	return std::make_shared <ChilliesBlenderTextScript>();
}

std::string ChilliesBlenderTextScript::getScriptName()
{
	return name;
}






