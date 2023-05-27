// /******************************************************************************/
// /*!
// \file         TimeDestructor.cpp
// \project name Engine
// \author(s)    Wilfred Ng
// \date         2nd Dec, 2021
// \brief        This file contains the definition of the system that handles
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

#include "TimeDestructor.h"//time destructor 

//extern
std::shared_ptr<TimeDestructor> TimeDestructorPtr;

void TimeDestructor::update(float dt) {
	std::vector<entityID> entitiesToDelete{};

	//Get which entities to delete first
	for (auto const& e : getEntitiesRegistered())
	{
		auto& oDestructor = ecs.getComponent<Com_TimeDestructor>(e);//getting the destructor component
		oDestructor.timeDelayDestructor -= dt; //Decrement the timer by dt
		if (oDestructor.timeDelayDestructor <= 0.0f)
			entitiesToDelete.emplace_back(e);
	}

	//Then take care of all of them
	for (entityID e : entitiesToDelete)
		ecs.deleteEntity(e);
}