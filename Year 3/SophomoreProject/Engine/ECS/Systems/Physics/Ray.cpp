// /******************************************************************************/
// /*!
// \file         ForceManager.hcpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         12th Nov, 2021
// \brief        Physics system for ecs that handles movement and collision
// \lines        86 Lines (seg)
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Ray.h"


RayCast::RayCast()
{
}

RayCast::RayCast(Mathf::vec2 _start, Mathf::vec2 _dir) :
    start{ _start },
    dir{ _dir }
{
}

RayCast::RayCast(Mathf::vec2 _start, float _radian) :
    start{ _start }
{
    dir.x = cos(_radian);
    dir.y = sin(_radian);
}

void RayCast::lookAt(Mathf::vec2 target)
{
    dir = { target.x - start.x, target.y - start.y };
    //TODO: normalised
}


bool RayCast::cast(LineSeg line)
{
    float t;
    float u;
    //return true if t between 0 to 1 AND if u greater than 0

    //saving line segment info
    float x1 = line.start.x;
    float y1 = line.start.y;
    float x2 = line.end.x;
    float y2 = line.end.y;

    //saving ray info
    float x3 = start.x;
    float y3 = start.y;
    float x4 = start.x + dir.x;
    float y4 = start.y + dir.y;

    //According to Wiki, modified
    float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denominator == 0.f)
    {
        rayHit = false;
        return false; // parallel line
    }

    float t_numerator = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
    t = t_numerator / denominator;

    float u_numerator = (x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3);
    u_numerator = -u_numerator;
    u = u_numerator / denominator;

    if (t > 0 && t < 1 && u > 0)
    {
        rayHit = true; 
        IntersectedPoint = { x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
        return true;
    }
    else
    {
        rayHit = false;
        return false;
    }
}

bool testRay()
{
    LineSeg boundary;
    boundary.start = { 3.f,1.f };
    boundary.end = { 3.f,3.f };
    RayCast ray{};
    ray.start = { 1.f,2.f };
    ray.dir = { 1.f, 0.f };

    bool result = ray.cast(boundary);
    std::cout << result << std::endl;
    std::cout << ray.IntersectedPoint.x << "." << ray.IntersectedPoint.y << std::endl;
    return result;
}

void RayArc::lookAt(Mathf::vec2 target)
{
    RayCast::lookAt(target);
    //rot = ;
}

void RayArc::createRays()
{

}

void RayArc::update()
{

}

void RayArc::collided()
{

}
