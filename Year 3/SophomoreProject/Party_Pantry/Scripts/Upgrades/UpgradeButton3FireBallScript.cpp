#include "../../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "UpgradeButton3FireBallScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>

using std::cout;
using std::endl;

UpgradeButtonFireballScript::UpgradeButtonFireballScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void UpgradeButtonFireballScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void UpgradeButtonFireballScript::update(float dt)
{
	if (textLoaded == true) {
		ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
	}

	UNREFERENCED_PARAMETER(dt);//unref
	//mouse picking 
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		auto& oMaterial = ecs.getComponent<material>(id);//getting the material 

		//if the super speed has aleady been unlocked 
		if (UpgradeManager::GetInstance().upgradeFireBall) {
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
			//check if there is enough to get the super fire ball 
			if (AssetManager::GetInstance().chilliesCollected < 6) {
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
					UpgradeManager::GetInstance().upgradeFireBall = true;//to upgrade super speed 
					UpgradeManager::GetInstance().next_upgrade = UpgradeManager::Next_Upgrade::Fireball;
					UpgradeManager::GetInstance().numberOfChilliesToDeduct = 6;//to deduct 5 chillies
					ecs.loadLevel("Resources/SavedLevels/BlenderLevelLevel.json");
					LogicSystemsPtr->sceneIsChanging = true;
				}
			}
		}
	}
}

void UpgradeButtonFireballScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> UpgradeButtonFireballScript::createCopy()
{
	return std::make_shared <UpgradeButtonFireballScript>();
}

std::string UpgradeButtonFireballScript::getScriptName()
{
	return name;
}