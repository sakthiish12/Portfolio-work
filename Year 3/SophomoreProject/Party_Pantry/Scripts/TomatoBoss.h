// /******************************************************************************/
// /*!
// \file         TomatoBoss.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Mar, 2022
// \brief        This file contains the declarations for the tomato boss
//               enemy logic
// \lines        ??? Lines
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/


#ifndef TOMATOBOSS_H
#define TOMATOBOSS_H

#include "../../Engine/ECS/ECS.h"
#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct TomatoBoss : EnemyBehaviour
{
	//List of states
	//idle    - Waiting for the player to arrive. Not moving at all
	//active  - Chasing the player slowly
	//throwing- Throwing a bomb enemy
	//fuse    - Preparing to explode as a result of losing all health
	//explode - Exploding, prepare to disappear
	enum class action { idle, active, throwing, fuse, explode };
	action currentAction{ action::idle };

	TomatoBoss(const char* scriptName = "TomatoBoss");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:
	//For damage
	float redFlashTimer = 0.0f;
	float redFlashInterval = 0.05f;
	bool redFlashOn = false;

	//For fuse
	float flashTimer = 0.0f;
	bool flashOn = false;

	//For damaging player
	const float damageInterval = 1.0f;
	float damageCooldownRemaining = 0.0f;
};
#endif