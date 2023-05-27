// /******************************************************************************/
// /*!
// \file         PerformancePanel.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         8th Oct, 2021
// \brief        Drawing collision boundary using graphic debugging
// \lines        77 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once
#include "../../StopWatches/CategoryStopWatch.h"
#include "../../EditorStateManager/EditorStates/gameState.h"
#include <vector>
struct metrics
{
public:
	CategoryStopWatch systemExecutionTimings;

	std::vector<float> m_DeltaTimeList;
	SceneMetrics m_Metrics{};
	
};