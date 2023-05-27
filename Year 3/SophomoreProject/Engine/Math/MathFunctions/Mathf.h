// /******************************************************************************/
// /*!
// \file         Mathf.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        35 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once
#include "../../Dependencies/Vendor/glm/glm.hpp"//glm
#include "../Vector/Vector2.h"

namespace Mathf
{
	constexpr float EPSILON = 0.0001f;
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_TWO = PI * 2;
	constexpr float PI_HALF = PI * 0.5;
	constexpr float PI_QUATER = PI * 0.25;

	// Clamps the given value between the given minimum and maximum value
	// Returns the given value if it is within the min and max range
	template <typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	// Returns the smallest of two or more values
	template <typename T>
	T Min(T value, T min)
	{
		return (value < min) ? min : value;
	}

	// Returns the largest of two or more values
	template <typename T>
	T Max(T value, T max)
	{
		return (value > max) ? max : value;
	}

	// Converts a given Degree to Radian
	float DegreeToRadian(float degree);

	// Converts a given Radian to Degree
	float RadianToDegree(float radian);

	// Compares two float values for equality using epsilon
	// Returns TRUE if lhs == rhs
	bool IsEqual(float lhs, float rhs);

	// Returns the linear interpolation between start and end for the parameter t
	// (or extrapolation, when t is outside the range [0, 1]
	float Lerp(float start, float end, float t);
	float ConstLerp(float start, float t);

	// Loops the value t, so that it is never larger than length and never smaller than 0.
	float Repeat(float t, float length);

	// Ping pongs the value t, so that it is never larger than length and never smaller than 0.
	float PingPong(float t, float length);

	// Divides the two integers and returns the result closest to the whole number
	// E.g 5/2=3 , 10/3 = 3
	unsigned DivideRounded(unsigned dividend, unsigned divisor);

	int Floor(float value);

	int Floor(double value);

	//decompose a transformation matrix to it's respective translation, rotation, scale matrix 
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

	
	float getVectorRotationRadian(Vec2 a);
};