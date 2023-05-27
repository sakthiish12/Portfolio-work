// /******************************************************************************/
// /*!
// \file         StopWatch.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        37
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "StopWatch.h"
#include <algorithm>
#include <chrono> //Frame time

auto current_time = std::chrono::system_clock::now();
auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
double num_seconds = duration_in_seconds.count();
float frameTime = 0.01f;
double GetTime()
{
	using namespace std::chrono;
	using SecondsFP = std::chrono::duration<double>;
	return duration_cast<SecondsFP>(high_resolution_clock::now().time_since_epoch()).count();
}
void StopWatch::StartTimer()
{
	m_CurrentTime = m_StartTime = GetTime();
}

void StopWatch::StopTimer()
{
	m_StartTime = 0;
	m_CurrentTime = 0;
	m_PreviousTime = 0;
}

void StopWatch::LapTimer()
{
	m_PreviousTime = m_CurrentTime;
	m_CurrentTime = GetTime();
}

float StopWatch::GetDeltaTime() const
{
	//auto endTime = std::chrono::high_resolution_clock::now();
	//deltaTime = endTime - startTime;
	//startTime = std::chrono::high_resolution_clock::now();
	//frameTime = deltaTime.count();

	const float dt = static_cast<float>(m_CurrentTime - m_PreviousTime);
	return std::min(dt, 0.25f); //imit delta time to 0.25f
}

float StopWatch::GetElapsedTime() const
{
	return static_cast<float>(num_seconds - m_StartTime);
}