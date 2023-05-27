// /******************************************************************************/
// /*!
// \file         BlenderOutputScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function defintion of the blender output script 
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

#include "../../../Engine/InputMgr/InputMgr.h"//input mgr
#include "BlenderOutputScript.h"
#include "../../../Engine/AssetManager/UpgradeManager.h"
#include "../../../Engine/AssetManager/AssetManager.h"
#include <iostream>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;
constexpr float rotated_angle_offset = 0.785f;

BlenderOutputScript::BlenderOutputScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BlenderOutputScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void BlenderOutputScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting transform com 

		//if key q is triggered, spawn the enemies down from the pipe
		if (KeyManager.isKeyTriggered('f', true)) {
			if (UpgradeManager::GetInstance().numberOfTomatoesToDeduct != 0) {
				audio.playPannedSoundEffect(audio.sfxBlenderDispense);
				entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/blenderTomato.json");
				ecs.getComponent<transform2D>(newID).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(newID).pos.y = oTransform.pos.y;
				ecs.getComponent<spriteCom>(newID).isVisible = true;


				////Shift focus to new entity
				rigidBody2D eRigidBody;
				eRigidBody.isGravityOn = true;
				eRigidBody.forceManager._gravity = Mathf::vec2(0.0,-300.f);
				eRigidBody.restitution = 0.f;//setting resitution
				ecs.addComponent<rigidBody2D>(newID, eRigidBody);//adding the rigidbody forces to the entity
				Poly ePoly;
				ePoly.angle_offset = rotated_angle_offset; //45 degree
				ecs.addComponent<Poly>(newID, ePoly);
				AssetManager::GetInstance().tomatoesCollected--;//decrement the number of tomatoes collected 
				UpgradeManager::GetInstance().numberOfTomatoesToDeduct--;
			}
			else if (UpgradeManager::GetInstance().numberOfMushroomToDeduct != 0) {
				audio.playPannedSoundEffect(audio.sfxBlenderDispense);
				entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/blenderMushroom.json");
				ecs.getComponent<transform2D>(newID).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(newID).pos.y = oTransform.pos.y;
				ecs.getComponent<spriteCom>(newID).isVisible = true;

				////Shift focus to new entity
				rigidBody2D eRigidBody;
				eRigidBody.isGravityOn = true;
				eRigidBody.forceManager._gravity = Mathf::vec2(0.0, -300.f);
				eRigidBody.restitution = 0.f;//setting resitution
				ecs.addComponent<rigidBody2D>(newID, eRigidBody);//adding the rigidbody forces to the entity
				Poly ePoly;
				ePoly.angle_offset = rotated_angle_offset; //45 degree
				ecs.addComponent<Poly>(newID, ePoly);
				AssetManager::GetInstance().mushroomsCollected--;//decrement the number of tomatoes collected 
				UpgradeManager::GetInstance().numberOfMushroomToDeduct--;
			}
			else if (UpgradeManager::GetInstance().numberOfChilliesToDeduct != 0) {
				audio.playPannedSoundEffect(audio.sfxBlenderDispense);
				entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/blenderChillies.json");
				ecs.getComponent<transform2D>(newID).pos.x = oTransform.pos.x;
				ecs.getComponent<transform2D>(newID).pos.y = oTransform.pos.y;
				ecs.getComponent<spriteCom>(newID).isVisible = true;

				////Shift focus to new entity
				rigidBody2D eRigidBody;
				eRigidBody.isGravityOn = true;
				eRigidBody.forceManager._gravity = Mathf::vec2(0.0, -300.f);
				eRigidBody.restitution = 0.f;//setting resitution
				ecs.addComponent<rigidBody2D>(newID, eRigidBody);//adding the rigidbody forces to the entity
				Poly ePoly;
				ePoly.angle_offset = rotated_angle_offset; //45 degree
				ecs.addComponent<Poly>(newID, ePoly);
				AssetManager::GetInstance().chilliesCollected--;//decrement the number of tomatoes collected 
				UpgradeManager::GetInstance().numberOfChilliesToDeduct--;
			}
		}
	}
}

void BlenderOutputScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	UpgradeManager::GetInstance().clear();//clearing all the to deduct 
}

std::shared_ptr<scriptBase> BlenderOutputScript::createCopy()
{
	return std::make_shared <BlenderOutputScript>();
}

std::string BlenderOutputScript::getScriptName()
{
	return name;
}






