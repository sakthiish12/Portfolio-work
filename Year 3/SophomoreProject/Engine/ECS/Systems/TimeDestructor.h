// /******************************************************************************/
// /*!
// \file         TimeDestructor.h
// \project name Engine
// \author(s)    Wilfred Ng
// \date         2nd Dec, 2021
// \brief        This file contains the declaration of the system that handles
//				 management and destruction of entities that have limited lifespans
// \lines        ??? Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef TIME_DESTRUCTOR
#define TIME_DESTRUCTOR

#include "../../ECS/ECS.h"//ecs

class TimeDestructor : public systemBase
{
public:
	TimeDestructor(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<TimeDestructor> TimeDestructorPtr;

#endif // !TIME_DESTRUCTOR
