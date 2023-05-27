#include "../../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "UpgradeButton2Shield.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>

using std::cout;
using std::endl;

UpgradeButtonShieldScript::UpgradeButtonShieldScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void UpgradeButtonShieldScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void UpgradeButtonShieldScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (textLoaded == true) {
		ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
	}

	//mouse picking 
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		auto& oMaterial = ecs.getComponent<material>(id);//getting the material 
		

		//if the super speed has aleady been unlocked 
		if (UpgradeManager::GetInstance().upgradeSuperShield) {
			oMaterial.color.x = oMaterial.color.x / 3;
			oMaterial.color.y = oMaterial.color.y / 3;
			oMaterial.color.z = oMaterial.color.z / 3;

			//show the dialogue 
			if (textLoaded == false) {
				textId = ecs.createEntityFromFile("Resources/SavedEntities/Acquired.json");
				//setting the position of the text 
				ecs.getComponent<transform2D>(textId).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(textId).pos.y = oTransform.pos.y;
				textLoaded = true;
			}

			if (textLoaded == true) {
				ecs.getComponent<spriteCom>(textId).isVisible = true;//set to invisible 
			}

			return;
		}


		if (!UiLayer::GetInstance().dialoguePause) {
			//check if there is enough to get the super speed 
			if (AssetManager::GetInstance().tomatoesCollected < 3 || AssetManager::GetInstance().mushroomsCollected < 10) {
				//darken it 
				oMaterial.color.x = oMaterial.color.x / 2;
				oMaterial.color.y = oMaterial.color.y / 2;
				oMaterial.color.z = oMaterial.color.z / 2;
			}
			else {
				//can click on it to go to blender 
				Vec2 mousePos = MouseManager.getWorldPosition();
				if (
					mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
					mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON))
				{
					UpgradeManager::GetInstance().upgradeSuperShield = true;//to upgrade super speed 
					UpgradeManager::GetInstance().next_upgrade = UpgradeManager::Next_Upgrade::Shield;
					UpgradeManager::GetInstance().numberOfMushroomToDeduct = 10;//to deduct 10 mushrooms 
					UpgradeManager::GetInstance().numberOfTomatoesToDeduct = 3;//to deduct 3 tomatoes 
					ecs.loadLevel("Resources/SavedLevels/BlenderLevelLevel.json");
					LogicSystemsPtr->sceneIsChanging = true;
				}
			}
		}
	}
}

void UpgradeButtonShieldScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> UpgradeButtonShieldScript::createCopy()
{
	return std::make_shared <UpgradeButtonShieldScript>();
}

std::string UpgradeButtonShieldScript::getScriptName()
{
	return name;
}