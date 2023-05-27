// /******************************************************************************/
// /*!
// \file         TransformParentSystem.h
// \project name Engine
// \author(s)    Ryan Wang 
// \date         25th Nov, 2021
// \brief        This file contains the declaration of the tranform parent system
//				 that calculates world matrix of entity based off its parents
// \lines        14 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef TRANSFORMPARENTSYSTEM_H
#define TRANSFORMPARENTSYSTEM_H

#include "../ECS.h"

class TransformParentSystem : public systemBase
{
public:
	TransformParentSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update();
};
extern std::shared_ptr<TransformParentSystem> TransformParentSysPtr;

#endif