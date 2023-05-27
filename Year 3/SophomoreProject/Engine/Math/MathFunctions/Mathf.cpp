// /******************************************************************************/
// /*!
// \file         Mathf.cpp
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        42 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//#include "../pch.h"
#include"../../pch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include "../../Dependencies/Vendor/glm/vec3.hpp"

float Mathf::DegreeToRadian(float degree)
{
	return degree * (PI / 180.0f);
}

float Mathf::RadianToDegree(float radian)
{
	return (radian / PI) * 180.0f;
}

bool Mathf::IsEqual(float lhs, float rhs)
{
	return (lhs - rhs) <= EPSILON && (rhs - lhs) <= EPSILON;
}

float Mathf::Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}

float Mathf::ConstLerp(float start, float t)
{
	return start + t;
}

float Mathf::Repeat(float t, float length)
{
	return Clamp<float>(t - floorf(t / length) * length, 0, length);
}

float Mathf::PingPong(float t, float length)
{
	t = Repeat(t, length * 2.0f);
	return length - abs(t - length);
}

unsigned Mathf::DivideRounded(unsigned dividend, unsigned divisor)
{
	return (dividend + (divisor / 2)) / divisor;
}

int Mathf::Floor(float value)
{
	return static_cast<int>(floor(value));
}

int Mathf::Floor(double value)
{
	return static_cast<int>(floor(value));
}


bool Mathf::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
	// From glm::decompose in matrix_decompose.inl

	using T = float;

	glm::mat4 LocalMatrix(transform);

	// Normalize the matrix.
	if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
		return false;

	// First, isolate perspective.  This is the messiest.
	if (
		glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
		glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
		glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>()))
	{															 
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
		LocalMatrix[3][3] = static_cast<T>(1);
	}

	// Next take care of translation (easy).
	translation = glm::vec3(LocalMatrix[3]);
	LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

	glm::vec3 Row[3];

	// Now get scale and shear.
	for (glm::length_t i = 0; i < 3; ++i)
		for (glm::length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	scale.x = glm::length(Row[0]);
	Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
	scale.y = glm::length(Row[1]);
	Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
	scale.z = glm::length(Row[2]);
	Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
#if 0
	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (dot(Row[0], Pdum3) < 0)
	{
		for (length_t i = 0; i < 3; i++)
		{
			scale[i] *= static_cast<T>(-1);
			Row[i] *= static_cast<T>(-1);
		}
	}
#endif

	rotation.y = asin(-Row[0][2]);
	if (cos(rotation.y) != 0) {
		rotation.x = atan2(Row[1][2], Row[2][2]);
		rotation.z = atan2(Row[0][1], Row[0][0]);
	}
	else {
		rotation.x = atan2(-Row[2][0], Row[1][1]);
		rotation.z = 0;
	}


	return true;
}


float Mathf::getVectorRotationRadian(Vec2 a)
{
	//Top is 0, going clockwise
	float angle;
	//getting angle
	if (a.x >= 0)
	{
		if (a.y >= 0)
		{
			//top right
			angle = atan(a.x / a.y);
		}
		else
		{
			//bottom right
			angle = static_cast<float>(3.14) - atan(a.x / -a.y);
		}
	}
	else
	{
		if (a.y >= 0)
		{
			//top left
			angle = static_cast<float>(2 * 3.14) - atan(-a.x / a.y);
		}
		else
		{
			//bottom left
			angle = static_cast<float>(1 * 3.14) + atan(-a.x / -a.y);
		}
	}
	//std::cout << "rad: " << angle << std::endl;
	return angle;
}
