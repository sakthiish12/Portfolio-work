// /******************************************************************************/
// /*!
// \file         CameraSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declaration for the camera system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef CAMERA_SYS
#define CAMERA_SYS
#include <iostream>
#include "../ECS.h"

/***************************************
* @Brief : this is the camera 2D system
****************************************/
class SerializedCamera2DSystem : public systemBase
{
public:
	SerializedCamera2DSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<SerializedCamera2DSystem> SerializedCamera2DSys;

#endif