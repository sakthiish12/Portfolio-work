// /******************************************************************************/
// /*!
// \file         HUDUIScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         11th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the abilities ui script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _HUDUISCRIPT_H_
#define _HUDUISCRIPT_H_

struct scriptBase;
struct HUDUIScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "HUDUIScript" }; //name identifier

	bool superSpeedUnlocked = false;//tracks if the super speed is unlocked
	bool shieldUnlocked = false;//tracks if the shield is unlocked 
	bool fireballUnlocked = false;//tracks if the fireball ability is unlocked 

	//enum class for airblast stages 
	enum class airBlastStages {
		none,
		first,
		second,
		third,
		fourth
	};

	airBlastStages stage = airBlastStages::none;

	//functions
	HUDUIScript(const char* scriptName = "HUDUIScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 