// /******************************************************************************/
// /*!
// \file         ChilliCrackler.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         31st Mar, 2022
// \brief        This file contains the declarations for the chilli crackler
//               enemy logic
// \lines        25
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef CHILLICRACKLER_H
#define CHILLICRACKLER_H

#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct ChilliCrackler : EnemyBehaviour
{
	//States:
	//idle    - Not presently alert, waiting for player to be near
	//alert   - Noted presence of player, ready to act
	//chase   - Attempt to get close enough to the player, but back away if too near
	//prepare - Preparing to shoot out a burst of flames. Holds still in this position
	//fire    - Firing out a burst of flame. Vulnerable
	//stunned - Stunned after getting damaged, can be sucked in
	//death   - Lost all health and is dying
	//suction - Getting sucked in by the vacuum
	enum class action {idle, alert, chase, prepare, fire, stunned, death, suction};
	action currentAction{ action::idle };

	ChilliCrackler(const char* scriptName = "ChilliCrackler");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:
	//Delays between breathing bursts
	const float flameBurstDelay{ 3.0f };

	//Number of fireballs to spit out per burst
	const int maxFlameShots{ 5 };
	//Number of fireballs remaining in the current burst
	int flameShotsRemaining{ 5 };
	//Interval between shots
	const float flameThrowerInterval{ 0.2f };

	//Flash variables
	float flashTimer{ 0.0f };
	const float flashInterval{ 0.05f };
	bool flashOn{ false };
};

#endif