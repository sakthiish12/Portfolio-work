// /******************************************************************************/
// /*!
// \file         Logic.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Dec, 2021
// \brief        Logic sys class for scripting cpp
// \lines        43 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _LOGICSYSTEM_H_
#define _LOGICSYSTEM_H_

#include <string>
#include <map>
#include <memory>

#include "../system.h"
#include "../../ECS/ECS.h"

struct scriptBase;
class scriptManager
{
	std::map<std::string, std::shared_ptr<scriptBase> > _scriptDatabase;
	std::vector<std::string> _scriptToBeRemoved;

	//void sortByAlphabetical();
public:
	//add script to data base
	void addScript(std::shared_ptr<scriptBase> script);

	void deleteScript(std::shared_ptr<scriptBase> script);
	void deleteScript(std::string script);
	static scriptManager& getInstance();
	
	std::shared_ptr<scriptBase> createFromScriptDatabase(std::string name);

	//get a specific script pointer
	std::shared_ptr<scriptBase> getScript(std::string name);
	std::map<std::string, std::shared_ptr<scriptBase> > getScriptDatabase();
};
#define ScriptManager ::scriptManager::getInstance()

/*
Logic
For C++ script
HOW TO USE ME:

1) In application, Inherit scriptBase and define all virtual function
2) Add to includeAllScript function
3) Add it to entity script component container

eg
scriptComponent ass;
scriptComponent ass2;
//or get the reference if already existed
ass.scriptContainer.push_back(ScriptManager.createFromScriptDatabase("TestingScriptCPP"));
ass2.scriptContainer.push_back(ScriptManager.createFromScriptDatabase("TestingScriptCPP"));

ecs.addComponent<scriptComponent>(1, ass);
ecs.addComponent<scriptComponent>(2, ass2);
*/

//logic / script system 
class LogicSystem : public systemBase
{
	//scriptManager _scriptManager; //for editor to show
	//multimap container so that one entity can have more than one script. Testing first
	//std::multimap<entityID, std::string > _entityIDScript;
public:
	LogicSystem(bool requireComponentMatch = false);
	~LogicSystem();
	
	void registerScriptToSystem();

	void addScript(std::shared_ptr<scriptBase> script);
	void deleteScript(std::shared_ptr<scriptBase> script);
	void deleteScript(std::string script);

	//static std::shared_ptr<LogicSystem> getInstance();

	bool sceneIsChanging = false;

	static void init();
	void update(float dt);
private:
	LogicSystem(LogicSystem& rhs) = delete;
	LogicSystem(const LogicSystem& rhs) = delete;
	LogicSystem& operator=(const LogicSystem& rhs) = delete;
	LogicSystem& operator=(const LogicSystem& rhs) const = delete;
};



//#define PhysicsSystem ::physicsSystem::GetInstance()
extern std::shared_ptr<LogicSystem> LogicSystemsPtr;
#define logicSystem LogicSystemsPtr

#endif //_PHYSICSSYSTEM_H_
