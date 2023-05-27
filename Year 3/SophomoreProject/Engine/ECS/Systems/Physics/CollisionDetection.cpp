 /******************************************************************************/
 /*!
 \file         CollisionDetection.cpp
 \project name Engine
 \author(s)    Ho Zi Xing , Poh Chun Ren
 \date         13rd Apr, 2022 <Date of last SIGNIFICANT CHANGE>
 \brief        Collision detection functions
 \lines        141 Lines
 \lines        423 Lines total (Gold)
 
 All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
 reserved.
 Copyright (C) 2021 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the
 prior written consent of DigiPen Institute of Technology is prohibited.
  */
  /******************************************************************************/

#include <limits>
#include "CollisionDetection.h"
#include "Manifold.h"
#include "../../ECS.h"
#include "ImplicitGrid.h"
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

//void abs(float &a)
//{
//	if (a < 0)
//		a *= -1.f;
//}

/******************************************************************************/
/*!
	Collision between two boxes
 */
 /******************************************************************************/
bool CollisionIntersection_RectRect(const AABB& aabb1, const Vec2& vel1,
	const AABB& aabb2, const Vec2& vel2,
	float dt, collidablePairID& m)
{
	//mid of aabb1 = min + scale / 2
	vec2 midAABB1 = aabb1.min + (aabb1.max - aabb1.min) * 0.5f;
	vec2 midAABB2 = aabb2.min + (aabb2.max - aabb2.min) * 0.5f;
	vec2 aabb1aabb2scaleHalfed = ((aabb1.max - aabb1.min) + (aabb2.max - aabb2.min)) * 0.5f; //2aabb combinded and halfed
	//checking for static collision
	if (!(aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x || aabb1.max.y < aabb2.min.y || aabb2.max.y < aabb1.min.y))
	{
		////////////////////////////////////////////GETTING OF COLLIDING INFO////////////////////
		vec2 deltaDist = midAABB2 - midAABB1; //This is the actual distance between 2 aabb
		deltaDist.x = abs(deltaDist.x);
		deltaDist.y = abs(deltaDist.y);

		float resultX = deltaDist.x - aabb1aabb2scaleHalfed.x; //real distance - 2 aabb side by side distance
		float resultY = deltaDist.y - aabb1aabb2scaleHalfed.y;

		//if both real distance is smaller than scaled distance side by side...
		if (resultX < 0.f && resultY < 0.f)
		{
			//should be colliding
			//bigger result value means that it is less penetrated.
			if (resultX < resultY)
			{
				//y is the one with smaller penetration
				//std::cout << "Y axis has smaller penetration, to resolve either top or bottom" << std::endl;
				//m.penetration = resultX;
				m.penetration = resultY;
				if (m.static1 == false)
				{
					if (vel1.y < 0.f)
						m.collisionNormal = { 0.f, 1.f };
					else
						m.collisionNormal = { 0.f, -1.f };
				}
				else
				{
					if (vel2.y < 0.f)
						m.collisionNormal = { 0.f, -1.f };
					else
						m.collisionNormal = { 0.f, 1.f };
				}
			}
			else
			{
				//x is the one with smaller penetration
				//std::cout << "X axis has smaller penetration, to resolve either left or right" << std::endl;
				//m.penetration = resultY;
				m.penetration = resultX;
				if (m.static1 == false)
				{
					if (vel1.x < 0.f)
						m.collisionNormal = { 1.f, 0.f };
					else
						m.collisionNormal = { -1.f, 0.f };
				}
				else
				{
					if (vel2.x < 0.f)
						m.collisionNormal = { -1.f, 0.f };
					else
						m.collisionNormal = { 1.f, 0.f };
				}
			}
		}
		////////////////////////////////////////////GETTING OF COLLING INFO////////////////////
		return true;
	}
	/*Step 2: Initialize and calculate the new velocity of Vb
		tFirst = 0
		tLast = dt*/

	vec2 bRelVel{ vel2.x - vel1.x, vel2.y - vel1.y };
	float tFirst = 0.0f;
	float tLast = dt;

	//From the perspective of '1', where the collision might occur
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	// '2' is moving left
	if (bRelVel.x < 0.0f)
	{
		// '2' is at the left, '2' move left, '1' at right, no collision
		if (aabb1.min.x > aabb2.max.x)
		{//case 1
			return false;
		}
		// '2' is at the right, '2' move left, possible collision
		if (aabb1.max.x < aabb2.min.x)
		{//case 4 //first takes larger
			tFirst = tFirst < (aabb1.max.x - aabb2.min.x) / bRelVel.x ? (aabb1.max.x - aabb2.min.x) / bRelVel.x : tFirst;
			right = true;
			m.collisionNormal = { 1.f, 0.f };
			float requiredDistance = midAABB1.x + aabb1aabb2scaleHalfed.x;
			m.penetration = abs(requiredDistance - midAABB2.x);
		}
		if (aabb1.min.x < aabb2.max.x)
		{//case 4 //first takes larger
			tLast = tLast < (aabb1.min.x - aabb2.max.x) / bRelVel.x ? tLast : (aabb1.min.x - aabb2.max.x) / bRelVel.x;
			//right = true;
		}
	}
	//'2' is moving right
	if (bRelVel.x > 0.0f)
	{
		//'2' is at the right, '2' move right, no collision
		if (aabb1.max.x < aabb2.min.x)
		{//case 3
			return false;
		}
		//'2' is at the left, '2' move right, possible collision
		if (aabb1.min.x > aabb2.max.x)
		{//case 2
			tFirst = tFirst < (aabb1.min.x - aabb2.max.x) / bRelVel.x ? (aabb1.min.x - aabb2.max.x) / bRelVel.x : tFirst;
			left = true;
			m.collisionNormal = { -1.f, 0.f };
			float requiredDistance = midAABB1.x - aabb1aabb2scaleHalfed.x;
			m.penetration = requiredDistance - midAABB2.x;
		}
		if (aabb1.max.x > aabb2.min.x)
		{
			tLast = tLast < (aabb1.max.x - aabb2.min.x) / bRelVel.x ? tLast : (aabb1.max.x - aabb2.min.x) / bRelVel.x;
			//left = true;
		}
	}

	// '2' is moving down
	if (bRelVel.y < 0.0f)
	{
		// '2' is at the left, '2' move left, no collision
		if (aabb1.min.y > aabb2.max.y)
		{//case 1
			return false;
		}
		// '2' is at the right, '2' move left, possible collision
		if (aabb1.max.y < aabb2.min.y)
		{//case 4 //first takes larger
			tFirst = tFirst < (aabb1.max.y - aabb2.min.y) / bRelVel.y ? (aabb1.max.y - aabb2.min.y) / bRelVel.y : tFirst;
			up = true;
			m.collisionNormal = { 0.f, 1.f };
			float requiredDistance = midAABB1.y + aabb1aabb2scaleHalfed.y;
			m.penetration = abs(requiredDistance - midAABB2.y);
		}
		if (aabb1.min.y < aabb2.max.y)
		{//case 4 //first takes larger
			tLast = tLast < (aabb1.min.y - aabb2.max.y) / bRelVel.y ? tLast : (aabb1.min.y - aabb2.max.y) / bRelVel.y;
			//up = true;
		}
	}
	//'2' is moving up
	if (bRelVel.y > 0.0f)
	{
		//'2' is at the up, '2' move up, no collision
		if (aabb1.max.y < aabb2.min.y)
		{//case 3
			return false;
		}
		//'2' is at the left, '2' move up, possible collision
		if (aabb1.min.y > aabb2.max.y)
		{//case 2
			tFirst = tFirst < (aabb1.min.y - aabb2.max.y) / bRelVel.y ? (aabb1.min.y - aabb2.max.y) / bRelVel.y : tFirst;
			down = true;
			m.collisionNormal = { 0.f, -1.f };
			float requiredDistance = midAABB1.y - aabb1aabb2scaleHalfed.y;
			m.penetration = requiredDistance - midAABB2.y;
		}
		if (aabb1.max.y > aabb2.min.y)
		{
			tLast = tLast < (aabb1.max.y - aabb2.min.y) / bRelVel.y ? tLast : (aabb1.max.y - aabb2.min.y) / bRelVel.y;
			//down = true;
		}
	}
	if (bRelVel.x == 0.0f)
	{
		//when '2' relative is 0
		if (aabb1.min.x > aabb2.max.x)
		{
			return false;
		}
		else if (aabb1.max.x < aabb2.min.x)
		{
			return false;
		}
	}
	if (bRelVel.y == 0.0f)
	{
		//when '2' relative is 0
		if (aabb1.min.y > aabb2.max.y)
		{
			return false;
		}
		else if (aabb1.max.y < aabb2.min.y)
		{
			return false;
		}
	}
	if (tFirst > tLast)
	{
		//case 5
		return false;
	}

	//std::cout << std::endl;
	//std::cout << "Up " << up << std::endl;
	//std::cout << "Down " << down << std::endl;
	//std::cout << "left " << left << std::endl;
	//std::cout << "right " << right << std::endl;
	//std::cout << std::endl;
	//std::cout << "penetration: " << m.penetration << std::endl;
	//std::cout << "Collision normal: " << m.collisionNormal.x << ", " <<m.collisionNormal.y << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;

	//std::cout << "Possible tunneling detected on " << m.first << " and " << m.second << std::endl;
	return true;
}

bool lineIntersection(	float Ax, float Ay, float Bx, float By,
						float Cx, float Cy, float Dx, float Dy,
						float* X, float* Y) {

	float  distAB, theCos, theSin, newX, ABpos;

	//  Fail if either line is undefined.
	if (Ax == Bx && Ay == By || Cx == Dx && Cy == Dy) 
		return false;

	//  (1) Translate the system so that point A is on the origin.
	Bx -= Ax; By -= Ay;
	Cx -= Ax; Cy -= Ay;
	Dx -= Ax; Dy -= Ay;

	//  Discover the length of segment A-B.
	distAB = sqrt(Bx * Bx + By * By);

	//  (2) Rotate the system so that point B is on the positive X axis.
	theCos = Bx / distAB;
	theSin = By / distAB;
	newX = Cx * theCos + Cy * theSin;
	Cy = Cy * theCos - Cx * theSin; Cx = newX;
	newX = Dx * theCos + Dy * theSin;
	Dy = Dy * theCos - Dx * theSin; Dx = newX;

	//  Fail if the lines are parallel.
	if (Cy == Dy) 
		return false;

	//  (3) Discover the position of the intersection point along line A-B.
	ABpos = Dx + (Cx - Dx) * Dy / (Dy - Cy);

	//  (4) Apply the discovered position to line A-B in the original coordinate system.
	*X = Ax + ABpos * theCos;
	*Y = Ay + ABpos * theSin;

	//  Success.
	return true;
}

bool lineIntersection(vec2 A, vec2 B, vec2 C, vec2 D, vec2& result)
{
	return lineIntersection(A.x, A.y, B.x, B.y, C.x, C.y, D.x, D.y, &result.x, &result.y);
}

/******************************************************************************/
/*!
	SAT // USE THIS 


 */
 /******************************************************************************/
bool CollisionIntersection_DynamicSAT(Poly& polygon1, const Vec2& vel1, Poly& polygon2, const Vec2& vel2, collidablePairID& m)
{
	UNREFERENCED_PARAMETER(vel1);
	UNREFERENCED_PARAMETER(vel2);
	Poly* p1 = &polygon1;
	Poly* p2 = &polygon2;

	// set the smallest value of float
	float separation = std::numeric_limits<float>::lowest();
	float separation1 = 0;
	float separation2 = 0;
	vec2 normal_of_seperation1{ 0 };
	//short index_of_smallest_seperation1 = 0;
	vec2 normal_of_seperation2{ 0 };
	//short index_of_smallest_seperation2 = 0;

	//For rotation
	/*
	* Test algorithmn
	1) Find center of object, add velocity until it reaches the perimeter to get a point
	2) Project the point onto the normal
	3a)If outside, means rotate
	3b)Else dont rotate 
	*/
	//1)
	/*vec2 point_on_perimeter1 = ecs.getComponent<transform2D>(m.first).pos.getVec2();
	vec2 point_on_perimeter2 = ecs.getComponent<transform2D>(m.second).pos.getVec2();
	for (size_t i = 0; i < p1->points.size(); ++i)
	{
		size_t j = (i + 1) % p1->points.size();
		lineIntersection(point_on_perimeter1, point_on_perimeter1 + vel1,);
	}
	
	float x = 0.f, y = 0.f;
	point_on_perimeter1 + x * vel1 == p1 + y * (p2-p1);*/



	for (size_t loop = 0; loop < 2; ++loop)
	{
		if (loop == 1)
		{
			p1 = &polygon2;
			p2 = &polygon1;
		}

		// loop through each of the edges for polygon1
		for (size_t i = 0; i < p1->points.size(); ++i)
		{
			// using the index, index + 1 and wrap around the 2 edges
			size_t j = (i + 1) % p1->points.size();

			// compute the normal to the vector between points i and j
			vec2 edgeNormal{ -(p1->points[j].y - p1->points[i].y), (p1->points[j].x - p1->points[i].x) };

			// normalise the vector normal to the edge
			edgeNormal = Normalize(edgeNormal);
			//Initialise the result normal 
			if (i == 0)
			{
				if (loop == 0)
				{
					normal_of_seperation1 = edgeNormal;
				}
				else if (loop == 1)
				{
					normal_of_seperation2 = edgeNormal;
				}
			}
			// getting the biggest value of the float
			float minSeparation = std::numeric_limits<float>::max();

			// loop through each of the edges for polygon2
			for (size_t p = 0; p < p2->points.size(); ++p)
			{
				// computing the vector between each polygon
				vec2 vector{ p2->points[p] - p1->points[i] };
				//vec2 vector{ p1->points[i] - p2->points[p] };
				// project the vector onto the normalised vector
				float project_onto_normal{ Dot(vector, edgeNormal) };
				// getting the minimum value
				//minSeparation = min(minSeparation, project_onto_normal);
				if (minSeparation > project_onto_normal)
				{
					minSeparation = project_onto_normal;
				}
			}

			if (minSeparation > separation)
			{
				separation = minSeparation;
				if (loop == 0)
				{
					normal_of_seperation1 = edgeNormal;
				}
				else if (loop == 1)
				{
					normal_of_seperation2 = edgeNormal;
				}
			}
		}
		if (loop == 0)
		{
			separation1 = separation;
		}
		else if (loop == 1)
		{
			separation2 = separation;
		}
	}
	//if (separation > 0)
	if (separation1 > 0 || separation2 > 0)
	{
		//std::cout << "SAT NOT COLLIDED " <<  std::endl;
		//std::cout << "separation1: " << separation1 << std::endl;
		//std::cout << "separation2: " << separation2 << std::endl;
		return false;
	}
	else
	{
		if (separation1 >= separation2)
		{
			m.penetration = separation1;
			m.collisionNormal = -normal_of_seperation1;
		}
		else
		{
			m.penetration = separation2;
			m.collisionNormal = normal_of_seperation2;
		}
		//std::cout << "m.penetration: " << m.penetration << std::endl;
		//std::cout << "m.collisionNormal: " << m.collisionNormal.x << ", " << m.collisionNormal.x << std::endl;
		return true;
	}
}

bool CollisionIntersection_DynamicSAT2(Poly& polygon1, Poly& polygon2, collidablePairID& m)
{
	Poly* p1 = &polygon1;
	Poly* p2 = &polygon2;

	// set the smallest value of float
	//float separation = std::numeric_limits<float>::lowest();

	//For each polygon to check
	for (size_t loop = 0; loop < 2; ++loop)
	{
		if (loop == 1)
		{
			p1 = &polygon2;
			p2 = &polygon1;
		}

		// loop through each of the edges for current polygon
		for (size_t i = 0; i < p1->points.size(); ++i)
		{
			// using the index, index + 1 and wrap around the 2 edges
			size_t j = (i + 1) % p1->points.size();

			// compute the normal to the vector between points i and j
			// aka compute the outward facing normal of an edge
			vec2 edgeNormal{ -(p1->points[j].y - p1->points[i].y), (p1->points[j].x - p1->points[i].x) };
			edgeNormal = Mathf::Normalize(edgeNormal);
			float minR1 = INFINITY, maxR1 = -INFINITY;

			//look for the min and max of polygon1 in terms of projection onto the edgenormal
			for (size_t p = 0; p < p1->points.size(); ++p)
			{
				float project_onto_normal{ Dot(p1->points[p], edgeNormal) };
				minR1 = std::min(minR1, project_onto_normal);
				maxR1 = std::max(maxR1, project_onto_normal);
			}

			float minR2 = INFINITY, maxR2 = -INFINITY;
			//look for the min and max of polygon2 in terms of projection onto the edgenormal
			for (size_t p = 0; p < p2->points.size(); ++p)
			{
				float project_onto_normal{ Dot(p2->points[p], edgeNormal) };
				minR2 = std::min(minR2, project_onto_normal);
				maxR2 = std::max(maxR2, project_onto_normal);
			}

			//Previously written this way, leaving it here in case got issue
			//if (!(maxR2 >= minR1 && maxR1 >= minR2))
			//	return false;

			//when min of 1polygon is greater than the max of the other polygon,
			//there is no collision
			if ((maxR2 < minR1 || maxR1 < minR2))
				return false;
			if (i == p1->points.size() - 1)
			{
				/*std::cout << "maxR2 >= minR1" << std::endl;
				std::cout << maxR2 << std::endl;
				std::cout << minR1 << std::endl;
				std::cout << "maxR1 >= minR2" << std::endl;
				std::cout << maxR1 << std::endl;
				std::cout << minR2 << std::endl;*/
				if ((maxR2 - minR1) > (maxR1 - minR2))
				{
					m.penetration = (maxR1 - minR2);
				}
				else
				{
					m.penetration = (maxR2 - minR1);
				}
			}
		}
	}
	


	//Currently testing out for correct value
	m.collisionNormal;
	//std::cout << "Collision Normal: " << m.collisionNormal.x << ", "<< m.collisionNormal.y << std::endl;
	m.penetration;
	//std::cout << "Collision Penetration: " << m.penetration << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;

	//colliding since no "seperating" axis was found
	polygon1.overlap = true;
	polygon2.overlap = true;
	return true;
}

bool CollisionCircleCircle(const Circle& cir1, const Vec2& vel1, const Circle& cir2, const Vec2& vel2, float dt, collidablePairID& m) 
{
	UNREFERENCED_PARAMETER(m);
	UNREFERENCED_PARAMETER(vel1);
	UNREFERENCED_PARAMETER(vel2);
	UNREFERENCED_PARAMETER(dt);
	//static check
	vec2 dist = cir2.m_center - cir1.m_center;
	float sumOfRadius = cir2.m_radius + cir1.m_radius;
	//comparing in squared distance instead of rooting it to save computation
	if ((dist.x * dist.x + dist.y * dist.y) > sumOfRadius * sumOfRadius)
		return false;
	else
	{
		//TODO: Calculate penetration details
		//m;
		return true;
	}
}

bool CollisionRectCircle(const Box2DCollider& rect, const Vec2& velPoly, const Circle& cir, const Vec2& velCir, float dt, collidablePairID& m)
{
	UNREFERENCED_PARAMETER(cir);
	UNREFERENCED_PARAMETER(m);
	UNREFERENCED_PARAMETER(dt);
	UNREFERENCED_PARAMETER(rect);
	UNREFERENCED_PARAMETER(velCir);
	UNREFERENCED_PARAMETER(velPoly);
	//static check
	//vec2 circleDistance;
	//circleDistance.x = abs(cir.m_center.x - rect.offset_pos.x);
	//circleDistance.y = abs(cir.m_center.y - rect.offset_pos.y);

	//if (circleDistance.x > (rect.width / 2 + cir.m_radius)) { return false; }
	//if (circleDistance.y > (rect.height / 2 + cir.m_radius)) { return false; }

	//if (circleDistance.x <= (rect.width / 2)) { return true; }
	//if (circleDistance.y <= (rect.height / 2)) { return true; }

	//cornerDistance_sq = (circleDistance.x - rect.width / 2) ^ 2 + (circleDistance.y - rect.height / 2) ^ 2;

	//return (cornerDistance_sq <= (circle.r ^ 2));


	return false;
}

bool CollisionRectRect(const Box2DCollider& rect1, const Vec2& vel1, const Box2DCollider& rect2, const Vec2& vel2, float dt, collidablePairID& m)
{
	UNREFERENCED_PARAMETER(m);
	UNREFERENCED_PARAMETER(rect1);
	UNREFERENCED_PARAMETER(rect2);
	UNREFERENCED_PARAMETER(vel2);
	UNREFERENCED_PARAMETER(vel1);

	//static check
	UNREFERENCED_PARAMETER(dt);
	return false;
}

bool CollisionPointRect(vec2& pt, Poly& box)
{	
	//Only rectangles
	if (box.points.size() == 4)
	{
		vec2 v1 = box.points[1] - box.points[0]; // one side of vector from [0]
		vec2 v2 = box.points[3] - box.points[0]; // one side of vector from [0] "another"
		vec2 v3 = box.points[1] - box.points[2]; // one side of vector from [2]
		vec2 v4 = box.points[3] - box.points[2]; // one side of vector from [2] "another"

		if (Mathf::Dot(pt, v1) > 0 && Mathf::Dot(pt, v2) > 0 &&
			Mathf::Dot(pt, v3) > 0 && Mathf::Dot(pt, v4) > 0)
			return true;
	}
	return false;
}

/*Another way is to imagine a mid point of polygon 1 extend out to the other point. And use that line to intersect with all polygon2 edge,
repeat for both side. And it should have same collision result, and with this, can use parametrisation to find out the penetration value*/


/******************************************************************************/
/*!
	Collision between a point and the ray
 */
 /******************************************************************************/
//
//int CollisionIntersection_RayCircle(const Ray & ray,
//									const Circle & circle,
//									float& interTime)
//{
//	Vec2 BsC = circle.m_center - ray.m_pt0;
//
//	Vec2 normalisedVel = ray.m_dir;
//	Mathf::Normalize(normalisedVel);
//
//	float m = Mathf::Dot(BsC, normalisedVel);
//	float nSquare = Mathf::LengthSqr(BsC) - m * m;
//	float sSquare = (circle.m_radius * circle.m_radius) - nSquare;
//
//	if (m < 0 &&Mathf::LengthSqr(BsC) >(circle.m_radius * circle.m_radius))
//	{
//		return 0;
//	}
//	else if (nSquare > (circle.m_radius * circle.m_radius))
//	{
//		return 0;
//	}
//	else
//	{
//		float ti0 = (m - sqrt(sSquare)) / Mathf::Length(ray.m_dir);
//		float ti1 = (m + sqrt(sSquare)) / Mathf::Length(ray.m_dir);
//		interTime = min(ti0, ti1);
//
//		if (interTime > 0 && interTime < 1)
//		{
//			return 1;
//		}
//		else
//		{
//			return 0;
//		}
//	}
//}

/******************************************************************************/
/*!
	Collision between two circles
 */
 /******************************************************************************/
//int CollisionIntersection_CircleCircle(const Circle& circleA,
//									   const Vec2& velA,
//									   const Circle& circleB,
//									   const Vec2& velB,
//									   Vec2& interPtA,
//									   Vec2& interPtB,
//									   float& interTime)
//{
//	//compute relative velocity
//	Vec2 relVel{ velA - velB };
//
//	Circle circleC;
//	circleC.m_center = circleB.m_center;
//	circleC.m_radius = circleA.m_radius + circleB.m_radius;
//	circleC.m_mass = circleB.m_mass;
//
//	Ray ray;
//	ray.m_pt0 = circleA.m_center;
//	ray.m_dir = relVel;
//
//	if (CollisionIntersection_RayCircle(ray, circleC, interTime))
//	{
//		interPtA = circleA.m_center + (velA * interTime);
//
//		interPtB = circleB.m_center + (velB * interTime);
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//}

float min(float l, float r)
{
	if (l < r)
		return l;
	else
		return r;
};

template <typename T>
T max(T l, T r)
{
	if (l > r)
		return l;
	else
		return r;
}

bool CollisionIntersectionLineLine(vec2 point1, vec2 point2, vec2 point3, vec2 point4)
{
	//ad - bc
	float determinant = (point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x);

	//lines are parallel
	if (determinant == 0)
	{
		return false; //no collision
	}

	//Ax + By = C : standard form of linear eqn
	float C1 = (point1.x * point2.y) - (point1.y * point2.x);
	float C2 = (point3.x * point4.y) - (point3.y * point4.x);

	//Point of intersection
	float x = (((point3.x - point4.x) * C1) - ((point1.x - point2.x) * C2)) / determinant;
	float y = (((point3.y - point4.y) * C1) - ((point1.y - point2.y) * C2)) / determinant;

	//Check if the point x is not on the line segment

	if (x < min(point1.x, point2.x) || x > max(point1.x, point2.x) ||
		x < min(point3.x, point4.x) || x > max(point3.x, point4.x))
		return false;

	//Check if the point y is not on the line segment
	if (y < min(point1.y, point2.y) || y > max(point1.y, point2.y) ||
		y < min(point3.y, point4.y) || y > max(point3.y, point4.y))
		return false;

	return true;
}

