// /******************************************************************************/
// /*!
// \file         SpaceToSkipScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         11th April 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _SPACETOSKIPENDSCENESCRIPT_H_
#define _SPACETOSKIPENDSCENESCRIPT_H_

struct scriptBase;
struct SpaceToSkipEndSceneScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "SpaceToSkipEndSceneScript" }; //name identifier
	//functions
	SpaceToSkipEndSceneScript(const char* scriptName = "SpaceToSkipEndSceneScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 