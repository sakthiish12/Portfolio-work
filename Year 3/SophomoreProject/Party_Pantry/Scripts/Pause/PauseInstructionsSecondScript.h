// /******************************************************************************/
// /*!
// \file         PauseInstructionsSecondScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _PAUSEINSTRUCSECONDSCRIPT_H_
#define _PAUSEINSTRUCSECONDSCRIPT_H_

struct scriptBase;
struct PauseInstrucSecondScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseInstrucSecondScript" }; //name identifier
	int testCounter = 0;
	int clickCounter = 0;
	//functions
	PauseInstrucSecondScript(const char* scriptName = "PauseInstrucSecondScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 