// /******************************************************************************/
// /*!
// \file         EnemyStateSystem.h
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
#ifndef ENEMYSTATE_SYS
#define ENEMYSTATE_SYS

#include "../system.h"
#include "../../ECS/ECS.h"//ecs 

//try to have entity 0 as a player controlled obj
class EnemyStateSys : public systemBase
{

public:

	EnemyStateSys(bool requireComponentMatch = false);
	~EnemyStateSys() = default;

	//delete fncs
	EnemyStateSys(EnemyStateSys& rhs) = delete;
	EnemyStateSys(const EnemyStateSys& rhs) = delete;
	EnemyStateSys& operator=(const EnemyStateSys& rhs) = delete;
	EnemyStateSys& operator=(const EnemyStateSys& rhs) const = delete;


	//enemy fncs 
	void init();
	void update(float dt);
	void updateSteps(float dt);
	void ChangeState(Com_EnemyStates::states newState);

	//states 
	void IDLE_ENTER();
	void IDLE_UPDATE();
	void IDLE_EXIT();

	void MOVE_ENTER();
	void MOVE_UPDATE();
	void MOVE_EXIT();

	void ATTACK_ENTER();
	void ATTACK_UPDATE();
	void ATTACK_EXIT();


	//function pointer for the states 
	using FP_STATES = void(EnemyStateSys::*)();
	FP_STATES _fp_states[9] = { &EnemyStateSys::IDLE_ENTER, &EnemyStateSys::IDLE_UPDATE, &EnemyStateSys::IDLE_EXIT,
								&EnemyStateSys::MOVE_ENTER, &EnemyStateSys::MOVE_UPDATE, &EnemyStateSys::MOVE_EXIT,
								&EnemyStateSys::ATTACK_ENTER, &EnemyStateSys::ATTACK_UPDATE, &EnemyStateSys::ATTACK_EXIT };

private:
	float	_turn_step{ 0.5f };
	float	_turn_step_counter{ _turn_step };
	bool	_turn{ false };
	entityID _player_id { static_cast<entityID>(-1) };//getting the entity id of the player 
	//Entity* _player{ nullptr };
	//Grid* _grid{ nullptr };
	entityID _tilemap{ static_cast<entityID>(-1) };//getting the entity id of the tile map 
};

extern std::shared_ptr<EnemyStateSys> EnemyStateSysPtr;

#endif //ENEMYSTATE_SYS