// /******************************************************************************/
// /*!
// \file         CategoryStopWatch.h
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        25
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include "StopWatch.h"
#include <string>
#include <map>
#include <deque>
// Time measuring class with category
class CategoryStopWatch
{

public:
	using CategoryTimeMap = std::map<std::string, std::deque<float>>;

	// Starts the Stopwatch timer
	void StartTimer();

	// Stops and reset all lapped timers
	void StopTimer();

	// Laps the timer with a given Category name to allow retrieval of LappedCategories()
	void LapTimer(const std::string& categoryName);

	// Sets the smoothing of values for each Category
	// * Default value is 1 (No smoothing)
	void SetMaxCategoryTicks(size_t tickCount) { m_TickCount = tickCount; }

	// Returns the time since the timer started
	float GetElapsedTime() const;

	// Returns the Categories that were lapped
	CategoryTimeMap GetLappedCategories() const;

private:
	StopWatch m_StopWatch;

	CategoryTimeMap m_LappedCategories;

	size_t m_TickCount{ 1 };

};