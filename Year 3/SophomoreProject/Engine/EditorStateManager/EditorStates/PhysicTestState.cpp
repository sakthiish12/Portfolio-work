
//STL library
#include <iostream>
//#include <random>
//#include <chrono>
#include <memory>

//Essential Gamestate/ECS
#include "../EditorStates/gameState.h"
#include "../gameStateManager.h"
#include "../../ECS/ECS.h"

#include "../../WinMgr/WinMgr.h"
#include "../../InputMgr/InputMgr.h"

//ECS Systems
//#include "../../Editor/Editor.h"//for the imgui
#include "../../ECS/Systems/Physics/physicsSystem.h"
#include "../../ECS/Systems/controllerSystem.h"

physicsTestState g_physicTestState;

void physicsTestState::load()
{

}

void physicsTestState::init()
{
	//TEST 0: Initialise ECS
	
	//registerring physic system
	physicsSystemsPtr = ecs.registerSystem<physicsSystem>();
	ecs.setSystemComponentRequirement<physicsSystem, transform2D>();//setting up required component -- do we need a transform sys??
	ecs.setSystemComponentRequirement<physicsSystem, rigidBody2D>();//setting up required component
	//ecs.setSystemComponentRequirement<physicsSystem, AABB>();//setting up required component
	//ecs.setSystemComponentRequirement<physicsSystem, Ray>();//setting up required component
	//ecs.setSystemComponentRequirement<physicsSystem, Circle>();//setting up required component

	physicsSystemsPtr->init();

	ControllerSystem = ecs.registerSystem<controllerSystem>();
	ecs.setSystemComponentRequirement<controllerSystem, controller>();
	ecs.setSystemComponentRequirement<controllerSystem, rigidBody2D>();

	//system setup END

	//Then reserve vectors. Do this only after registering systems
	//ecs.reserveAll(2); //testing to create 1 entities 

	


	ecs.registerComponentType<AABB>();



	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	ecs.createEntityFromFile("Config/PresetEntities/PhyTest0.json"); //creates the player 
	ecs.createEntityFromFile("Config/PresetEntities/PhyTest1.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest2.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest3.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest4.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest5.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest6.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest7.json"); //creates the player 
	//ecs.createEntityFromFile("Config/PresetEntities/PhyTest8.json"); //creates the player 

	//for (size_t i{ 0 }; i < 2500; ++i) {
	//	ecs.createEntityFromFile("Config/PresetEntities/sampleEntity.json"); //creates the 2,500 pepe 
	//}




	//transform2D pos;
	//pos.pos;
	//pos.rotation;
	//pos.scale = { 200, 200 }; //strength to divide by 2
	//ecs.addComponent<transform2D>(9, pos);

	////inverted Triangle model for suction
	//Poly triCollider;
	//triCollider.models.push_back({ 0.f, 0.f });
	//triCollider.models.push_back({ 0.5f, 0.5f });
	//triCollider.models.push_back({ -0.5f, 0.5f });
	//ecs.addComponent<Poly>(9, triCollider);





	//controller PlayerController;
	//ecs.addComponent<controller>(0, PlayerController);

	///*Transform*/
	//transform2D t;
	//t.pos = { 500.f, 500.f };
	//t.rotation = 0.f;
	//t.scale = { 100.f, 100.f };

	////player
	//ecs.addComponent<transform2D>(0, t);
	//
	////AABB
	//t.pos = { 500.f, 200.f };
	//ecs.addComponent<transform2D>(1, t);
	//t.pos = { 800.f, 600.f };
	//ecs.addComponent<transform2D>(2, t);
	//t.pos = { 800.f, 400.f };
	//ecs.addComponent<transform2D>(3, t);
	//t.pos = { 300.f, 200.f };
	//ecs.addComponent<transform2D>(4, t);
	//
	////POLYGON
	//t.pos = { 200.f, 800.f };
	//ecs.addComponent<transform2D>(5, t);
	//t.pos = { 190.f, 600.f };
	//ecs.addComponent<transform2D>(6, t);
	//t.pos = { 200.f, 300.f };
	//ecs.addComponent<transform2D>(7, t);
	//t.pos = { 200.f, 100.f };
	//ecs.addComponent<transform2D>(8, t);

	///*PhysicSys*/
	//rigidBody2D rb;
	//rb.Mass = 10.f; //need to not 0
	//rb.InvMass = 1 / rb.Mass; //calculated
	//rb.isGravityOn = false;
	//rb.Type = rigidBody2D::Types::Kinametics;
	//ecs.addComponent<rigidBody2D>(0, rb);
	//rb.Type = rigidBody2D::Types::Dynamics;
	//ecs.addComponent<rigidBody2D>(1, rb);
	//ecs.addComponent<rigidBody2D>(2, rb);
	//ecs.addComponent<rigidBody2D>(3, rb);
	//ecs.addComponent<rigidBody2D>(4, rb);

	////POLYGON
	//rb.isGravityOn = true;
	//ecs.addComponent<rigidBody2D>(5, rb);
	//ecs.addComponent<rigidBody2D>(6, rb);
	//ecs.addComponent<rigidBody2D>(7, rb);
	//rb.Type = rigidBody2D::Types::Static;
	//ecs.addComponent<rigidBody2D>(8, rb);
	//rb.Type = rigidBody2D::Types::Kinametics;

	//AABB boundingBox;
	//boundingBox.scale = t.scale;
	//ecs.addComponent<AABB>(0, boundingBox);
	//ecs.addComponent<AABB>(1, boundingBox);
	//ecs.addComponent<AABB>(2, boundingBox);
	//ecs.addComponent<AABB>(3, boundingBox);
	//boundingBox.offset_pos = { boundingBox.scale.x * 0.25f, boundingBox.scale.y * 0.25f}; //will offset to top right quadrant of a normal box size
	//ecs.addComponent<AABB>(4, boundingBox);
	////ecs.addComponent<AABB>(5, boundingBox);
	////ecs.addComponent<AABB>(6, boundingBox);
	////ecs.addComponent<AABB>(7, boundingBox);
	////ecs.addComponent<AABB>(8, boundingBox);

	//Poly polygon;
	//polygon.models.push_back({ 0, 0.5 });
	//polygon.models.push_back({ -0.5, 0.1 });
	//polygon.models.push_back({ -0.3, -0.5 });
	//polygon.models.push_back({ 0.3, -0.5 });
	//polygon.models.push_back({ 0.5, 0.1 });

	//polygon.points.push_back({ 0, 0.5 });
	//polygon.points.push_back({ -0.5, 0.1 });
	//polygon.points.push_back({ -0.3, -0.5 });
	//polygon.points.push_back({ 0.3, -0.5 });
	//polygon.points.push_back({ 0.5, 0.1 });

	//ecs.addComponent<Poly>(5, polygon);
	//ecs.addComponent<Poly>(6, polygon);
	//ecs.addComponent<Poly>(7, polygon);
	//ecs.addComponent<Poly>(8, polygon);
}

void physicsTestState::input(float dt)
{
	ControllerSystem->inputUpdate(dt);
	//Reset key
	if (KeyManager.isKeyPressed('R', true))
	{
		//Reset
		
		//player
		auto& t1 = ecs.getComponent<transform2D>(0);
		t1.pos = { 500.f, 500.f ,1.f};

		//AABB
		auto& t2 = ecs.getComponent<transform2D>(1);
		t2.pos = { 500.f, 200.f ,1.f };
		auto& t3 = ecs.getComponent<transform2D>(2);
		t3.pos = { 800.f, 600.f,1.f };
		auto& t4 = ecs.getComponent<transform2D>(3);
		t4.pos = { 800.f, 400.f,1.f };
		auto& t5 = ecs.getComponent<transform2D>(4);
		t5.pos = { 300.f, 200.f,1.f };

		//POLYGON
		auto& t6 = ecs.getComponent<transform2D>(5);
		t6.pos = { 200.f, 800.f,1.f };
		auto& t7 = ecs.getComponent<transform2D>(6);
		t7.pos = { 200.f, 600.f ,1.f };
		auto& t8 = ecs.getComponent<transform2D>(7);
		t8.pos = { 200.f, 300.f,1.f };
		auto& t9 = ecs.getComponent<transform2D>(8);
		t9.pos = { 200.f, 100.f ,1.f };
	}

	if (KeyManager.isKeyPressed('N', true))
	{
		//player
		//auto& t1 = ecs.getComponent<transform2D>(0);
		//t1.pos = { 500.f, 500.f };
		auto& rb = ecs.getComponent<rigidBody2D>(0);
		Force newForce(Force::ForceType::ROTATIONAL_FORCE, { TEST_COLLIDING_FORCES_MAGNITUDE, 0.f }, dt);
		newForce.SetTorque(TEST_COLLIDING_FORCES_MAGNITUDE);
		rb.forceManager.AddLinearForce("Rotate", newForce);
	}
	else if(KeyManager.isKeyPressed('M', true))
	{
		auto& rb = ecs.getComponent<rigidBody2D>(0);
		Force newForce(Force::ForceType::ROTATIONAL_FORCE, { TEST_COLLIDING_FORCES_MAGNITUDE, 0.f }, dt);
		newForce.SetTorque(-TEST_COLLIDING_FORCES_MAGNITUDE);
		rb.forceManager.AddLinearForce("Rotate", newForce);
	}

	//Debug key
	if (KeyManager.isKeyPressed('Y', true))
	{
		PhysicsSystem->setDebugPhysics(false);
	}
	else if (KeyManager.isKeyPressed('U', true))
	{
		PhysicsSystem->setDebugPhysics(true);
	}

	//Test key t,f,g,h
	if (KeyManager.isKeyPressed('H', true))
	{
		std::cout << "'H' is pressed" << std::endl;
		auto& rb = ecs.getComponent<rigidBody2D>(4);
		//rb.Force = { 50, 0.f };
		Force newForce(Force::ForceType::LINEAR_FORCE, { TEST_COLLIDING_FORCES_MAGNITUDE, 0.f}, dt);
		rb.forceManager.AddLinearForce("movement", newForce);
	}
	//This is for testing two moving
	else if (KeyManager.isKeyPressed('F', true))
	{
		std::cout << "'F' is pressed" << std::endl;
		auto& rb = ecs.getComponent<rigidBody2D>(4);
		Force newForce(Force::ForceType::LINEAR_FORCE, { TEST_COLLIDING_FORCES_MAGNITUDE, 0.f }, dt);
		rb.forceManager.AddLinearForce("movement", newForce);

		auto& rb1 = ecs.getComponent<rigidBody2D>(1);
		newForce.setLinearForce({ -TEST_COLLIDING_FORCES_MAGNITUDE, 0.f });
		rb1.forceManager.AddLinearForce("movement", newForce);

	}
	static_cast<void>(dt);//unreferenced variable
	 if (KeyManager.isKeyPressed('G', true))
	{
		std::cout << "'G' is pressed" << std::endl;
		auto& rb = ecs.getComponent<rigidBody2D>(2);
		Force newForce(Force::ForceType::LINEAR_FORCE, { 0.f, -TEST_COLLIDING_FORCES_MAGNITUDE }, dt);
		rb.forceManager.AddLinearForce("movement", newForce);
	}
	//This is for testing two moving
	else if (KeyManager.isKeyPressed('T', true))
	{
		std::cout << "'T' is pressed" << std::endl;

		auto& rb = ecs.getComponent<rigidBody2D>(2);
		Force newForce(Force::ForceType::LINEAR_FORCE, { 0.f, -TEST_COLLIDING_FORCES_MAGNITUDE }, dt);
		rb.forceManager.AddLinearForce("movement", newForce);

		auto& rb1 = ecs.getComponent<rigidBody2D>(3);
		newForce.setLinearForce({ 0.f, TEST_COLLIDING_FORCES_MAGNITUDE });
		rb1.forceManager.AddLinearForce("movement", newForce);

	}

	 if (KeyManager.isKeyPressed('X', true))
	 {
		auto& rb1 = ecs.getComponent<rigidBody2D>(5);
		Force newForce(Force::ForceType::LINEAR_FORCE, { 0.f, -TEST_COLLIDING_FORCES_MAGNITUDE }, dt);
		rb1.forceManager.AddLinearForce("movement", newForce);

		auto& rb = ecs.getComponent<rigidBody2D>(6);
		newForce.setLinearForce({ 0.f, TEST_COLLIDING_FORCES_MAGNITUDE });
		rb.forceManager.AddLinearForce("movement", newForce);
	 }

	 if (KeyManager.isKeyPressed('Z', true))
	 {
		auto& rb = ecs.getComponent<rigidBody2D>(6);
		Force newForce(Force::ForceType::LINEAR_FORCE, { 0.f, -TEST_COLLIDING_FORCES_MAGNITUDE }, dt);
		rb.forceManager.AddLinearForce("movement", newForce);
	 }
	else if (KeyManager.isKeyReleased(VK_ESCAPE))
	{
		gameStateManager::quitGame();
	}
}

void physicsTestState::update(float dt)
{
	WindowsManager.Update(dt);
	InputManager.update(dt);
	physicsSystemsPtr->update(dt);
}

void physicsTestState::render(float dt)
{
	static_cast<void>(dt);
	SwapBuffers(WindowsManager.getWindowDC());
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

void physicsTestState::free()
{
	ecs.resetECS();
}

void physicsTestState::unload()
{
}