// /******************************************************************************/
// /*!
// \file         AbilitiesSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the abilities system
// \lines        121
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "AbilitiesSystem.h"//abilities system
#include "../../InputMgr/InputMgr.h"//input mgr
#include "../../ECS/factory.h"//factory 
#include "../../ECS/Systems/controllerSystem.h"//controller system 
#include "../../ECS/Systems/Logic.h"
#include "../../Sound/SoundManager.h"
//#include <cmath>



constexpr float BULLET_MAGNITUDE = 2000.f;
//extern
std::shared_ptr<AbilitiesSystem> AbilitiesSysPtr;

extern bool playerSpeedToggle;//extern 
float pos[] = { 0,0,0 };
void AbilitiesSystem::update(float dt) {
	UNREFERENCED_PARAMETER(dt);//unref

#if 0
	for (auto const& e : getEntitiesRegistered()) {
		//auto& oTransform = ecs.getComponent<transform2D>(e);//getting transform com 
		auto& oType = ecs.getComponent<Com_type>(e);//getting type of the entity
		auto& oAbilities = ecs.getComponent<Com_Abilities>(e);//getting the abilites
		oType;
		static entityID shieldEntityId;//storing of the shield entity id 

		////tweaking the abilities forward
		//if (KeyManager.isKeyTriggered('e', true)) {
		//	SoundClass::getInstance()->PlayAgain(button);
		//	
		//	//if it's not the last ability
		//	if (oAbilities.typeAbilities != oAbilities.fireball) {
		//		oAbilities.typeAbilities++;//changing the abilities
		//	}
		//	else {
		//		oAbilities.typeAbilities = oAbilities.waterCannon;//set to the starting ability
		//	}
		//}		
		////tweaking the abilities backwards
		//if (KeyManager.isKeyTriggered('q', true)) {
		//	SoundClass::getInstance()->PlayAgain(button);
		//	//if it's not the last ability
		//	if (oAbilities.typeAbilities != oAbilities.waterCannon) {
		//		oAbilities.typeAbilities--;//changing the abilities
		//	}
		//	else {
		//		oAbilities.typeAbilities = oAbilities.fireball;//set to the ending ability
		//	}
		//}
		//if (oAbilities.isShieldLoaded) {
		//	ecs.getComponent<spriteCom>(shieldEntityId).isVisible = false;//make is invisible 
		//}
		//playerSpeedToggle = false;//disable the player speed 
		//if (MouseManager.isButtonPressed(MouseButton::RIGHT_BUTTON)) {
		//	switch (oAbilities.typeAbilities) {
		//		case oAbilities.superSpeed:
		//			playerSpeedToggle = true;//toggle the speed 
		//			break;
		//		case oAbilities.shield:
		//			//show the shield 
		//			if (oAbilities.isShieldLoaded == false) {
		//				//creating the shield 
		//				shieldEntityId = ecs.createEntityFromFile
		//				("Resources/PresetEntities/shield.json");
		//				oAbilities.isShieldLoaded = true;
		//			}
		//			if (oAbilities.isShieldLoaded == true) {
		//				//toggle between visiblity	
		//				ecs.getComponent<spriteCom>(shieldEntityId).isVisible = true;
		//			}
		//			break;
		//		default:
		//			break;
		//	}
		//}
		//if (oAbilities.isShieldLoaded) {
		//	ecs.getComponent<transform2D>(shieldEntityId).pos = oTransform.pos;//shield follows the player 
		//}

		//if the mouse right button is pressed 
		if(MouseManager.isButtonTriggered(MouseButton::RIGHT_BUTTON)){
			//check which abilities is currently active 
			switch (oAbilities.typeAbilities)
			{
			case oAbilities.waterCannon: {

				entityID newID = ecs.createEntityFromFile
				("Resources/PresetEntities/waterdroplet.json");
				int dec;
				dec = (rand() % 4 + 1);
				switch (dec)
				{
				case 1: {SoundClass::getInstance()->PlayAgain(airBlast1); }
						break;
				case 2: {SoundClass::getInstance()->PlayAgain(airBlast2); }
						break;
				case 3: {SoundClass::getInstance()->PlayAgain(airBlast3);  }
						break;
				case 4: {SoundClass::getInstance()->PlayAgain(airBlast4);  }
						break;
				}
				//Shift focus to new entity
				ecs.getComponent<transform2D>(newID).pos.x = ecs.getComponent<transform2D>(e).pos.x; //Apparently not using oTransform fixes this? But the camera is still acting strange.
				ecs.getComponent<transform2D>(newID).pos.y = ecs.getComponent<transform2D>(e).pos.y;
				rigidBody2D eRigidBody;
				vec2 playerToMouse = MouseManager.getWorldPosition() - vec2{ ecs.getComponent<transform2D>(e).pos.x, ecs.getComponent<transform2D>(e).pos.y } ;//getting the player to mouse vector
				playerToMouse = BULLET_MAGNITUDE * Normalize(playerToMouse);
				//set bullet dir for sprite
				ecs.getComponent<transform2D>(newID).rotation = 360.f + 90.f - getVectorRotationRadian(playerToMouse) * 180.f / 3.14f;
				Force waterDropletForce(Force::ForceType::LINEAR_FORCE, playerToMouse, 100.f);
				eRigidBody.forceManager.AddLinearForce("waterDropletForce", waterDropletForce,true);//adding linear force to the water droplet 
				eRigidBody.forceManager.setMaxVel(600.f);
				ecs.addComponent<rigidBody2D>(newID,eRigidBody);//adding the rigidbody forces to the entity
				Com_TimeDestructor eTimeDestructor;
				eTimeDestructor.timeDelayDestructor = 5.0f;//5 seconds self destruct
				ecs.addComponent<Com_TimeDestructor>(newID, eTimeDestructor);//adding the time destructor component to the entity 
				Com_type eType;
				eType.type= Com_type::type::bullet;//5 seconds self destruct
				ecs.addComponent<Com_type>(newID, eType);//adding the time destructor component to the entity 
				AABB eAABB;
				ecs.addComponent<AABB>(newID, eAABB);//adding the time destructor component to the entity
				scriptComponent eScript;
				eScript.scriptContainer["WaterShotScript"] = ScriptManager.createFromScriptDatabase("WaterShotScript");
				ecs.addComponent<scriptComponent>(newID, eScript);

				//HACK force visibility of water droplet
				ecs.getComponent<spriteCom>(newID).isVisible = true;
				break;		
			}	
			case oAbilities.fireball: {
				//factory create the fireball
				entityID newID = ecs.createEntityFromFile
				("Resources/PresetEntities/fireball.json");
				int dec;
				dec = (rand() % 4 + 1);
				switch (dec)
				{
				case 1: {SoundClass::getInstance()->PlayAgain(airBlast1); }
					  break;
				case 2: {SoundClass::getInstance()->PlayAgain(airBlast2); }
					  break;
				case 3: {SoundClass::getInstance()->PlayAgain(airBlast3);  }
					  break;
				case 4: {SoundClass::getInstance()->PlayAgain(airBlast4);  }
					  break;
				}
				//Shift focus to new entity
				ecs.getComponent<transform2D>(newID).pos.x = ecs.getComponent<transform2D>(e).pos.x;
				ecs.getComponent<transform2D>(newID).pos.y = ecs.getComponent<transform2D>(e).pos.y;
				rigidBody2D eRigidBody;
				vec2 playerToMouse = MouseManager.getWorldPosition() - vec2{ ecs.getComponent<transform2D>(e).pos.x, ecs.getComponent<transform2D>(e).pos.y };//getting the player to mouse vector 
				playerToMouse = BULLET_MAGNITUDE * Normalize(playerToMouse);
				ecs.getComponent<transform2D>(newID).rotation = 360.f + 90.f - getVectorRotationRadian(playerToMouse) * 180.f / 3.14f;
				Force fireBallForce(Force::ForceType::LINEAR_FORCE, playerToMouse, 100.f);
				eRigidBody.forceManager.AddLinearForce("fireBallForce", fireBallForce,true);//adding linear force to the water droplet 
				eRigidBody.forceManager.setMaxVel(300.f);
				ecs.addComponent<rigidBody2D>(newID, eRigidBody);//adding the rigidbody forces to the entity
				Com_TimeDestructor eTimeDestructor;
				eTimeDestructor.timeDelayDestructor = 5.0f;//5 seconds self destruct
				ecs.addComponent<Com_TimeDestructor>(newID, eTimeDestructor);//adding the time destructor component to the entity
				Com_type eType;
				eType.type = Com_type::type::bullet;//5 seconds self destruct
				ecs.addComponent<Com_type>(newID, eType);//adding the time destructor component to the entity 
				AABB eAABB;
				ecs.addComponent<AABB>(newID, eAABB);//adding the time destructor component to the entity
				scriptComponent eScript;
				eScript.scriptContainer["FireShotScript"] = ScriptManager.createFromScriptDatabase("FireShotScript");
				ecs.addComponent<scriptComponent>(newID, eScript);

				//HACK force visibility of fire shot
				ecs.getComponent<spriteCom>(newID).isVisible = true;
				break;
			}
			default:
				break;
			}
		}
	}
#endif
}