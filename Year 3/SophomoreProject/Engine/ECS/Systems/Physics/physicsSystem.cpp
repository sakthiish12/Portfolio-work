// /******************************************************************************/
// /*!
// \file         physicSystem.cpp
// \project name Engine
// \author(s)    Poh Chun Ren, 51%, 106 + 181(seg)
// \co-author(s) Ho Zi Xing, 49%, 100
// \date         8th Oct, 2021
// \brief        Physics system for ecs that handles movement and collision
// \lines        206 Lines (Engine)
// \lines        387 Lines (segment proof) to 172 (episode)
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <iostream>
#include "PhysicsSystem.h"
#include "../../ECS.h"
#include "CollisionDetection.h"
#include "../../Graphics/Source/GraphicsDebugging.h"//for debugging
#include "../../EditorStateManager/gameStateManager.h"
#include "../../Debugging/PhysicsDebugging.h"
#include "../../InputMgr/InputMgr.h"
#include "../controllerSystem.h"
#include "CollisionSystem.h"
#include "ImplicitGrid.h"
 

/*
* 
* Note: if there is every any bugy thing related to NAN or position etc going NAN, possibly due to divide by 0, check those areas
* 
* 
*/

using std::cout;
using std::endl;

std::shared_ptr<physicsSystem> physicsSystemsPtr;

physicsSystem::physicsSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{
	gameStateManager::setPhysicsTimeStepFixedness(true);
	setDebugPhysics(false);
}

physicsSystem::~physicsSystem()
{
}

void physicsSystem::init()
{
	ecs.setSystemComponentRequirement<physicsSystem, transform2D>();
	ecs.setSystemComponentRequirement<physicsSystem, rigidBody2D>();
}

void physicsSystem::update(float dt_in)
{
	//Using accumulated DeltaTime to simulate stepped fixed dt if true
	//This will run the physic multiple time then let the renderer render it once.
	if (gameStateManager::usingFixedDTTimestep)
	{
		dt = static_cast<float>(gameStateManager::fixedDeltaTime);
		for (int step = 0; step < gameStateManager::currentNumberOfSteps; ++step)
		{
			physicsSteps();
		}
	}
	else
	{
		dt = dt_in;
		physicsSteps();
	}

	
	//draw once if debug drawing is on
	if (debugPhysics)
	{
		//draw colliders because collision resolution affected some position
		collisionSysInstance->updateAllColliders();
		drawPhysicDebugging();
	}

	// Clear all forces
	for (auto const& e : getEntitiesRegistered())
	{
		auto& rc = ecs.getComponent<rigidBody2D>(e);
		rc.forceManager.clearForce();
	}
}

void physicsSystem::setDebugPhysics(bool state)
{
	debugPhysics = state;
}

bool physicsSystem::getDebugPhysics()
{
	return debugPhysics;
}


void physicsSystem::motionDynamics()
{
	float dampingFactor = 1.0f - 0.95f;
	float frameDamping = powf(dampingFactor, dt);

	for (auto const& e : getEntitiesRegistered())
	{
		auto& tc = ecs.getComponent<transform2D>(e);
		auto& rc = ecs.getComponent<rigidBody2D>(e);
		rc.prev_pos = tc.pos;

		//skips if entity is deactivated
		if (ecs.entityHas<Com_Health>(e) && ecs.getComponent<Com_Health>(e).isActive == false)
			continue;

		//skips static rigidbody, also represented with 0 mass, or 0 inverse mass
		if (rc.Type == rigidBody2D::Types::Static || rc.InvMass == 0.0f)
			continue;
		
		//integrating force to velocity
		rc.forceManager.ComputeForce(dt); //sum up active forces
		rc.forceManager.ApplyConstaintOnForce(rc.freezeX, rc.freezeY, rc.freezeR);
		rc.forceManager.IntegrateVelocity(rc.vel, rc.InvMass, dt, rc.isGravityOn, rc.rot_vel, rc.InvInertiaMass); //integrate to get velocity
				
		//damping or friction
		rc.vel *= frameDamping;
		
		//update position
		tc.pos += vec3(rc.vel * dt, 0.f);

		//update rotation
		if (rc.isRotatable)
		{
			tc.rotation += rc.rot_vel * dt;

			//limit rotation
			if (tc.rotation > 360.f)
				tc.rotation -= 360.f;
			else if (tc.rotation < 0)
				tc.rotation += 360.f;
		}
	}
}

void physicsSystem::suction(float angleFromPlayer, transform2D& pTrans, const entityID& pID, float arc_angle, float magLimit, float magKillLimit, float suctionStrength)
{
	if (!ControllerSystem->suctionActive)
		return;

	float cosRot = cos(angleFromPlayer);
	float sinRot = sin(angleFromPlayer);
	vec3 targetVector{ 0.f, 1.f * magLimit, 0.f };//need to form a vec towards orientation
	Matrix3 rotate{
		cosRot, sinRot, 0.f,
		-sinRot, cosRot, 0.f,
		0.f, 0.f, 1.f
	};
	targetVector = rotate * targetVector;
	vec3 targetPosTemp = pTrans.pos + targetVector;
	vec2 targetPos = { targetPosTemp.x , targetPosTemp.y };

	vec3 killVector{ 0.f, 1.f * magKillLimit, 0.f };//need to form a vec towards orientation
	killVector = rotate * killVector;
	vec3 killPosTemp = pTrans.pos + killVector;
	vec2 killPos = { killPosTemp.x , killPosTemp.y };
	//commented away for presentation - to be uncommented once there is some sort of visual feedback
	//if (debugPhysics)
	{
		PhysicsDebugging::drawArcRange(angleFromPlayer, pTrans, arc_angle, magLimit);
		//show suction range
		PhysicsDebugging::drawSuctionLimit(pTrans, targetPos);
		//show killing range
		//PhysicsDebugging::drawSuctionLimit(pTrans, killPos);
	}

	for (auto const& e : getEntitiesRegistered())
	{
		if (e == pID)
			continue;

		//Check if its only enemy and enemy range
		if (ecs.entityHas<Com_type>(e))
		{
			auto& ct = ecs.getComponent<Com_type>(e);
			if (ct.type != Com_type::type::enemy && ct.type != Com_type::type::enemyrange)
				continue;
		}

		//DO dot product to check if its suckable
		auto tc = ecs.getComponent<transform2D>(e);
		if (!checkIfInSuctionArc(pTrans, tc, targetVector, arc_angle))
			continue;

		if (ecs.entityHas<Com_Health>(e) && ecs.getComponent<Com_Health>(e).isActive == false)
		{
			continue;
		}

		//check if killable
		Vec2 suckingVector = tc.pos - pTrans.pos;
		//Check if this works, use this instead if work coz less sqrt call
		//if (Length(suckingVector) < magKillLimit && ecs.entityHas<Com_Health>(e))
		if (checkIfInSuctionArc(pTrans, tc, killVector, arc_angle) && ecs.entityHas<Com_Health>(e))
		{
			//make the entity fly to netherland
			//tc.pos = { 10000.f, 10000.f, tc.pos.z };
			//ecs.getComponent<Com_Health>(e).isActive = false;
		

			ecs.deleteEntity(e);
			//std::cout << "KILL" << std::endl;
			continue;
		}

		//if it is, do ray cast and see if its blocked by obstacle
		//TODO
		//if it is not blocked, apply force

		

		auto& rc = ecs.getComponent<rigidBody2D>(e);
		//Find vector that move towards player
		Force suctionForce{Force::ForceType::LINEAR_FORCE, -suckingVector * suctionStrength, 1000};
		rc.forceManager.AddLinearForce("Suction", suctionForce);
	}
	ControllerSystem->suctionActive = false;
}

void physicsSystem::drawPhysicDebuggingEditorMode()
{
	collisionSysInstance->updateAllColliders(); // update to see
	collisionSysInstance->collidersDebugDraw();
}

bool physicsSystem::checkIfInSuctionArc(transform2D& playerT, transform2D& entityT, vec2 target, float arc_angle)
{
	//2 conditions:
	vec2 dist = entityT.pos - playerT.pos;
	if (Dot(dist, dist) >= Dot(target, target))
		return false;
	if (Dot(target, dist) / (Length(target) * Length(dist)) <= cos(arc_angle))
		return false;
	return true;
}

void physicsSystem::drawPhysicDebugging()
{
	for (auto const& e : getEntitiesRegistered())
	{
		if (ecs.entityHas<Com_Health>(e))
		{
			if (ecs.getComponent<Com_Health>(e).isActive == false)
				continue;
		}
		auto& tc = ecs.getComponent<transform2D>(e);
		auto& rc = ecs.getComponent<rigidBody2D>(e);

		//draw velocity
		PhysicsDebugging::drawDebugVelocity(tc, rc);
	}

	//draw colliders and implict grids
	collisionSysInstance->collidersDebugDraw();
}

void physicsSystem::physicsSteps()
{
	/*
	* Apply force, Update vel, Update pos
	* Detect collision
	* Solve constraints
	*/
	motionDynamics();
	collisionSysInstance->combineColliderRegisteredEntity(); //This is to combine aabb and poly collider
	collisionSysInstance->updateAllColliders();
	collisionSysInstance->broadPhase();
	collisionSysInstance->narrowPhase(dt);
	//commented away temporary for push to github
	collisionSysInstance->collisionResolution(dt);
	//collisionResolution();
}
