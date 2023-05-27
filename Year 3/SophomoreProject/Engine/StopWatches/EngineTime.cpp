// /******************************************************************************/
// /*!
// \file         EngineTime.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        35
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "EngineTime.h"
#include <corecrt_math.h>
#include "StopWatch.h"
// Static variable definition
float Time::deltaTime = 0;
float Time::elapsedTime = 0;
float Time::timeScale = 1;
float Time::physicsAlpha = 0;
// End definition

void Time::SetTimeScale(float value)
{
	timeScale = fabsf(value);
}

void Time::AddTimeScale(float value)
{
	timeScale += fabsf(value);
}

float Time::FixedDeltaTime()
{
	return 1.0f / 80.0f;
}

void Time::UpdateTime(const StopWatch& stopWatch)
{
	deltaTime = stopWatch.GetDeltaTime();
	elapsedTime += stopWatch.GetDeltaTime();
}


void Time::Reset()
{
	deltaTime = 0;
	elapsedTime = 0;
	timeScale = 1;
	physicsAlpha = 0;
}
