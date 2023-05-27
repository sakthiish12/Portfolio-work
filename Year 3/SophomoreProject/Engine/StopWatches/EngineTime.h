// /******************************************************************************/
// /*!
// \file         EngineTime.h
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        22
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once


class StopWatch;

class Time
{

public:

	// How much the delta & elapsed time is being scaled by
	static float TimeScale() { return timeScale; }

	// Modifying TimeScale allows you to create slow/fast motion effects
	// * Positive values only
	static void SetTimeScale(float value);

	// Modifying TimeScale allows you to create slow/fast motion effects
	// * Can be a negative/positive value
	static void AddTimeScale(float value);

	// Time between current & previous frames in seconds
	// * Value unaffected by TimeScale
	static float UnscaledDeltaTime() { return deltaTime; }

	// Total elapsed time since beginning of the application in seconds
	// * Value unaffected by TimeScale
	static float UnscaledElapsedTime() { return elapsedTime; }

	// Time between each Physics Time-step
	static float FixedDeltaTime();

	// Time between current & previous frames in seconds
	static float DeltaTime() { return deltaTime * timeScale; }

	// Total elapsed time since beginning of the application in seconds
	static float ElapsedTime() { return elapsedTime * timeScale; }

	// Updates internal variables using a StopWatch
	static void UpdateTime(const StopWatch& stopWatch);


	// Reset all members back to default value
	static void Reset();

private:
	Time() = default;

	static float deltaTime;
	static float elapsedTime;
	static float timeScale;
	static float physicsAlpha;
};