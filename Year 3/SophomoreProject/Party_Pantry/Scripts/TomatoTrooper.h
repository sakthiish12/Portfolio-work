// /******************************************************************************/
// /*!
// \file         TomatoTrooper.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         27th Feb, 2022
// \brief        This file contains the declarations for the tomato trooper
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

#ifndef TOMATOTROOPER_H
#define TOMATOTROOPER_H

#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct TomatoTrooper : EnemyBehaviour
{
	//List of states
	//GENERIC STATES:
	//idle    - Not presently alert, waiting for the player to be near
	//alert   - Noted presence of player, ready to act
	//active  - Chasing the player
	//stunned - Stunned after getting damaged, can be sucked in
	//death   - Lost all health and is dying
	//suction - Getting sucked in by the vacuum
	//UNIQUE STATES:
	//prepare - Enemy walks back a little before charging
	//charge  - Charge ahead at a faster speed in an attempt to hurt the player
	//recover - Recovering after charging
	enum class action {idle, alert, active, stunned, death, suction, prepare, charge, recover};
	action currentAction{ action::idle };

	static const float playerKnockback_cooldown_max;
	static float playerKnockback_cooldown;

	TomatoTrooper(const char* scriptName = "TomatoTrooper");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
	
private:

};

#endif