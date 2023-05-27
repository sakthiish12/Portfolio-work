// /******************************************************************************/
// /*!
// \file         ForceManager.hcpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         12th Nov, 2021
// \brief        Physics system for ecs that handles movement and collision
// \lines        33 Lines
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _RAY_H_
#define _RAY_H_

#include <iostream>
#include <vector>
#include <map>
#include "../../../Math/Vector/Vector2.h"
#include "LineSeg.h"

/*
* Raycasting Youtube notes:
*
* LineSeg: a line seg
* RayCast: a pos, a dir
*
* Goal: if ray hits line seg, return the point
*/

struct RayCast
{
    Mathf::vec2 start{ 0.f, 0.f };
    Mathf::vec2 dir{ 1.f, 0.f};
    Mathf::vec2 IntersectedPoint{ 0 }; //First point hit by this ray
    bool rayHit = false;

    RayCast();
    RayCast(Mathf::vec2 _start, Mathf::vec2 _dir);
    RayCast(Mathf::vec2 _start, float _radian); //with 0 radian pointing to right

    //Set the line seg
    virtual void lookAt(Mathf::vec2 target);

    //check for intersection
    bool cast(LineSeg line);


};

bool testRay();

//int main()
//{
//    testRay();
//    return 2;
//}

struct RayArc : public RayCast
{
    float rot = 0.f;
    float arc = 90.f; //for now in degree, the total sum of
    float angle_per_ray;
    float lenghtLimit;

    std::vector<RayCast> shotRay;
    //std::map<unsigned int> collidedObject;

    void lookAt(Mathf::vec2 target) override; // to affect the rotation also
    void createRays(); //create ray into shotRay
    void update(); //move rays
    void collided();// check for collision, to return entityID

};

#endif

