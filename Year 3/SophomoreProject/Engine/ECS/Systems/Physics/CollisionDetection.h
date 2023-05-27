 /******************************************************************************/
 /*!
 \file         CollisionDetection.h
 \project name Engine
 \author(s)    Ho Zi Xing
 \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
 \brief        Collision detection function declarations
 \lines        20 Lines

 All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
 reserved.
 Copyright (C) 2021 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the
 prior written consent of DigiPen Institute of Technology is prohibited.
  */
  /******************************************************************************/

#pragma once
#include <filesystem>
#include <stdlib.h>
#include "../../component.h"
#include "../../../Math/Vector/Vector.h"

struct collidablePairID;
bool CollisionIntersection_RectRect(const AABB& aabb1, const Vec2& vel1,
									const AABB& aabb2, const Vec2& vel2,
									float dt, collidablePairID& m);

//int CollisionIntersection_CircleCircle(const Circle& circleA,
//									   const Vec2& velA,
//									   const Circle& circleB,
//									   const Vec2& velB,
//									   Vec2& interPtA,
//									   Vec2& interPtB,
//									   float& interTime);
//
//int CollisionIntersection_RayCircle(const Ray& ray,
//									const Circle& circle,
//									float& interTime);

bool CollisionIntersection_DynamicSAT(Poly& polygon1, const Vec2& vel1, Poly& polygon2, const Vec2& vel2, collidablePairID& m);
bool CollisionIntersection_DynamicSAT2(Poly& polygon1, Poly& polygon2, collidablePairID& m);


//Main collision check
[[nodiscard("CollisionCheck")]] bool CollisionCircleCircle(const Circle& cir1, const Vec2& vel1, const Circle& cir2, const Vec2& vel2, float dt, collidablePairID& m);

//not fully coded yet
[[nodiscard("CollisionCheck")]] bool CollisionRectCircle(const Box2DCollider& rect, const Vec2& velPoly, const Circle& cir, const Vec2& velCir, float dt, collidablePairID& m);

[[nodiscard("CollisionCheck")]] bool CollisionRectRect(const Box2DCollider& rect1, const Vec2& vel1, const Box2DCollider& rect2, const Vec2& vel2, float dt, collidablePairID& m);



bool CollisionPointRect(vec2& pt, Poly& poly);
bool CollisionIntersectionLineLine(vec2 point1, vec2 point2, vec2 point3, vec2 point4);

//struct Manifold;
//bool AABBvsAABB(Manifold* m);
