// /******************************************************************************/
// /*!
// \file         EnemySpawningSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declarations of the enemy spawning system
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef ENEMY_SPAWNING_SYS
#define ENEMY_SPAWNING_SYS

#include "../../ECS/ECS.h"//ecs

class EnemySpawningSystem : public systemBase
{
public:
	EnemySpawningSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<EnemySpawningSystem> EnemySpawningSysPtr;



#endif // !ENEMY_SPAWNING_SYS
