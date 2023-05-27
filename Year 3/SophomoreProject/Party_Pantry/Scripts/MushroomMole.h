// /******************************************************************************/
// /*!
// \file         MushroomMole.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Mar, 2022
// \brief        This file contains the declarations for the mushroom mole
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

#ifndef MUSHROOMMOLE_H
#define MUSHROOMMOLE_H

#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct MushroomMole : EnemyBehaviour
{
	//List of states
	//GENERIC STATES:
	//idle    - Not presently alert, waiting for the player to be near
	//alert   - Noted presence of player, reveal self before starting to move
	//active  - Chasing the player, potentially harming player
	//stunned - Stunned after getting damaged, can be sucked in
	//death   - Lost all health and is dying
	//suction - Getting sucked in by the vacuum
	//UNIQUE STATES:
	//digin   - Enemy digs into the ground, will be extremely vulnerable
	//burrow  - Enemy is underground, quickly approaching player, invulnerable
	//digout  - Enemy under player, digging out, still invulnerable
	enum class action { idle, alert, active, stunned, death, suction, digin, burrow, digout };
	action currentAction{ action::idle };

	MushroomMole(const char* scriptName = "MushroomMole");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
	std::vector <entityID> kids;
	Mathf::vec2 emitterVelocity = { 0.f,700.f };//velocity of the emitter
	Mathf::vec3 emitterPos;//emitter's position 
	const float emitterRangeX = 15.f;
	const float emitterRangeY = 30.f;

private:
	float currentDamageDelay = 0.0f;
	const float setDamageDelay = 1.0f;
};

#endif