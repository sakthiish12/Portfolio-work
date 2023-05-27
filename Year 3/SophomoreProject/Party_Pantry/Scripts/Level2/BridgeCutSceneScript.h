// /******************************************************************************/
// /*!
// \file         BridgeCutSceneScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         4th April, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _BRIDGECUTSCENESCRIPT_H_
#define _BRIDGECUTSCENESCRIPT_H_
#include "../../../Engine/ECS/component.h"
struct scriptBase;
struct BridgeCutSceneScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BridgeCutSceneScript" }; //name identifier
	entityID playerID;//stores the player id 
	bool active = false; //tracks if the script is active 
	entityID cutSceneEntity;//stores the cut scene entity 
	bool createdEntity = false;
	bool secondLoaded = false;
	bool completed = false;

	//functions
	BridgeCutSceneScript(const char* scriptName = "BridgeCutSceneScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 