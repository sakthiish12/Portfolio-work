#pragma once
#include "ECSTracker.h"
#include "../../../Components/ParticleSystem.h"

namespace BE
{
	//Stores all entities that has specified components that the tracker needs

	class ParticleSystemTracker :public IECSTracker
	{
	public:

		//Additional updating needed, if any
		void AdditionalUpdateOnAddition(EntityID _eid);	//Additional things to do when adding entity for tracking

		void AdditionalUpdateOnRemoval(EntityID _eid);	//Additional things to do when removing entity for tracking
	};
}