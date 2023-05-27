#pragma once
#include "ECSTracker.h"

#ifndef	GAMELOGIC_TRACKER_H
#define GAMELOGIC_TRACKER_H

namespace BE
{
	//Stores all entities that has specified components that the tracker needs

	class GameLogicTracker :public IECSTracker
	{
	public:

		//Additional updating needed, if any
		void AdditionalUpdateOnAddition(EntityID);

		void AdditionalUpdateOnRemoval(EntityID);
	};
}

#endif