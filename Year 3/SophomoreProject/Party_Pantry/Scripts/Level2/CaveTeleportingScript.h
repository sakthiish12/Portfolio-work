// /******************************************************************************/
// /*!
// \file         CaveTeleportingScript.h
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
#ifndef _CAVETELEPORTSCRIPT_H_
#define _CAVETELEPORTSCRIPT_H_
#include "../../../Engine/ECS/component.h"
extern bool teleportActived;
struct scriptBase;
struct CaveTeleportScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "CaveTeleportScript" }; //name identifier
	entityID playerID;//stores the player id 
	const float radiusOfDetection = 100.f;//setting the radius of detection for the boulder script 

	bool teleportActive = false;//toglge for teleporting the player to the cave 
	//bool teleportActived = false;//toglge for teleporting the player to the cave 
	bool darkenToggle = true;//toggle for the darkening of the scene 
	const float speedOfTransition = 0.5f;//the speed of the transition

	//the teleporting cave position 
	const float cavePosX = 11.010f;
	const float cavePosY = -3467.737f;

	//functions
	CaveTeleportScript(const char* scriptName = "CaveTeleportScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 