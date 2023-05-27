#pragma once
#ifndef GAMELOGIC_SYSTEM_HPP
#define GAMELOGIC_SYSTEM_HPP

#include "../ISystem.h"
#include "ScriptManager.h"

namespace BE
{
	class GameLogicSystem : public Singleton<GameLogicSystem>, public ISystem
	{
		bool loaded = false;
		EntityID PlayerID = NULL_ENTITY;
	public:

		ISYSTEM_FUNCTIONS;
		friend class ScriptManager;
		inline EntityID GetPlayerID() const {return PlayerID;}
		inline void SetPlayerID(EntityID in) { PlayerID = in;}
	};

	//GLS = Game logic system
#define GLS GameLogicSystem::GetInstance()

}


#endif 