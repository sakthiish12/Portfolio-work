 /******************************************************************************/
 /*!
 \file         CollisionResponse.cpp
 \project name Engine
 \author(s)    Ho Zi Xing
 \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
 \brief        Collision response functions
 \lines        34 Lines

 All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
 reserved.
 Copyright (C) 2021 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the
 prior written consent of DigiPen Institute of Technology is prohibited.
  */
  /******************************************************************************/

#include "CollisionResponse.h"

/******************************************************************************/
/*!
	The behaviour in the program after the circle collides
	One of the circle is static and the other is animated
 */
/******************************************************************************/
void CollisionResponse_CirclePillar(const Vec2& normal,
									const float& interTime,
									const Vec2& ptStart,
									const Vec2& ptInter,
									Vec2& ptEnd,
									Vec2& reflectedVectorNormalized)
{
	static_cast<void>(interTime);//unreferenced variable 
	Vec2 BiBs{ ptStart - ptInter };
	//CSD1130::Vec2 reflection = 2 * (CSD1130::Vector2DDotProduct(BiBs, normal) * normal) - BiBs;
	ptEnd = ptInter + ((Mathf::Dot(BiBs, normal) * 2 * normal) - BiBs);
	// ptEnd = ptInter + reflection;
	reflectedVectorNormalized = ptEnd - ptInter;
	Mathf::Normalize(reflectedVectorNormalized);
}

/******************************************************************************/
/*!
	The behaviour in the program after the circle collides
	Both of the circles are animated
*/
/******************************************************************************/
void CollisionResponse_CircleCircle(Vec2& normal,
									const float interTime,
									Vec2& velA,
									const float& massA,
									Vec2& interPtA,
									Vec2& velB,
									const float& massB,
									Vec2& interPtB,
									Vec2& reflectedVectorA,
									Vec2& ptEndA,
									Vec2& reflectedVectorB,
									Vec2& ptEndB)
{
	//Compute the length of Va along N
	float aA = Mathf::Dot(velA, normal);

	//Compute the length of Vb along N
	float aB = Mathf::Dot(velB, normal);

	//Compute the reflected vector Va'
	reflectedVectorA = velA - (2.0f * (aA - aB) / (massA + massB)) * massB * normal;

	//Compute the reflected vector Vb'
	reflectedVectorB = velB + (2.0f * (aA - aB) / (massA + massB)) * massA * normal;

	//Compute position Cae after reflection
	ptEndA = interPtA + (1.0f - interTime) * reflectedVectorA;

	//Compute position Cbe after reflection
	ptEndB = interPtB + (1.0f - interTime) * reflectedVectorB;
}
