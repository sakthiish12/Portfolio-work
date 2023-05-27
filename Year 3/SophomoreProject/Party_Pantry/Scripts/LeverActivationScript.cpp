// /******************************************************************************/
// /*!
// \file         LeverActivationScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the lever activation script 
// \lines        51
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
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities 
#include "../../Engine/Sound/SoundManager.h"
#include "LeverActivationScript.h"
#include "../../Engine/AssetManager/UpgradeManager.h"
#include <iostream>
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"

using std::cout;
using std::endl;
LeverActivationScript* leverScriptPtr = nullptr;
LeverActivationScript::LeverActivationScript(const char* scriptName) :
	scriptBase(scriptName)
{
	leverScriptPtr = this;
}

LeverActivationScript::~LeverActivationScript()
{
	leverScriptPtr = nullptr;
}

void LeverActivationScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}
	UiLayer::GetInstance().m_currentLevelState = UiLayer::LevelState::level1;

}

void LeverActivationScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id)) {

		//auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

		if (!UpgradeManager::GetInstance().level1Completed) {

			if (textLoaded == true) {
				ecs.getComponent<spriteCom>(textId).isVisible = false;//set to invisible 
			}

			//check if hit by bullet 
			//change state to FreeFloat
			//get pushed, scale down, and die
			std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
			for (entityID& eID_collided : collidedEntities)
			{
				//Handle bullet hitting lever
				if (ecs.entityHas<Com_type>(eID_collided) && ecs.getComponent<Com_type>(eID_collided).type == Com_type::type::bullet)
				{
					//skips if player cannot see lever when bullet hit lever	
					transform2D* playerTransform = nullptr;

					//get the only registered player with abilities
					for (auto& eInner : AbilitiesSysPtr->getEntitiesRegistered()) {
						playerTransform = &ecs.getComponent<transform2D>(eInner);//getting the transform of the player 
					}
				
					int testCheck = -1; //-1 untouched, 0 lever not inside camera, 1 lever inside camera

					if (playerTransform != nullptr && activeCameraData != nullptr)
					{				//Can player see lever?
						float camwidth = activeCameraData->height * activeCameraData->aspectRatio;
					
						float camMinX = activeCameraTransform->pos.x - 0.5f * camwidth;
						float camMaxX = camMinX + camwidth;
						float camMinY = activeCameraTransform->pos.y - 0.5f * activeCameraData->height;
						float camMaxY = camMinY + activeCameraData->height;
						//cout << "Cam Scale X: " << activeCameraTransform->scale.x << endl;
						//cout << "Cam Scale X: " << activeCameraTransform->scale.y << endl;
						//cout << "camMinX: " << camMinX << endl;
						//cout << "camMaxX: " << camMaxX << endl;
						//cout << "camMinY: " << camMinY << endl;
						//cout << "camMaxY: " << camMaxY << endl;
						auto& leverTrans = ecs.getComponent<transform2D>(id);
						//cout << "activeCameraTransform Pos: " << activeCameraTransform->pos.x << ", " << activeCameraTransform->pos.y << endl;
						//cout << "leverTrans Pos: " << leverTrans.pos.x << ", " << leverTrans.pos.y << endl;
					
						if (leverTrans.pos.x < camMinX ||leverTrans.pos.x > camMaxX ||
							leverTrans.pos.y < camMinY ||leverTrans.pos.y > camMaxY)
							//player cannot see lever, thus, don't attempt to activate lever
						{
							testCheck = 0;
							//cout << "testCheck: " << testCheck << endl;
							break;
						}
						else
						{
							testCheck = 1;
						}
					}
					//cout << "testCheck: " << testCheck << endl;
					//Lever hitted
					float pos[] = { 0,0,0 };
					if (leverTriggered == false) {
						//std::cout << __func__ << " called , lever has been activated" << std::endl;
						// 
						
						//Insert Sound Here #lever
						//Insert Sound Here #bridgeExtension
						//Insert Sound Here #fightingtheboss

						//if b is activated, change the image 
						auto& oSprite = ecs.getComponent<spriteCom>(id);
						oSprite.textureId = "lever2.png";//changing to activated lever 
						leverTriggered = true;
						LeverHasBeenTriggeredStatus.Notify(leverTriggered);

						//setting the starting camera position
						activeCameraTransform->pos.x = 2069.701f;
						cameraOriginalHeight = activeCameraData->height;//setting the original camera height 
						activeCameraData->height = 120.f;
					}

				}
			}

			//if the lever is trigger
			if (leverTriggered == true) {
				//do the camera panning 
				if (cameraPanned == false) {
					activeCameraData->isFixedToPlayer = false;//disable follow player 
					activeCameraTransform->pos.y = -858.555f;
					//if have not reached the desired pos and scale, 
					if (activeCameraTransform->pos.x <= desiredCameraPosX) {
						activeCameraTransform->pos.x += cameraPanningSpeed * dt;//decreasing the pos 
					}

					if (activeCameraData->height <= desiredCameraHeight) {
						activeCameraData->height += cameraPanningHeightSpeed * dt;//increasing the height 
					}

					//if both reached 
					if (activeCameraTransform->pos.x >= desiredCameraPosX && activeCameraData->height >= desiredCameraHeight) {
						cameraPanned = true;
						activeCameraData->height = cameraOriginalHeight;//set back the original height of the camera
						activeCameraData->isFixedToPlayer = true;
					}
				}
			}

		}else {
			auto& oSprite = ecs.getComponent<spriteCom>(id);
			oSprite.textureId = "lever2.png";//changing to activated lever 
		}
	}
}

void LeverActivationScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> LeverActivationScript::createCopy()
{
	return std::make_shared <LeverActivationScript>();
}

std::string LeverActivationScript::getScriptName()
{
	return name;
}






