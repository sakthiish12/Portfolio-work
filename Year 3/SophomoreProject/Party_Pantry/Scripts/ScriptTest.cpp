// /******************************************************************************/
// /*!
// \file         ScriptTest.cpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        ScriptTest to show how scripting works
// \lines        43 (epis)
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

#include "ScriptTest.h"
#include <iostream>

using std::cout;
using std::endl;

TestingScriptCPP::TestingScriptCPP(const char* scriptName) :
	scriptBase(scriptName)
{
}

void TestingScriptCPP::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	cout << "TestingScriptCPP::init()" << endl;
	state = scriptBase::ScriptState::UPDATE;
}

void TestingScriptCPP::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "update(float dt)" << " called by : " << id << " count "<< testCounter++ << endl;
	
	if (ecs.entityHas<transform2D>(id))
	{
		transform2D* pos = &ecs.getComponent<transform2D>(id);
		pos->pos.x++;
		cout << "Position update x:" << pos->pos.x << " called by : " << id << endl;
		if (pos->pos.x > 1000.f)
		{
			state = scriptBase::ScriptState::END;
		}
	}
}

void TestingScriptCPP::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> TestingScriptCPP::createCopy()
{
	return std::make_shared <TestingScriptCPP>();
}

std::string TestingScriptCPP::getScriptName()
{ 
	return name; 
}






