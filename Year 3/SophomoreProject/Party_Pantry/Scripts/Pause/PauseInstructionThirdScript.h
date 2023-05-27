// /******************************************************************************/
// /*!
// \file         PauseInstructionThirdScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         4th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration of the pause instruction second script
// \lines        14
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PAUSEINSTRUCTIONTHIRDSCRIPT_H_
#define _PAUSEINSTRUCTIONTHIRDSCRIPT_H_

struct scriptBase;
struct PauseInstructionThirdScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseInstructionThirdScript" }; //name identifier
	int testCounter = 0;
	int clickCounter = 0;
	//functions
	PauseInstructionThirdScript(const char* scriptName = "PauseInstructionThirdScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 