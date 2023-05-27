// /******************************************************************************/
// /*!
// \file         UpgradeManager.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the AssetManager
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "UpgradeManager.h"


void UpgradeManager::clear() {
	//clearing the data 
	numberOfTomatoesToDeduct = 0;
	numberOfMushroomToDeduct = 0;
	numberOfChilliesToDeduct = 0;
}