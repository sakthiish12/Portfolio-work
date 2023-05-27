// /******************************************************************************/
// /*!
// \file         BossMushroom.h
// \project name Party_Pantry
// \author(s)    Poh Chun Ren
// // \date        29th Jan, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This is the logic for boss mushroom
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BOSS_MUSHROOM_SCRIPT_H_
#define _BOSS_MUSHROOM_SCRIPT_H_

struct scriptBase;
struct BossMushroomScript : scriptBase
{
	//base data
	std::string name{ "BossMushroomScript" }; //name identifier

	enum class BossState
	{
		INACTIVE,
		ACTIVE,
		DEAD
	} m_BossState = BossState::INACTIVE;

	//functions
	BossMushroomScript(const char* scriptName = "BossMushroomScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

	//WaveData
	int m_current_wave = 0; //current wave;
	bool m_is_current_wave_cleared = true; //if all summoned mushroom is dead
	const float boss_inactive_period = 5.f; //the time before boss summon next wave after current wave is cleared
	float m_boss_nextsummon_timer;

	//shield
	bool m_is_shield_activated = true;
	//float m_boss_attack_cooldown; //might be needed if decreasing hp by 1 everyframe
	//float m_boss_walk_rest_timer; //probably might need to allow player to escape etc



	//MinionPerWave
	unsigned int minionToSummonPerWave = 5;
	unsigned int activeMinion;

	//PlayerTriggerData
	/*
	Boss to be in center of island
	boss area is 800 by 800
	*/
	float x_diff_activate_trigger = 500.f;
	float y_diff_activate_trigger = 500.f;

	//PlayerData
	int playerID = -1; 
	bool playerComTypeexist = false;

	
private:
	int updatePlayerID();
	bool checkPlayerRadiusToBoss();
	void summonMushroomMinion(int numToSummon);
	bool checkIfMinionIsCleared();
	bool canBossSummon();
	void chasePlayer(float bossSpeed, float dt);
};

#endif 