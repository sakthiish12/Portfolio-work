#pragma once
#ifndef	MathLib_H
#define MathLib_H

#include <Precompiled/pch.h>

namespace BE
{
	namespace Math
	{
		//Constants
		constexpr double EPSILON = 0.0001;
		constexpr double PI = 3.14159265358979323846264338327950288;
		constexpr double PI_TIMES_2 = 3.14159265358979323846264338327950288 * 2.0;
		constexpr double PI_OVER_2 = 3.14159265358979323846264338327950288 / 2.0;
		constexpr double PI_OVER_3 = 3.14159265358979323846264338327950288 / 3.0;
		constexpr double PI_OVER_4 = 3.14159265358979323846264338327950288 / 4.0;

		//Single value functions
		template <typename T = float> T Sqrt(T value);								//Square root of value
		template <typename T = float> T Abs(T value);								//Absolute value
		template <typename T = float> T Floor(T value);								//Round down the value
		template <typename T = float> T Ceil(T value);								//Round up the value
		template <typename T = float> T Round(T value);								//Round the value to nearest whole number
		template <typename T = float> T Power(T base, T exponent);					//Raise a number to a certain exponent

		//Multi values functions
		template <typename T = float> T Clamp(T min, T max, T value);				//Clamp value at [min, max]
		template <typename T = float> T Wrap(T min, T max, T value);				//Wrap value around [min, max)
		template <typename T = float> T PingPong(T min, T max, T value);			//Ping pong a value around [min, max]
		template <typename T = float> T Min(T val1, T val2);						//Get the minimum of the 2 values
		template <typename T = float> T Max(T val1, T val2);						//Get the maximum of the 2 values
		template <typename T = float> T Lerp(T start, T end, T t);				    //Interpolate between start and end with unbounded t

		//Checking functions
		template <typename T = float> bool IsInRange(T min, T max, T value);		//Check if value is between min and max
		template <typename T = float> bool IsEqual(T val1, T val2);					//Checks if 2 values are approximately equal
																					
		//Angle conversion
		template <typename T = float> T DegToRad(T deg);
		template <typename T = float> T RadToDeg(T rad);

		//Trigonometry
		template <typename T = float> T SinRad(T rad);
		template <typename T = float> T CosRad(T rad);
		template <typename T = float> T TanRad(T rad);
		template <typename T = float> T SinDeg(T deg);
		template <typename T = float> T CosDeg(T deg);
		template <typename T = float> T TanDeg(T deg);

		//Inverse Trigonometry
		template <typename T = float> T ASinRad(T value);
		template <typename T = float> T ACosRad(T value);
		template <typename T = float> T ATanRad(T value);
		template <typename T = float> T ATanRad(T y, T x);
		template <typename T = float> T ASinDeg(T value);
		template <typename T = float> T ACosDeg(T value);
		template <typename T = float> T ATanDeg(T value);
		template <typename T = float> T ATanDeg(T y, T x);
    }
}

namespace BEMath = BE::Math;
#include "MathLib.hpp"

#endif