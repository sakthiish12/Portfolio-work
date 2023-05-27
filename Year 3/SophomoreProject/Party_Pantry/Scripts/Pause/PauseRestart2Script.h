// /******************************************************************************/
// /*!
// \file         PauseRestart2Script.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _PAUSERESTART2SCRIPT_H_
#define _PAUSERESTART2SCRIPT_H_

struct scriptBase;
struct PauseRestart2Script : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseRestart2Script" }; //name identifier
	//functions
	PauseRestart2Script(const char* scriptName = "PauseRestart2Script");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 