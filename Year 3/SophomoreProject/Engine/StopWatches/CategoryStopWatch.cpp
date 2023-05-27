// /******************************************************************************/
// /*!
// \file         CategoryStopWatch.cpp
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
#include "CategoryStopWatch.h"

void CategoryStopWatch::StartTimer()
{
	m_StopWatch.StartTimer();
}

void CategoryStopWatch::StopTimer()
{
	m_LappedCategories.clear();
	m_StopWatch.StopTimer();
}

void CategoryStopWatch::LapTimer(const std::string& categoryName)
{
	// TODO: void this function on Release mode (?)
	m_StopWatch.LapTimer();

	if (m_LappedCategories[categoryName].size() >= m_TickCount)
		m_LappedCategories[categoryName].pop_front();

	m_LappedCategories[categoryName].push_back(m_StopWatch.GetDeltaTime());
}

float CategoryStopWatch::GetElapsedTime() const
{
	return static_cast<float>(m_StopWatch.GetElapsedTime());
}

CategoryStopWatch::CategoryTimeMap CategoryStopWatch::GetLappedCategories() const
{
	return m_LappedCategories;
}
