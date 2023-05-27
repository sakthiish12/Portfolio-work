#include "../../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "MenuSelectorLevel2Script.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include "../Blender/BlenderOutputScript.h"
#include <iostream>

using std::cout;
using std::endl;

MenuSelectorLevel2Script::MenuSelectorLevel2Script(const char* scriptName) :
	scriptBase(scriptName)
{
}

void MenuSelectorLevel2Script::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void MenuSelectorLevel2Script::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//mouse picking 
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform

		//can click on it to go to blender 
		Vec2 mousePos = MouseManager.getWorldPosition();
		if (
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON))
		{
			if (UpgradeManager::GetInstance().level2Completed) {
				//set the script
				ecs.loadLevel("Resources/SavedLevels/NewLevel2CavePart2Level.json");
				LogicSystemsPtr->sceneIsChanging = true;
			}
			else {
				//set the script
				ecs.loadLevel("Resources/SavedLevels/NewLevel2CaveLevel.json");
				LogicSystemsPtr->sceneIsChanging = true;
			}
		}

	}
}

void MenuSelectorLevel2Script::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> MenuSelectorLevel2Script::createCopy()
{
	return std::make_shared <MenuSelectorLevel2Script>();
}

std::string MenuSelectorLevel2Script::getScriptName()
{
	return name;
}