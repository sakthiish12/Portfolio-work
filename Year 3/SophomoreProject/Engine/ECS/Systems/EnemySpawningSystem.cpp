// /******************************************************************************/
// /*!
// \file         EnemySpawningSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions of the enemy spawning system 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "EnemySpawningSystem.h"//enemy spawning 
#include "../../InputMgr/InputMgr.h"//input

std::shared_ptr<EnemySpawningSystem> EnemySpawningSysPtr;

void EnemySpawningSystem::update(float dt) {

	for (auto const& e : getEntitiesRegistered()) {
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting transform com 
		auto& oEnemySpawner = ecs.getComponent< Com_EnemySpawner>(e);//getting the enemy spawner 

		if (oEnemySpawner.currentTimer <= 0.f) {
			oEnemySpawner.currentTimer = oEnemySpawner.timeDelay;//resetting the current timer for the new spawn 
			//if there are still enemies to spawn 
			if (oEnemySpawner.numOfSpawns != 0) {
				//depending on the type of spawner, spawns the enemy 
				switch (oEnemySpawner.typeOfEnemy)
				{
				case oEnemySpawner.tomato: {
					entityID newID = ecs.createEntityFromFile	//creating the enemy
					("Resources/PresetEntities/TomatoNew.json");
					ecs.renameEntity(newID, "TomatoNew");
					ecs.getComponent<transform2D>(newID).pos = oTransform.pos;//setting the position to the spawner position
					break;
				}
				case oEnemySpawner.onion: {
					entityID newID = ecs.createEntityFromFile
					("Resources/PresetEntities/OnionNew.json");
					ecs.renameEntity(newID, "OnionNew");
					ecs.getComponent<transform2D>(newID).pos = oTransform.pos;
					break;
				}
				case oEnemySpawner.pumpkin: {
					entityID newID = ecs.createEntityFromFile
					("Resources/PresetEntities/PumpkinNew.json");
					ecs.renameEntity(newID, "PumpkinNew");
					ecs.getComponent<transform2D>(newID).pos = oTransform.pos;
					break;
				}
				case oEnemySpawner.chilli: {
					entityID newID = ecs.createEntityFromFile
					("Resources/PresetEntities/ChiliNew.json");
					ecs.renameEntity(newID, "ChiliNew");
					ecs.getComponent<transform2D>(newID).pos = oTransform.pos;
					break;
				}
				default:
					break;
				}

				oEnemySpawner.numOfSpawns--;//decrement the number of spawns
			}
		}
		else {
			oEnemySpawner.currentTimer -= dt;//decrement the current timer by dt 
		}

	}
}