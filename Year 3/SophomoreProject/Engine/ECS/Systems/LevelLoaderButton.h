// /******************************************************************************/
// /*!
// \file         LevelLoaderButton.h
// \project name Engine
// \author(s)    Ryan Wang 
// \date         12th Nov, 2021
// \brief        This file contains the declaration of the level loader button system
// \lines        14 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef LEVELLOADERBUTTON_H
#define LEVELLOADERBUTTON_H

#include "../ECS.h"

class LevelLoaderSystem : public systemBase
{
public:
	LevelLoaderSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<LevelLoaderSystem> LevelLoaderSysPtr;

#endif