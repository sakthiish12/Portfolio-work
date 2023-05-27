// /******************************************************************************/
// /*!
// \file         BlenderTextCountScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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

#include "BlenderTextCountScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

BlenderTextScript::BlenderTextScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderTextScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void BlenderTextScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<Com_Font>(id))
	{
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		oFont.text = std::to_string(AssetManager::GetInstance().tomatoesCollected);
	}
}

void BlenderTextScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BlenderTextScript::createCopy()
{
	return std::make_shared <BlenderTextScript>();
}

std::string BlenderTextScript::getScriptName()
{
	return name;
}






