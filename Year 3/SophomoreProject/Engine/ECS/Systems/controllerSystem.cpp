// /******************************************************************************/
// /*!
// \file         controllerSystem.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "controllerSystem.h"
#include "../ECS.h"

#include "../../InputMgr/InputMgr.h"
#include "../Systems/Physics/physicsSystem.h"
#include "../../Mono/Mono.h"
#include "../../Sound/SoundManager.h"//sound manager
#include "../../ECS/Systems/Physics/Ray.h"
#include "../../Editor/Editor.h"//editor state TODO : remove 
#include <cmath>

#ifdef _DEBUG

#include <iostream>
using std::endl;
using std::cout;
bool god_mode = false;

#endif

//could change into variable for change in runtime?
#define PLAYER_MAGNITUDE 800.f
#define PLAYER_MAGNITUDE_SPEED_BOOST 1400.f
bool playerSpeedToggle = false;

std::shared_ptr<controllerSystem> controllerSystemPtr;

controllerSystem::controllerSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{
}

void controllerSystem::init()
{
	//if confirm that controller do not change for each game state,
	//possible to try init and save the entity id to the system
	//then can straight call the entity id from here Maybe??

}

void controllerSystem::inputUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	/*for (auto const& e : getEntitiesRegistered())
	{
		auto& cc = ecs.getComponent<controller>(e);

		//might need to try catch here??
		auto& rb = ecs.getComponent<rigidBody2D>(e);

		auto& tc = ecs.getComponent<transform2D>(e);

		if (KeyManager.isKeyPressed('I'))
		{
			//start using old physics
			physicsSystemsPtr->oldPhy = true;
		}
		else if (KeyManager.isKeyPressed('O'))
		{
			//off old phy
			physicsSystemsPtr->oldPhy = false;
		}

		//Movement Keys
		if (KeyManager.isKeyPressed('W') || KeyManager.isKeyPressed('A') ||
			KeyManager.isKeyPressed('S') || KeyManager.isKeyPressed('D'))
		{
			if (KeyManager.isKeyTriggered('W') || KeyManager.isKeyTriggered('A') ||
				KeyManager.isKeyTriggered('S') || KeyManager.isKeyTriggered('D')){

				//SoundManager::GetInstance().playPlayerWalking();//play the player walking sound 
			}

			vec2 v = { 0.f, 0.f };

			if (KeyManager.isKeyPressed('W'))
			{
				cc.up = true;
				if (playerSpeedToggle != true) {
					v.y += PLAYER_MAGNITUDE;
				}
				else {
					v.y += PLAYER_MAGNITUDE_SPEED_BOOST;
				}
			}
			else
			{
				cc.up = false;
			}

			if (KeyManager.isKeyPressed('S'))
			{
				cc.down = true;
				if (playerSpeedToggle != true) {
					v.y += -PLAYER_MAGNITUDE;
				}
				else {
					v.y += -PLAYER_MAGNITUDE_SPEED_BOOST;
				}
			}
			else
			{
				cc.down = false;
			}

			if (KeyManager.isKeyPressed('A'))
			{
				cc.left = true;
				if (playerSpeedToggle != true) {
					v.x += -PLAYER_MAGNITUDE;

				}
				else {
					v.x += -PLAYER_MAGNITUDE_SPEED_BOOST;
				}
			}
			else
			{
				cc.left = false;
			}

			if (KeyManager.isKeyPressed('D'))
			{
				cc.right = true;
				if (playerSpeedToggle != true) {
					v.x += PLAYER_MAGNITUDE;
				}
				else {
					v.x += PLAYER_MAGNITUDE_SPEED_BOOST;
				}
			}
			else
			{
				cc.right = false;
			}
			Force newForce(Force::ForceType::LINEAR_FORCE, v, dt);
			rb.forceManager.setMaxVel(1000.f);//setting the max velocity
			rb.forceManager.setLinearDrag(0.1f);//setting the linear drag so that it doesn't slide 
			rb.forceManager.AddLinearForce("movement", newForce);
		}

		//rotation based on mouse.
		vec2 playerToMouse = MouseManager.getWorldPosition() - vec2{ tc.pos.x, tc.pos.y};
		vec2 up = { 0.f, 1.f };
		float angle;
		//getting angle
		if (playerToMouse.x > 0)
		{
			if (playerToMouse.y > 0)
			{
				//top right
				angle = atan(playerToMouse.x / playerToMouse.y);
			}
			else
			{
				//bottom right
				angle = static_cast<float>(3.14) - atan(playerToMouse.x / -playerToMouse.y);
			}
		}
		else
		{
			if (playerToMouse.y > 0)
			{
				//top left
				angle = static_cast<float>(2*3.14) - atan(-playerToMouse.x / playerToMouse.y);
			}
			else
			{
				//bottom left
				angle = static_cast<float>(1*3.14) + atan(-playerToMouse.x / -playerToMouse.y);
			}
		}
		//tc.rotation = angle;
		
		//Mouse button, rotation based on mouse
		if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON))
		{
			//apply suction
			suctionActive = true;
			PhysicsSystem->suction(angle, tc, e, arc_angle, suctionLimit, suctionKillLimit, suctionStrenth);
			if (Editor::GetInstance().isPlaying == true) {
				//SoundManager::GetInstance().playSuctionSound();//plays the suction sound 
			}
			//create ray based on degree "arc ray"
		}

#ifdef _DEBUG
		//Debug key
		if (KeyManager.isKeyPressed('Y', true))
		{
			PhysicsSystem->setDebugPhysics(false);
		}
		else if (KeyManager.isKeyPressed('U', true))
		{
			PhysicsSystem->setDebugPhysics(true);
		}

		//God mode
		if (KeyManager.isKeyReleased(VK_F10))
		{
			if (!god_mode)
			{
				god_mode = true;
				cout << "God Mode activated" << endl;
			}
			else
			{
				god_mode = false;
				cout << "God Mode deactivated" << endl;
			}

		}
#endif
	}*/
}

void controllerSystem::update(float dt)
{
	for (auto const& e : getEntitiesRegistered())
	{
		auto& cc = ecs.getComponent<controller>(e);
		//might need to try catch here??
		auto& rb = ecs.getComponent<rigidBody2D>(e);

		//unrenferenced varaible
		static_cast<void>(cc);
		static_cast<void>(rb);

	}
	static_cast<void>(dt);
}

void controllerSystem::applySuction(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}




