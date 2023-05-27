//click body script 
//
//
//
//
//
#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"

#include "../../../Engine/ECS/Systems/Logic.h"
#include "UpgradeBodyScript.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

UpgradeBodyScript::UpgradeBodyScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void UpgradeBodyScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void UpgradeBodyScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//mouse picking 
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform

		Vec2 mousePos = MouseManager.getWorldPosition();
		if (
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON))
		{
			ecs.loadLevel("Resources/SavedLevels/BodyUpgradeLevel.json");
			LogicSystemsPtr->sceneIsChanging = true;
		}
	}
}

void UpgradeBodyScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> UpgradeBodyScript::createCopy()
{
	return std::make_shared <UpgradeBodyScript>();
}

std::string UpgradeBodyScript::getScriptName()
{
	return name;
}

