#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "UpgradeSkillsScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>

using std::cout;
using std::endl;

UpgradeSkillsScript::UpgradeSkillsScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void UpgradeSkillsScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void UpgradeSkillsScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//mouse picking 

}

void UpgradeSkillsScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> UpgradeSkillsScript::createCopy()
{
	return std::make_shared <UpgradeSkillsScript>();
}

std::string UpgradeSkillsScript::getScriptName()
{
	return name;
}