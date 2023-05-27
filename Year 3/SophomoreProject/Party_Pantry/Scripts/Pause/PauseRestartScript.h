// /******************************************************************************/
// /*!
// \file         PauseRestartScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration of the pause restart script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PAUSERESTARTSCRIPT_H_
#define _PAUSERESTARTSCRIPT_H_

struct scriptBase;
struct PauseRestartScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseRestartScript" }; //name identifier
	int testCounter = 0;
	//functions
	PauseRestartScript(const char* scriptName = "PauseRestartScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 