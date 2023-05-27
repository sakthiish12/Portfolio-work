// /******************************************************************************/
// /*!
// \file         UpgradeDialogue3Script.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Feb, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the teleporting to enter the cave script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _UPGRADEDIALOGUE3SCRIPT_H_
#define _UPGRADEDIALOGUE3SCRIPT_H_
#include "../../../Engine/ECS/component.h"
struct scriptBase;
struct UpgradeDialogue3Script : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeDialogue3Script" }; //name identifier
	bool dialogueActivated = false;
	entityID playerID;//stores the player id 
	//functions
	UpgradeDialogue3Script(const char* scriptName = "UpgradeDialogue3Script");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 