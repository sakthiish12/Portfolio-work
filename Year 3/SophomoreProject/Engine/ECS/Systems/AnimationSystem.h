// /******************************************************************************/
// /*!
// \file         AnimationSystem.h
// \project name Engine
// \author(s)    Ryan Wang 
// \date         1st Mar, 2022
// \brief        This file contains the declaration of the animation system
//	             that facilitates animation of sprites
// \lines        ?? Lines
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS.h"

class AnimationSystem : public systemBase
{
public:
	AnimationSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<AnimationSystem> AnimationSysPtr;

#endif