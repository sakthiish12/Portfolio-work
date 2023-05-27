// /******************************************************************************/
// /*!
// \file         OfficialTestState.cpp 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the test state which showcase the 2,500 obj rendering & animation with serialization
// \lines        83 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

//A sample game state.


#include "../../WinMgr/WinMgr.h"
#include "../../InputMgr/InputMgr.h"
#include "../EditorStates/gameState.h"
#include "../gameStateManager.h"
#include "../../ECS/ECS.h"

#include "../../Editor/Editor.h"//for the imgui

#include "../../Serializer/SerializeManager.h"
#include "../../Serializer/Settings/GameObjectSerializer.h"

#include "../../Graphics/Source/FontRendering.h"//font rendering 
#include "../../ECS/Systems/CameraSystem.h"//camera system 



//for batch renderer 
//#include "../../Graphics/Source/BatchRenderer2D.h"
#include "../../ECS/Systems/SerializedOfficialSystem.h" //official system to use 
#include "../../ECS/Systems/BatchRenderingSystem.h"//batching 
//#include "../../Graphics/Source/Sprite.h"

//for physics and controller system integration
#include "../../ECS/Systems/Physics/physicsSystem.h"
#include "../../ECS/Systems/controllerSystem.h"
#include "../../ECS/Systems/Physics/CollisionSystem.h"
#include "../../Sound/SoundManager.h"

SerializeOfficial g_SerializeOfficial;
material e_SerializedMaterial; //adding test material for the first entity
spriteCom e_SerializedSprite;//adding test for sprite com 
transform2D e_SerializedTransform;//adding test for the transform com 
camera2DCom e_SerializedCamera;//adding test for the camera com 


void SerializeOfficial::load()
{
	//TextRendering::TextRenderer::GetInstance().InitFont("Resources/fonts/arial.ttf");//load text renderer 

}

void SerializeOfficial::init()
{
	/***************************
	@ Brief - init ecs
	****************************/
	SerializedSprite2DSys = ecs.registerSystem<SerializedSprite2DSystem>();
	ecs.setSystemComponentRequirement<SerializedSprite2DSystem, spriteCom>();//setting up sprite component
	//registering render system
	SerializedRenderer2DSys = ecs.registerSystem<SerializedRenderer2DSystem>();
	ecs.setSystemComponentRequirement<SerializedRenderer2DSystem, material>();//setting up renderer component
	//register transform system
	SerializedTransform2DSys = ecs.registerSystem<SerializedTransform2DSystem>();
	ecs.setSystemComponentRequirement<SerializedTransform2DSystem, transform2D>();//setting up transform component
	//register camera system
	SerializedCamera2DSys = ecs.registerSystem<SerializedCamera2DSystem>();
	ecs.setSystemComponentRequirement<SerializedCamera2DSystem, camera2DCom>();//setting up camera component
	//registerring physic system
	physicsSystemsPtr = ecs.registerSystem<physicsSystem>();
	ecs.setSystemComponentRequirement<physicsSystem, transform2D>();//setting up required component
	ecs.setSystemComponentRequirement<physicsSystem, rigidBody2D>();//setting up required component
	physicsSystemsPtr->init(); //init for running in step mode
	//registerring controller system
	controllerSystemPtr = ecs.registerSystem<controllerSystem>();
	ecs.setSystemComponentRequirement<controllerSystem, controller>();
	ecs.setSystemComponentRequirement<controllerSystem, rigidBody2D>();
	//registerring collision system
	//collisionSystemsPtr = ecs.registerSystem<CollisionSystem>();
	//ecs.setSystemComponentRequirement<CollisionSystem, transform2D>();
	//ecs.setSystemComponentRequirement<CollisionSystem, ICollider>();
	
	//register component not required in any system
	ecs.registerComponentType<AABB>();
	ecs.registerComponentType<Poly>();

	//ATTEMPT AT OBJECT DESERIALISATION
	ecs.createEntityFromFile("Config/PresetEntities/sampleBackground.json"); //creates the background 
	ecs.createEntityFromFile("Config/PresetEntities/samplePlayer.json"); //creates the player 
	
	//for (size_t i{ 0 }; i < 2500; ++i) {
	//	ecs.createEntityFromFile("Config/PresetEntities/sampleEntity.json"); //creates the 2,500 pepe 
	//}
}

void SerializeOfficial::input(float dt)
{
	static_cast<void>(dt);//unreferenced variable
	WindowsManager.Update(dt);
	InputManager.update(dt);
	ControllerSystem->inputUpdate(dt);

	//Debug key
	if (KeyManager.isKeyPressed('Y', true))
	{
		PhysicsSystem->setDebugPhysics(false);
	}
	else if (KeyManager.isKeyPressed('U', true))
	{
		PhysicsSystem->setDebugPhysics(true);
	}
}

void SerializeOfficial::update(float dt)
{
	static_cast<void>(dt);//unreferenced variable
	physicsSystemsPtr->update(dt);
	//collisionSystemsPtr->update(dt);
	if (KeyManager.isKeyPressed(VK_ESCAPE)) {
		gameStateManager::quitGame();//quit game
		return;
	}

	if (KeyManager.isKeyReleased('C', true)) {
		gameStateManager::setNextState(&g_physicTestState);
	}
	SerializedTransform2DSys->update(dt);//transform update 
	SerializedSprite2DSys->update(dt);//sprite update 
	SerializedCamera2DSys->update(dt);//camera update
	
}

void SerializeOfficial::render(float dt)
{
	SerializedRenderer2DSys->update(dt);//renderer update 
	//putt here in case need,based on physic test state
	/*SwapBuffers(WindowsManager.getWindowDC());
	glClear(GL_COLOR_BUFFER_BIT);*/
}

void SerializeOfficial::free()
{
	ecs.resetECS(); //resetting ecs systems
}

void SerializeOfficial::unload()
{

}
