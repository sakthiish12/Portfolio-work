// /******************************************************************************/
// /*!
// \file         UpgradeManager.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration for the asset manager 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef UPGRADE_MANAGER
#define UPGRADE_MANAGER

#include <iostream>

/********************
@ Brief - In charge of managing all assets of game
*******************/
class UpgradeManager {
public:
	static UpgradeManager& GetInstance() {
		static UpgradeManager instance; return instance;
	} //singleton get instance 

	UpgradeManager() = default; //cstor
	~UpgradeManager() = default; //dstor 

	void clear();//clearing all the settings


public:

	enum class Next_Upgrade
	{
		None,
		Speed,
		Shield,
		Fireball
	};

	Next_Upgrade next_upgrade = Next_Upgrade::None;

	//tracks all the existing upgrades
	bool upgradeSuperSpeed = false;
	bool upgradeSuperShield = false;
	bool upgradeFireBall = false;

	//tracks the type of upgrade 
	size_t numberOfTomatoesToDeduct = 0;//tracks the number of tomatoes to deduct	
	size_t numberOfMushroomToDeduct = 0;//tracks the number of mushrooms to deduct
	size_t numberOfChilliesToDeduct = 0;//tracks the number of chillies to deduct 

	bool showUpgradeTutorial = true;//show the upgrade tutorial 

	bool upgradeTutorial = false;//is it upgrade tutorial

	bool level1Completed = false; //check if level 1 is completed 
	bool level2Completed = false; //check if level 2 is completed 

	//tracks for the upgrade completed 
	bool part1UpgradeTutorial = true;
	bool part2UpgradeTutorial = true;

	bool partUpgradeSpeedTutorial = true;
	bool partUpgradeSpeedTutorial2 = true;
	bool partUpgradeSpeedTutorial3 = true;

	bool partUpgradeShieldTutorial1 = true;
	bool partUpgradeShieldTutorial2 = true;

	bool partUpgradeFireballTutorial1 = true;
	bool partUpgradeFireballTutorial2 = true;
};

#endif // ! Upgrade Manager
