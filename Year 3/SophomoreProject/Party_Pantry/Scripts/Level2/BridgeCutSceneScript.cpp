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
#include "BridgeCutSceneScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/AbilitiesSystem.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include <iostream>
#include <Sound/SoundManager.h>

using std::cout;
using std::endl;
BridgeCutSceneScript::BridgeCutSceneScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BridgeCutSceneScript::init(float dt, unsigned int idToBeSet)
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

void BridgeCutSceneScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<transform2D>(id))
	{
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform component 

		auto& eInnerTransform = ecs.getComponent<transform2D>(playerID);//getting player transform 

		//if the player is near to the blender, show the dialogue , AABB
		if (eInnerTransform.pos.x > (oTransform.pos.x - oTransform.scale.x/2.f) && eInnerTransform.pos.x < (oTransform.pos.x + oTransform.scale.x/2.f) &&
			eInnerTransform.pos.y >(oTransform.pos.y - oTransform.scale.y/2.f) && eInnerTransform.pos.y < (oTransform.pos.y + oTransform.scale.y/2.f) && completed == false) {

			active = true;

		}
		
		//check if active 
		if (active) {
			UiLayer::GetInstance().dialoguePause = true;//pause everything 
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//setting the scene state to edit 
			if (createdEntity == false) {
				//show the cutscene 
				cutSceneEntity = ecs.createEntityFromFile("Resources/SavedEntities/bridgeCutscene.json");
				//setting as chil
				ecs.setEntityParent(cutSceneEntity, playerID);
				createdEntity = true;
			}

			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true && secondLoaded == false) {
				//change the picture 
				secondLoaded = true;
				ecs.getComponent<spriteCom>(cutSceneEntity).textureId = "bridgeCut2.png";
			}
			else if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true && secondLoaded == true) {
				ecs.deleteEntity(cutSceneEntity);
				UiLayer::GetInstance().dialoguePause = false;//pause everything 
				UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Play;//setting the scene state to edit 
				active = false;
				completed = true;
			}
		}
	}
}

void BridgeCutSceneScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> BridgeCutSceneScript::createCopy()
{
	return std::make_shared <BridgeCutSceneScript>();
}

std::string BridgeCutSceneScript::getScriptName()
{
	return name;
}






