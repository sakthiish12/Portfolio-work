// /******************************************************************************/
// /*!
// \file         MushroomBoss.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         13th Mar, 2022
// \brief        This file contains the declarations for the mushroom boss
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

#ifndef MUSHROOMBOSS_H
#define MUSHROOMBOSS_H

#include "../../Engine/ECS/component.h"
#include "EnemyBehaviour.h"

struct scriptBase;
struct MushroomBoss : EnemyBehaviour
{
	//List of states
	//GENERIC STATES:
	//idle    - Waiting for the player to be close enough. Invulnerable
	//reveal  - Reveal itself before starting to move
	//walk    - Walk slowly towards the player
	//stunned - Hitpoints = 1
	//suction - Getting sucked in by the vacuum
	//UNIQUE STATES:
	//summon  - Summoning a mushroom minion
	//shield  - Propped a shield up. Invulnerable
	enum class action {idle, reveal, walk, stunned, suction, summon, shield};
	action currentAction{ action::idle };
	bool dialogueActivated = false;//tracks if the dialogue is already shown 

	MushroomBoss(const char* scriptName = "MushroomBoss");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:
	float redFlashTimer = 0.0f;
	float redFlashInterval = 0.05f;
	bool redFlashOn = false;
	int lastFrameHealth = 0;

	entityID shieldID = 0;

	const float damageInterval = 1.0f;
	float damageCooldownRemaining = 0.0f;
	//bool shieldLoaded = false;
};

#endif



//#ifndef MUSHROOMBOSS_H
//#define MUSHROOMBOSS_H
//
//struct scriptBase;
//struct MushroomBoss : scriptBase
//{
//	bool alert{false};
//	float frameTime{ 0.0f };
//	float shotDelay{ 2.0f };
//	enum class facing { up, right, left, down };
//	facing facingDirection{ facing::up };
//	facing prevFacingDirection;//tracks the previous facing direction
//	bool changedDirection = true;
//	std::string name{ "MushroomBoss" };
//	MushroomBoss(const char* scriptName = "MushroomBoss");
//	virtual void init(float dt, unsigned int idToSet) override;
//	virtual void update(float dt) override;
//	virtual void end(float dt) override;
//
//	std::shared_ptr<scriptBase> createCopy() override;
//	std::string getScriptName();
//
//};
//
//
//
//