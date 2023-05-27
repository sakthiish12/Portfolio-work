// /******************************************************************************/
// /*!
// \file         TeleporterLevel1.cpp
// \project name Engine
// \author(s)    Wilfred ng
// \date         28th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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
#include "TeleporterLevel1Script.h"
#include "../../Engine/ECS/Systems/ParticleSystem/EmitterTeleporter.h"
#include "../../Engine/AssetManager/UpgradeManager.h"
#include "../../Engine/AssetManager/AssetManager.h"
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


TeleportLevel1Script::TeleportLevel1Script(const char* scriptName) :
	scriptBase(scriptName)
{
}

void TeleportLevel1Script::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	ecs.getComponent<spriteCom>(id).isVisible = false;
	state = scriptBase::ScriptState::UPDATE;

	//create EmitterComponent
	EmitterComponent EC{};
	EC.emitterContainer.insert(std::make_pair("EmitterTeleporterTrail", std::make_shared<EmitterTeleporterTrail>()));
	ecs.addComponent<EmitterComponent>(id, EC);

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerEntity = e.first;//player is found 
			break;
		}
	}

}


void TeleportLevel1Script::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref

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

			//show the dialogue 
			if (textLoaded == false && bossDead == true) {
				textId = ecs.createEntityFromFile("Resources/PresetEntities/teleporterText.json");
				//setting the position of the text 
				ecs.getComponent<transform2D>(textId).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(textId).pos.y = oTransform.pos.y + textOffSetY;
				textLoaded = true;
			}

			if (textLoaded == true && bossDead == true)
				ecs.getComponent<spriteCom>(textId).isVisible = true;//is visible 
		}

		if (eInnerTransform.pos.x > (oTransform.pos.x - radiusTeleport) && eInnerTransform.pos.x < (oTransform.pos.x + radiusTeleport) &&
			eInnerTransform.pos.y >(oTransform.pos.y - radiusTeleport) && eInnerTransform.pos.y < (oTransform.pos.y + radiusTeleport)) {

			//if text is loaded and the boss is dead 
			if (bossDead == true) {
				oSprite.textureId = "newTeleporter.png";

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
					//check if blender is collected 
					if (!AssetManager::GetInstance().blenderCollected) {
						//show the dialogue to remind player to collect blender 

						entityID eDialogue = ecs.createEntityFromFile("Resources/PresetEntities/dialgoueCollectBlender.json");
						//setting the new position 
						ecs.getComponent<transform2D>(eDialogue).pos.x = ecs.getComponent<transform2D>(playerEntity).pos.x;
						ecs.getComponent<transform2D>(eDialogue).pos.y = ecs.getComponent<transform2D>(playerEntity).pos.y;
					}
					else {
						UpgradeManager::GetInstance().level1Completed = true;//set that level 1 is completed 
						//SoundClass::getInstance()->channels[Teleporter]->setPaused(false);
						//SoundClass::getInstance()->channels[throneRoom]->setPaused(false);
						//SoundClass::getInstance()->channels[winSound]->setPaused(true);
						ecs.loadLevel("Resources/SavedLevels/ThroneRoomLevel.json");
						audio.changeBGM(audio.bgmThroneRoom);
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
	if (bossDead == false) {
		for (auto const& e : ecs.getEntityNames())
		{
			float pos[] = { 0,0,0 };
			if (e.second == "TomatoNewBoss" && ecs.entityHas<Com_Health>(e.first) && ecs.getComponent<Com_Health>(e.first).health <= 1)
			{
				//SoundClass::getInstance()->Pause(fightingtheboss);
				ecs.getComponent<spriteCom>(id).isVisible = true;
				//SoundClass::getInstance()->setArgsAndPlay(winSound, pos);
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



void TeleportLevel1Script::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> TeleportLevel1Script::createCopy()
{
	return std::make_shared <TeleportLevel1Script>();
}

std::string TeleportLevel1Script::getScriptName()
{
	return name;
}