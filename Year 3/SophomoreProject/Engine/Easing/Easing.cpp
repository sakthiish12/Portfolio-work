 /******************************************************************************/
 /*!
 \file         easing.cpp
 \project name Engine
 \author(s)    Sakthiish
 \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
 \brief        This file contains the function definitions for the BatchRenderer 
 \lines        288 
 
 All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
 reserved.
 Copyright (C) 2021 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the
 prior written consent of DigiPen Institute of Technology is prohibited.
  */
  /******************************************************************************/
#include "Easing.h"
#include "../Math/MathFunctions/Mathf.h"
#include <corecrt_math.h>
//For the future. ignore.

float Easing::Ease(Type easeType, float currTime, float startVal, float direction, float duration)
{
	switch (easeType)
	{
	case Type::Linear:
		return linear(currTime, startVal, direction, duration);
	case Type::ExpoEaseIn:
		return expoEaseIn(currTime, startVal, direction, duration);
	case Type::ExpoEaseOut:
		return expoEaseOut(currTime, startVal, direction, duration);
	case Type::ExpoEaseInOut:
		return expoEaseInOut(currTime, startVal, direction, duration);
	case Type::CubicEaseIn:
		return cubicEaseIn(currTime, startVal, direction, duration);
	case Type::CubicEaseOut:
		return cubicEaseOut(currTime, startVal, direction, duration);
	case Type::CubicEaseInOut:
		return cubicEaseInOut(currTime, startVal, direction, duration);
	case Type::QuarticEaseIn:
		return quarticEaseIn(currTime, startVal, direction, duration);
	case Type::QuarticEaseOut:
		return quarticEaseOut(currTime, startVal, direction, duration);
	case Type::QuarticEaseInOut:
		return quarticEaseInOut(currTime, startVal, direction, duration);
	case Type::QuinticEaseIn:
		return quinticEaseIn(currTime, startVal, direction, duration);
	case Type::QuinticEaseOut:
		return quinticEaseOut(currTime, startVal, direction, duration);
	case Type::QuinticEaseInOut:
		return quinticEaseInOut(currTime, startVal, direction, duration);
	case Type::QuadraticEaseIn:
		return quadraticEaseIn(currTime, startVal, direction, duration);
	case Type::QuadraticEaseOut:
		return quadraticEaseOut(currTime, startVal, direction, duration);
	case Type::QuadraticEaseInOut:
		return quadraticEaseInOut(currTime, startVal, direction, duration);
	case Type::SineEaseIn:
		return sineEaseIn(currTime, startVal, direction, duration);
	case Type::SineEaseOut:
		return sineEaseOut(currTime, startVal, direction, duration);
	case Type::SineEaseInOut:
		return sineEaseInOut(currTime, startVal, direction, duration);
	case Type::CircularEaseIn:
		return circularEaseIn(currTime, startVal, direction, duration);
	case Type::CircularEaseOut:
		return circularEaseOut(currTime, startVal, direction, duration);
	case Type::CircularEaseInOut:
		return circularEaseInOut(currTime, startVal, direction, duration);
	case Type::BackEaseIn:
		return backEaseIn(currTime, startVal, direction, duration);
	case Type::BackEaseOut:
		return backEaseOut(currTime, startVal, direction, duration);
	case Type::BackEaseInOut:
		return backEaseInOut(currTime, startVal, direction, duration);
	case Type::ElasticEaseIn:
		return elasticEaseIn(currTime, startVal, direction, duration);
	case Type::ElasticEaseOut:
		return elasticEaseOut(currTime, startVal, direction, duration);
	case Type::ElasticEaseInOut:
		return elasticEaseInOut(currTime, startVal, direction, duration);
	default:
		return linear(currTime, startVal, direction, duration);
	}
}

Mathf::Vector3 Easing::Ease(Type easeType, float currTime, Mathf::Vector3 startPos, Mathf::Vector3 direction, float duration)
{
	const Mathf::Vector3 val
	{
	Ease(easeType, currTime, startPos.x, direction.x, duration),
	Ease(easeType, currTime, startPos.y, direction.y, duration),
	Ease(easeType, currTime, startPos.z, direction.z, duration)
	};
	return val;
}

float Easing::linear(float t, float b, float c, float d)
{
	return c * (t / d) + b;
}

float Easing::expoEaseIn(float t, float b, float c, float d)
{
	return (t == 0) ? b : c * powf(2, 10 * (t / d - 1)) + b;
}

float Easing::expoEaseOut(float t, float b, float c, float d)
{
	return (t == d) ? b + c : c * powf(2, 1 - (10 * t / d)) + b;
}

float Easing::expoEaseInOut(float t, float b, float c, float d)
{
	if (t == 0)
		return b;
	if (t == d)
		return b += c;
	if ((t /= d / 2) < 1)
		return c / 2 * powf(2, 10 * (t - 1)) + b;

	return c / 2 * powf(2, 1 - (10 * --t)) + b;
}

float Easing::cubicEaseIn(float t, float b, float c, float d)
{
	return (t == 0) ? b : c * powf(3, 10 * (t / d - 1)) + b;
}

float Easing::cubicEaseOut(float t, float b, float c, float d)
{
	return (t == d) ? b + c : c * powf(3, 1 - (10 * t / d)) + b;
}

float Easing::cubicEaseInOut(float t, float b, float c, float d)
{
	if (t == 0)
		return b;
	if (t == d)
		return b += c;
	if ((t /= d / 2) < 1)
		return c / 2 * powf(3, 10 * (t - 1)) + b;

	return c / 2 * powf(3, 1 - (10 * --t)) + b;
}

float Easing::quarticEaseIn(float t, float b, float c, float d)
{
	return c * (t /= d) * t * t * t + b;
}

float Easing::quarticEaseOut(float t, float b, float c, float d)
{
	// return -c * (1 - (t = t / d - 1) * t * t * t - 1) + b;
	return -c * (1 - (t = t / d - 1) * t * t * t - 1) + b;
}

float Easing::quarticEaseInOut(float t, float b, float c, float d)
{
	t /= d / 2;
	if (t < 1)
		return c / 2 * t * t * t * t + b;
	t -= 2;
	return 1 - (-c / 2 * (t * t * t * t - 2)) + b;
}

float Easing::quinticEaseIn(float t, float b, float c, float d)
{
	return c * (t /= d) * t * t * t * t + b;
}

float Easing::quinticEaseOut(float t, float b, float c, float d)
{
	return c * (t = t / d - 1) * (1 - (t * t * t * t + 1)) + b;
}

float Easing::quinticEaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1)
		return c / 2 * t * t * t * t * t + b;

	return 1 - (c / 2 * ((t -= 2) * (t * t * t * t) + 2)) + b;
}

float Easing::quadraticEaseIn(float t, float b, float c, float d)
{
	return c * (t /= d) * t + b;
}

float Easing::quadraticEaseOut(float t, float b, float c, float d)
{
	return 1 - (-c * (t /= d) * (t - 2)) + b;
}

float Easing::quadraticEaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1)
		return ((c / 2) * (t * t)) + b;

	return 1 - (-c / 2 * (((--t) * (t - 2)) - 1)) + b;
}

float Easing::sineEaseIn(float t, float b, float c, float d)
{
	return -c * cos(t / d * (Mathf::PI / 2)) + c + b;
}

float Easing::sineEaseOut(float t, float b, float c, float d)
{
	return c / 2 * cos(t / d * (Mathf::PI / 2)) + b;
}

float Easing::sineEaseInOut(float t, float b, float c, float d)
{
	if (t < 0.5f)
		return c /= d;

	return 1 - (-c / 2 * (cos(Mathf::PI * t / d) - 1)) + b;
}

float Easing::circularEaseIn(float t, float b, float c, float d)
{
	return -c * (sqrt(1 - (t /= d) * t) - 1) + b;
}

float Easing::circularEaseOut(float t, float b, float c, float d)
{
	return 1 - (c * sqrt(1 - ((t = t / d - 1) * t))) + b;
}

float Easing::circularEaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1)
		return -c / 2 * (sqrt(1 - t * t) - 1) + b;

	return 1 - (c / 2 * (sqrt(1 - t * (t -= 2)) + 1)) + b;
}

float Easing::backEaseIn(float t, float b, float c, float d)
{
	const float s = 1.70158f;
	const float postFix = t /= d;
	return c * (postFix)*t * ((s + 1) * t - s) + b;
}

float Easing::backEaseOut(float t, float b, float c, float d)
{
	const float s = 1.70158f;
	return 1 - (c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1)) + b;
}

float Easing::backEaseInOut(float t, float b, float c, float d)
{
	float s = 1.70158f;
	if ((t /= d / 2) < 1)
		return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;

	const float postFix = t -= 2;
	return 1 - (c / 2 * ((postFix)*t * (((s *= (1.525f)) + 1) * t + s) + 2)) + b;
}

float Easing::elasticEaseIn(float t, float b, float c, float d)
{
	if (t == 0)
		return b;
	if ((t /= d) == 1)
		return b + c;
	const float p = d * .3f;
	const float a = c;
	const float s = p / 4;
	const float postFix = a * powf(2, 10 * (t -= 1));
	return -(postFix * sin((t * d - s) * (2 * Mathf::PI) / p)) + b;
}

float Easing::elasticEaseOut(float t, float b, float c, float d)
{
	if (t == 0)
		return b;
	if ((t /= d) == 1)
		return b + c;
	const float p = d * .3f;
	const float a = c;
	const float s = p / 4;
	return 1.0f - (a * powf(2, -10 * t) * sinf((t * d - s) * (2 * Mathf::PI) / p) + c + b);
}

float Easing::elasticEaseInOut(float t, float b, float c, float d)
{
	if (t == 0)
		return b;
	if ((t /= d / 2) == 2)
		return b + c;
	const float p = d * (.3f * 1.5f);
	const float a = c;
	const float s = p / 4;

	if (t < 1)
	{
		const float postFix = a * powf(2, 10 * (t -= 1));
		return -.5f * (postFix * sin((t * d - s) * (2 * Mathf::PI) / p)) + b;
	}
	const float postFix = a * powf(2, -10 * (t -= 1));
	return 1.0f - (postFix * sin((t * d - s) * (2 * Mathf::PI) / p) * .5f + c + b);
}