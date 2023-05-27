// /******************************************************************************/
// /*!
// \file         BoulderCameraScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Feb, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _BOULDERCAMERASCRIPT_H_
#define _BOULDERCAMERASCRIPT_H_
#include "../../../Engine/ECS/component.h"

struct scriptBase;
struct BoulderCameraScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BoulderCameraScript" }; //name identifier
	entityID playerID;//stores the player id 
	entityID boulderID;//stores the boulder id

	float originalCameraHeight; //stores the original camera height 
	bool isActive = false;//tracks if the transition is active 
	bool vectorReceived = false;//tracks if the boulder to player vector is received 
	bool isCompleted = false;//tracks if the script is completed 
	bool skippedTransition = false;//to skip transition 

	Mathf::vec2 playerToBoulder;
	const float rateOfTransition = 120.f;

	const float heightTransition = 50.f;
	const float cameraMinHeight = 130.f;//min height 

	//functions
	BoulderCameraScript(const char* scriptName = "BoulderCameraScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 