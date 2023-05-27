// /******************************************************************************/
// /*!
// \file         Stopwatch.h
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        16
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once


// Small helper Time measuring class
class StopWatch
{
	
public:

	StopWatch() = default;
	~StopWatch() = default;
	// Starts the Stopwatch timer
	void StartTimer();

	// Stops and reset the timer
	void StopTimer();

	// Laps the timer to allow retrieval of DeltaTime()
	void LapTimer();

	// Returns the time between current time and lapped time
	float GetDeltaTime() const;

	// Returns the time since the timer started
	float GetElapsedTime() const;


private:
	double m_StartTime{};
	double m_CurrentTime{};
	double m_PreviousTime{};
};