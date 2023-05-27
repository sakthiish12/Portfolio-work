// /******************************************************************************/
// /*!
// \file         easing.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines       66 
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
/*
 C++ port of Penner easing
 equations specifically with
 SFML in mind

 Adapted from c++ port by Jesus Gollonet
 http://www.jesusgollonet.com/blog/2007/09/24/penner-easing-cpp

 Reference:
 https://code.markrichards.ninja/sfml/how-to-implement-easing-in-your-games
*/
#pragma once
#include "../Math/Vector/Vector3.h"

namespace Easing
{
	enum class Type
	{
		Linear,
		ExpoEaseIn,
		ExpoEaseOut,
		ExpoEaseInOut,
		CubicEaseIn,
		CubicEaseOut,
		CubicEaseInOut,
		QuarticEaseIn,
		QuarticEaseOut,
		QuarticEaseInOut,
		QuinticEaseIn,
		QuinticEaseOut,
		QuinticEaseInOut,
		QuadraticEaseIn,
		QuadraticEaseOut,
		QuadraticEaseInOut,
		SineEaseIn,
		SineEaseOut,
		SineEaseInOut,
		CircularEaseIn,
		CircularEaseOut,
		CircularEaseInOut,
		BackEaseIn,
		BackEaseOut,
		BackEaseInOut,
		ElasticEaseIn,
		ElasticEaseOut,
		ElasticEaseInOut,
	};


	/*
	\brief
		Ease in with Type
	\params
		currTime: current elapsed duration
		startVal: starting value being interpolated
		direction: direction value to move towards
		duration: total time for this animation

	 See Demo for visualization: https://easings.net/
	*/
	float Ease(Type easeType, float currTime, float startVal, float direction, float duration);
	Mathf::Vector3 Ease(Type easeType, float currTime, Mathf::Vector3 startPos, Mathf::Vector3 direction, float duration);

	// [Helpers]
	float linear(float t, float b, float c, float d);
	float expoEaseIn(float t, float b, float c, float d);
	float expoEaseOut(float t, float b, float c, float d);
	float expoEaseInOut(float t, float b, float c, float d);
	float cubicEaseIn(float t, float b, float c, float d);
	float cubicEaseOut(float t, float b, float c, float d);
	float cubicEaseInOut(float t, float b, float c, float d);
	float quarticEaseIn(float t, float b, float c, float d);
	float quarticEaseOut(float t, float b, float c, float d);
	float quarticEaseInOut(float t, float b, float c, float d);
	float quinticEaseIn(float t, float b, float c, float d);
	float quinticEaseOut(float t, float b, float c, float d);
	float quinticEaseInOut(float t, float b, float c, float d);
	float quadraticEaseIn(float t, float b, float c, float d);
	float quadraticEaseOut(float t, float b, float c, float d);
	float quadraticEaseInOut(float t, float b, float c, float d);
	float sineEaseIn(float t, float b, float c, float d);
	float sineEaseOut(float t, float b, float c, float d);
	float sineEaseInOut(float t, float b, float c, float d);
	float circularEaseIn(float t, float b, float c, float d);
	float circularEaseOut(float t, float b, float c, float d);
	float circularEaseInOut(float t, float b, float c, float d);
	float backEaseIn(float t, float b, float c, float d);
	float backEaseOut(float t, float b, float c, float d);
	float backEaseInOut(float t, float b, float c, float d);
	float elasticEaseIn(float t, float b, float c, float d);
	float elasticEaseOut(float t, float b, float c, float d);
	float elasticEaseInOut(float t, float b, float c, float d);
};