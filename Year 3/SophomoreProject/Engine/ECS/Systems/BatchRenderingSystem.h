// /******************************************************************************/
// /*!
// \file         BatchRenderingSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef BATCH_SYS
#define BATCH_SYS


#include <iostream>
#include "../ECS.h"

/***************************************
* @Brief : this is the renderer system for the ecs
****************************************/
class SerializedRenderer2DSystem : public systemBase
{
public:
	SerializedRenderer2DSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<SerializedRenderer2DSystem> SerializedRenderer2DSys;
#endif // !BATCH_SYS