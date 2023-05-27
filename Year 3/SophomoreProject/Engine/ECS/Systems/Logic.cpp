// /******************************************************************************/
// /*!
// \file         Logic.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Dec, 2021
// \brief        Logic sys class for scripting cpp
// \lines        90 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Logic.h"
#include <algorithm>


//void scriptManager::sortByAlphabetical()
//{
//	std::sort(_scriptDatabase.begin(), _scriptDatabase.end(), [](std::string a, std::string b) {
//			return a.compare(b); 
//		}
//	);
//}

//add script to data base
void scriptManager::addScript(std::shared_ptr<scriptBase> script)
{
	_scriptDatabase[script->name] = script;
	//sortByAlphabetical();
}

void scriptManager::deleteScript(std::shared_ptr<scriptBase> script)
{
	deleteScript(script->name);
}
void scriptManager::deleteScript(std::string script)
{
	_scriptDatabase.erase(script);
}

scriptManager& scriptManager::getInstance()
{
	static scriptManager inst;
	return inst;
}

std::shared_ptr<scriptBase> scriptManager::createFromScriptDatabase(std::string name)
{
	//things to consider, 
	//make new obj derived with a base shared ptr
	if (_scriptDatabase[name] == nullptr)
	{
		//NULLPTR!!!
		//to handle it in the future
		//return nullptr;
	}
	return _scriptDatabase[name]->createCopy();
}


//get a specific script pointer
std::shared_ptr<scriptBase> scriptManager::getScript(std::string name)
{
	return _scriptDatabase[name];
}

std::map<std::string, std::shared_ptr<scriptBase> > scriptManager::getScriptDatabase()
{
	return _scriptDatabase;
}




////////////////////////////////////
std::shared_ptr<LogicSystem> LogicSystemsPtr;

LogicSystem::LogicSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{

}
LogicSystem::~LogicSystem() 
{
}

void LogicSystem::registerScriptToSystem() 
{
}

void LogicSystem::addScript(std::shared_ptr<scriptBase> script)
{
	ScriptManager.addScript(script);
}

void LogicSystem::deleteScript(std::shared_ptr<scriptBase> script)
{
	ScriptManager.deleteScript(script->name);
}
void LogicSystem::deleteScript(std::string script)
{
	ScriptManager.deleteScript(script);
}

//std::shared_ptr<LogicSystem> LogicSystem::getInstance()
//{
//	//but ecs reset will clear it
//	if (LogicSystemsPtr == nullptr)
//	{
//		ecs.registerSystem<LogicSystem>();
//	}
//	else
//		return LogicSystemsPtr;
//}


void LogicSystem::init()
{
	//LogicSystemsPtr = ecs.registerSystem<LogicSystem>();//registering the system 
	ecs.setSystemComponentRequirement<LogicSystem, scriptComponent>();//setting up required component

	//For testing
	/*scriptComponent ass;
	scriptComponent ass2;
	
	ass.scriptContainer.push_back(ScriptManager.createFromScriptDatabase("TestingScriptCPP"));
	ass2.scriptContainer.push_back(ScriptManager.createFromScriptDatabase("TestingScriptCPP"));

	ecs.addComponent<scriptComponent>(1, ass);
	ecs.addComponent<scriptComponent>(2, ass2);*/
}

void LogicSystem::update(float dt)
{
	//update all entity with their script
	for (auto& e : getEntitiesRegistered())
	{
		if (ecs.entityHas<scriptComponent>(e))
		{
			auto& sc = ecs.getComponent<scriptComponent>(e);
			for (auto& x : sc.scriptContainer)
			{
				if (x.second == nullptr)
					continue;

				if (x.second->state == scriptBase::ScriptState::UPDATE)
					x.second->update(dt);
				else if (x.second->state == scriptBase::ScriptState::START)
				{
					x.second->init(dt, e);
				}
				else
					x.second->end(dt);

				if (sceneIsChanging)
				{
					sceneIsChanging = false;
					return;
				}
				if (!ecs.entityHas<scriptComponent>(e))
					break;
			}
		}
	}
}
