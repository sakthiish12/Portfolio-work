// /******************************************************************************/
// /*!
// \file         CollisionSystem.h
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        Collision system -> this file only uses 3 static fn atm
// \lines        20 Lines (seg) to 40 (epis)
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _COLLISIONSYSTEM_H_
#define _COLLISIONSYSTEM_H_

#include <map>
#include <string>
#include "../../ECS.h"
#include "../../component.h"
#include "ImplicitGrid.h"

//should be ENTITY TYPE??
typedef void (*CollisionOnCB)(unsigned int e1, unsigned int e2);
typedef void (*CollisionEnterCB)(unsigned int e1, unsigned int e2);
typedef void (*CollisionExitCB)(unsigned int e1, unsigned int e2);

class CollisionSystem : public systemBase
{
    ImplicitGrid currentGrid;

    std::map<std::string, CollisionOnCB> _collisionOnResponse; //This will store all the collision call back and be activated 
    std::map<std::string, CollisionEnterCB> _collisionEnterResponse; //This will store all the collision call back and be activated 
    std::map<std::string, CollisionExitCB> _collisionExitResponse; //This will store all the collision call back and be activated 
    
    //use getregisteredentity to get only AABB collider
    //contains entity with polygon
    std::vector<entityID> RegisteredEntityWithPoly{ 0 };
    //contains all collider entity
    std::vector<entityID> AllColliderEntity{ 0 };

    bool printDebugInfo = false;
public:
    CollisionSystem();
    CollisionSystem(bool requireComponentMatch);
    ~CollisionSystem();

    void combineColliderRegisteredEntity();
    std::vector<entityID>& getAllRegisteredEntity();//AABB && Poly

    static std::shared_ptr<CollisionSystem> getInstance();

    //for now only use AABB
    void init();

    //Update to be called via physics
    void update(float dt);
    void updateAllColliders();
    void broadPhase();
    void narrowPhase(float dt);
    void collisionResolution(float dt);
    //geting container & of entity collided with
    std::vector<unsigned int> getEntityCollidedWith(unsigned int id);
    bool checkCollisionBetween(unsigned int id1, unsigned int id2);


	//helper function to physics for collider updates
    static void UpdatePOLY_WithRotation(const transform2D& trans, Poly& poly);
	static Poly AABBToPoly(AABB& aabb);
	static void updateAABB(const transform2D& trans, AABB& aabb);

    void collidersDebugDraw(bool isEditorMode = false);

    CollisionSystem& GetInstance();
};

//# define CollisionSys ::CollisionSystem::GetInstance()
extern std::shared_ptr<CollisionSystem> collisionSystemsPtr;
#define collisionSys collisionSystemsPtr
#define collisionSysInstance collisionSystemsPtr


/*
Purpose of this system is to get all those registered under
poly collider, it will be added to main collision system check
*/
class CollisionPolygonSystem : public systemBase
{
public:
    CollisionPolygonSystem() = default;
    CollisionPolygonSystem(bool requireComponentMatch);
    ~CollisionPolygonSystem() = default;

    static std::shared_ptr<CollisionPolygonSystem> getInstance();
};

//# define CollisionSys ::CollisionSystem::GetInstance()
extern std::shared_ptr<CollisionPolygonSystem> collisionPolySystemsPtr;

#endif
