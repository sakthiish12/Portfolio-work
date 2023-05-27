// /******************************************************************************/
// /*!
// \file         ForceManager.h
// \project name Engine
// \author(s)    Poh Chun Ren, 51%, 13
// \co-author(s) Ho Zi Xing, 49%, 12
// \date         8th Oct, 2021
// \brief        Physics system for ecs that handles movement and collision
// \lines        25 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

#include "../../system.h"
#define TEST_COLLIDING_FORCES_MAGNITUDE 1000.f

class physicsSystem : public systemBase
{
	bool debugPhysics = false;
	float dt; // dt of current frame
public:	

	physicsSystem(bool requireComponentMatch = false);
	~physicsSystem();
	physicsSystem(physicsSystem& rhs) = delete;
	physicsSystem(const physicsSystem& rhs) = delete;
	physicsSystem& operator=(const physicsSystem& rhs) = delete;
	physicsSystem& operator=(const physicsSystem& rhs) const = delete;

	void init();
	void update(float dt);

	void setDebugPhysics(bool state);
	bool getDebugPhysics();

	void motionDynamics(); //handle force, vel and update pos
	
	bool oldPhy = false;
	/*PlayerTransform, PlayerID*/
	void suction(float angleFromPlayer,transform2D& pTrans, const entityID& pID, float arc_angle = 1.f / 4 * 3.14f, float magLimit = 200.f, float magKillLimit = 100.f, float suctionStrength = 100.f);
	
	void drawPhysicDebuggingEditorMode();
private:
	bool checkIfInSuctionArc(transform2D& playerT, transform2D& entityT, vec2 target, float arc_angle);
	void drawPhysicDebugging();
	void physicsSteps();
};

//#define PhysicsSystem ::physicsSystem::GetInstance()
extern std::shared_ptr<physicsSystem> physicsSystemsPtr;
#define PhysicsSystem physicsSystemsPtr

#endif //_PHYSICSSYSTEM_H_
