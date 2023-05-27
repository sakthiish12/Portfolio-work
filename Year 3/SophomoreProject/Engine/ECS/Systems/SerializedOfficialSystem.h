// /******************************************************************************/
// /*!
// \file         OfficialSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the systems of the official test state.
// \lines        60 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef OFFICIALSYS_H
#define OFFICIALSYS_H

#include <iostream>
//#include <random>
//#include <chrono>
#include "../ECS.h"


/***************************************
* @Brief : this is the transform system system
****************************************/
class SerializedTransform2DSystem : public systemBase
{
public:
	SerializedTransform2DSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{


	}

	void update(float dt);

	//Update Transform Matrices: Scale, Rotate, Translate
	void updateMatrix();

};
extern std::shared_ptr<SerializedTransform2DSystem> SerializedTransform2DSys;

/***************************************
* @Brief : this is the sprite system
****************************************/
class SerializedSprite2DSystem : public systemBase
{
public:
	SerializedSprite2DSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
	void updateSteps(float dt);
};
extern std::shared_ptr<SerializedSprite2DSystem> SerializedSprite2DSys;

#endif