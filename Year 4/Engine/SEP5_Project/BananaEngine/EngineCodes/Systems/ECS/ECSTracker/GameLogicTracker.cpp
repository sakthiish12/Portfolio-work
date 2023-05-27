#include "pch.h"
#include "GameLogicTracker.h"
#include "../ECS.h"
namespace BE
{
	void GameLogicTracker::AdditionalUpdateOnAddition(EntityID id)
	{
		ECS->GetComponent<GameLogic>(id).SetID(id);
	}

	void GameLogicTracker::AdditionalUpdateOnRemoval(EntityID )
	{
	}

}