// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        133
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
#include "../../Engine/InputMgr/InputMgr.h"//input mgr
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/Sound/SoundManager.h"
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "TeleporterScript.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterTeleporter.h"
#include "../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>
#include <random>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

/**
\brief - random number generator for integers
\param start - the starting integer range
\param end - the ending integer range
\returns - the random number within the range
**/
template<typename T>
T randomnumgen(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_int_distribution<T> random(start, end);
	return random(mt);
}


template<typename T>
T randomnumgenfloat(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_real_distribution<T> random(start, end);
	return random(mt);
}


TeleportScript::TeleportScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void TeleportScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	ecs.getComponent<spriteCom>(id).isVisible = false;
	state = scriptBase::ScriptState::UPDATE;

	//create EmitterComponent
	EmitterComponent EC{};
	EC.emitterContainer.insert(std::make_pair("EmitterTeleporterTrail", std::make_shared<EmitterTeleporterTrail>()));
	ecs.addComponent<EmitterComponent>(id, EC);

}


void TeleportScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	entityID playerEntity{ 1 };
	
	if (ecs.entityHas<transform2D>(id)) {

		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting sprite 
		if (textLoaded == true) {
			ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
		}


		auto& eInnerTransform = ecs.getComponent<transform2D>(playerEntity);//getting player transform 

		//if the player is near to the blender, show the dialogue , AABB
		if (eInnerTransform.pos.x > (oTransform.pos.x - radiusOfDetection) && eInnerTransform.pos.x < (oTransform.pos.x + radiusOfDetection) &&
			eInnerTransform.pos.y >(oTransform.pos.y - radiusOfDetection) && eInnerTransform.pos.y < (oTransform.pos.y + radiusOfDetection)) {
			if (bossDead == true) {
				//show the dialogue 
				if (textLoaded == false) {
					textId = ecs.createEntityFromFile("Resources/PresetEntities/teleporterText.json");
					//setting the position of the text 
					ecs.getComponent<transform2D>(textId).pos.x = oTransform.pos.x;
					ecs.getComponent<transform2D>(textId).pos.y = oTransform.pos.y + textOffSetY;
					textLoaded = true;
				}

				if (textLoaded == true && bossDead == true)
					ecs.getComponent<spriteCom>(textId).isVisible = true;//is visible 
			}
		}

		if (eInnerTransform.pos.x > (oTransform.pos.x - radiusTeleport) && eInnerTransform.pos.x < (oTransform.pos.x + radiusTeleport) &&
			eInnerTransform.pos.y >(oTransform.pos.y - radiusTeleport) && eInnerTransform.pos.y < (oTransform.pos.y + radiusTeleport)) {

			//if text is loaded and the boss is dead 
			if (bossDead == true) {
				oSprite.textureId = "newTeleporter.png";
				oTransform.pos.z = 1.f;
				emitterPos = oTransform.pos;
				emitterPos.x = randomnumgenfloat(oTransform.pos.x - emitterRangeX, oTransform.pos.x + emitterRangeX);
				emitterPos.y = randomnumgenfloat(oTransform.pos.y, oTransform.pos.y + emitterRangeY);

				//particle for teleporter 
				if (ecs.entityHas<EmitterComponent>(id))
				{
					EmitterTeleporterTrail* emi = dynamic_cast<EmitterTeleporterTrail*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterTeleporterTrail"].get());
					if (emi)
						emi->setTriggered(true, emitterVelocity, emitterPos);
				}

				if (KeyManager.isKeyTriggered('f', true) && !UiLayer::GetInstance().dialoguePause)
				{
					//end scene/win scene 
					if (UpgradeManager::GetInstance().level1Completed && UpgradeManager::GetInstance().level2Completed
						&& UpgradeManager::GetInstance().upgradeFireBall && UpgradeManager::GetInstance().upgradeSuperShield && UpgradeManager::GetInstance().upgradeSuperSpeed) {
						ecs.loadLevel("Resources/SavedLevels/EndPart1Level.json");
						audio.changeBGM(audio.bgmWin2);
						LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
					}
					else {
						UpgradeManager::GetInstance().level1Completed = true;//set that level 1 is completed 
						//SoundClass::getInstance()->channels[Teleporter]->setPaused(false);
						//SoundClass::getInstance()->channels[throneRoom]->setPaused(false);
						//SoundClass::getInstance()->channels[winSound]->setPaused(true);
						audio.changeBGM(audio.bgmWin1);
						ecs.loadLevel("Resources/SavedLevels/ThroneRoomLevel.json");
						LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
						//SoundClass::getInstance()->Pause(walk);
					}

				}
			}
		}
		else {
			oSprite.textureId = "teleporterNotActive.png";
		}


	}

	//if the boss is not dead 
	if(bossDead == false){
		for (auto const& e : ecs.getEntityNames())
		{
			float pos[] = { 0,0,0 };
			if (e.second == "TomatoNewBoss" && ecs.entityHas<Com_Health>(e.first) && ecs.getComponent<Com_Health>(e.first).health <= 1)
			{
				//SoundClass::getInstance()->Pause(fightingtheboss);
				ecs.getComponent<spriteCom>(id).isVisible = true;
				//SoundClass::getInstance()->setArgsAndPlay(winSound,pos);
				//ecs.addComponent<AABB>(id, AABB{});
				bossDead = true;
			}
		}
	}

	//if the boss is not dead 
	if (bossDead == false) {
		for (auto const& e : ecs.getEntityNames())
		{
			float pos[] = { 0,0,0 };
			if (e.second == "MushroomBoss" && ecs.entityHas<Com_Health>(e.first) && ecs.getComponent<Com_Health>(e.first).health <= 1)
			{
				//SoundClass::getInstance()->Pause(CaveBgm);
				ecs.getComponent<spriteCom>(id).isVisible = true;
				//SoundClass::getInstance()->setArgsAndPlay(winSound, pos);
				//ecs.addComponent<AABB>(id, AABB{});
				bossDead = true;
			}
		}
	}
	
	

}



void TeleportScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> TeleportScript::createCopy()
{
	return std::make_shared <TeleportScript>();
}

std::string TeleportScript::getScriptName()
{
	return name;
}