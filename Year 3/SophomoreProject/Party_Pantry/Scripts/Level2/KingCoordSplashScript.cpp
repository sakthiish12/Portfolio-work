// /******************************************************************************/
// /*!
// \file         BridgeCutSceneScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date		 4th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the teleporting to enter the cave 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "KingCoordSplashScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include <iostream>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;
KingCoordSplashScript::KingCoordSplashScript(const char* scriptName) :
	scriptBase(scriptName)
{
}
int counter2 = 0;
void KingCoordSplashScript::init(float dt, unsigned int idToBeSet)
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
}

void KingCoordSplashScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

		auto& eInnerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 

		//if the player is near to the blender, show the dialogue , AABB
		if (eInnerTransform.pos.x > (oTransform.pos.x - oTransform.scale.x / 2.f) && eInnerTransform.pos.x < (oTransform.pos.x + oTransform.scale.x / 2.f) &&
			eInnerTransform.pos.y >(oTransform.pos.y - oTransform.scale.y / 2.f) && eInnerTransform.pos.y < (oTransform.pos.y + oTransform.scale.y / 2.f) && completed == false) {

			active = true;

		}

		//check if active 
		if (active) {
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//pause the game 
			UiLayer::GetInstance().dialoguePause = true;//pause everything 
			if (createdEntity == false) {
				//show the cutscene 
				cutSceneEntity = ecs.createEntityFromFile("Resources/SavedEntities/KingCoordSplashScreen.json");

				if (counter2 == 0) {
					audio.changeBGM(audio.bgmOldBossFight);
					counter2 = 1;
				}
				//setting as chil
				ecs.setEntityParent(cutSceneEntity, playerID);
				createdEntity = true;
			}

			if (createdEntity) {
				auto& splashSprite = ecs.getComponent<spriteCom>(cutSceneEntity);//getting sprite 
				//check check if data has been loaded in 
				if (splashSprite.frameData.size() != 0) {

					if (splashSprite.animationData.find("splash") != splashSprite.animationData.end()) {
						splashSprite.endFrame = splashSprite.animationData.find("splash")->second.back();//getting the last frame
						splashSprite.startFrame = splashSprite.animationData.find("splash")->second[0];//setting the start frame
					}

					//animation loop
					frameTime -= dt;
					if (frameTime <= 0.0f)
					{
						//Go to this animation if not within the range of the current animation
						if (!(splashSprite.currentFrame >= splashSprite.startFrame && splashSprite.currentFrame <= splashSprite.endFrame))
						{
							splashSprite.currentFrame = splashSprite.startFrame;
						}

						if (splashSprite.currentFrame == splashSprite.endFrame) {
							ecs.deleteEntity(cutSceneEntity);
							UiLayer::GetInstance().dialoguePause = false;//pause everything 
							UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//back to playing 
							active = false;
							completed = true;
							counter2 = 0;
						}
						else {
							splashSprite.currentFrame++;
						}
						frameTime = 0.2f;
					}
					splashSprite.currentTexCoords = splashSprite.frameData[splashSprite.currentFrame];//getting the specific frame data
				}
			}
		}
	}
}

void KingCoordSplashScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> KingCoordSplashScript::createCopy()
{
	return std::make_shared <KingCoordSplashScript>();
}

std::string KingCoordSplashScript::getScriptName()
{
	return name;
}






