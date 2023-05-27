// /******************************************************************************/
// /*!
// \file         SpaceToSkipScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration skip scene script
// \lines        13
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _SPACETOSKIPSCRIPT_H_
#define _SPACETOSKIPSCRIPT_H_

struct scriptBase;
struct SpaceToSkipScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "SpaceToSkipScript" }; //name identifier
	//functions
	SpaceToSkipScript(const char* scriptName = "SpaceToSkipScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 