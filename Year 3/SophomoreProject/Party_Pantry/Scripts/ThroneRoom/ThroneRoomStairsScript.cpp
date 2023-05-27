#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "ThroneRoomStairsScript.h"
#include "../../InputMgr/InputMgr.h"
#include <iostream>

using std::cout;
using std::endl;

ThroneRoomStairsScript::ThroneRoomStairsScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void ThroneRoomStairsScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;


}

void ThroneRoomStairsScript::update(float dt)
{
	if (textLoaded == true) {
		ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
	}

	UNREFERENCED_PARAMETER(dt);//unref
	entityID playerEntity{ 1 };
	//mouse picking 
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);
		auto& eInnerTransform = ecs.getComponent<transform2D>(playerEntity);
		auto& oMaterial = ecs.getComponent<material>(id);
		//pulsating 
		if (pulsatingFadeIn == true) {
			oMaterial.color.x >= 1.f ? pulsatingFadeIn = false : oMaterial.color.x += 0.5f * dt;
			oMaterial.color.y >= 1.f ? pulsatingFadeIn = false : oMaterial.color.y += 0.5f * dt;
			oMaterial.color.z >= 1.f ? pulsatingFadeIn = false : oMaterial.color.z += 0.5f * dt;
		}
		else {
			oMaterial.color.x <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.x -= 0.5f * dt;
			oMaterial.color.y <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.y -= 0.5f * dt;
			oMaterial.color.z <= 0.5f ? pulsatingFadeIn = true : oMaterial.color.z -= 0.5f * dt;
		}

		if (eInnerTransform.pos.x > (oTransform.pos.x - radiusTeleport) && eInnerTransform.pos.x < (oTransform.pos.x + radiusTeleport) &&
			eInnerTransform.pos.y >(triggerPosY - radiusTeleport) && eInnerTransform.pos.y < (triggerPosY + radiusTeleport)) {


			//show the dialogue 
			if (textLoaded == false) {
				textId = ecs.createEntityFromFile("Resources/PresetEntities/PressFInteract.json");
				//setting the position of the text 
				ecs.getComponent<transform2D>(textId).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(textId).pos.y = triggerTextPosY;
				textLoaded = true;
			}

			if (textLoaded == true) {
				ecs.getComponent<spriteCom>(textId).isVisible = true;//set to invisible 
			}
			//if text is loaded and the boss is dead
			if (KeyManager.isKeyTriggered('f', true))
			{
				//ecs.loadLevel("Resources/SavedLevels/UpgradeMenuLevel.json");//disabled for beta

				ecs.loadLevel("Resources/SavedLevels/SkillUpgradeLevel.json");
				LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
			}
		}
	}
}

void ThroneRoomStairsScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> ThroneRoomStairsScript::createCopy()
{
	return std::make_shared <ThroneRoomStairsScript>();
}

std::string ThroneRoomStairsScript::getScriptName()
{
	return name;
}