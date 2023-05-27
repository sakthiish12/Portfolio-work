// /******************************************************************************/
// /*!
// \file         TomatoTimebomb.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Mar, 2022
// \brief        This file contains the declarations for the tomato timebomb
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

#ifndef TOMATOTIMEBOMB_H
#define TOMATOTIMEBOMB_H

#include "../../Engine/ECS/ECS.h"
#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct TomatoTimebomb : EnemyBehaviour
{
	//List of states
	//idle    - waiting for the player to be close enough
	//alert   - made aware of the player, preparing to chase
	//chase   - chasing the player
	//stunned - stunned as a result of getting hit
	//fuse    - preparing to explode and damage the player
	//explode - exploding, prepare to delete as animation finishes
	//suction - being sucked in
	enum class action { idle, alert, chase, stunned, fuse, explode, suction };
	action currentAction{ action::idle };

	TomatoTimebomb(const char* scriptName = "TomatoTimebomb");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:
	//Alternate colouration from normal tomato troopers
	//Somewhat darker, a lot greener
	Mathf::Vector4 colouration{ 0.5f, 2.0f, 1.0f, 1.0f };

	float flashTimer = 0.0f;
	bool flashOn = false;
};
#endif