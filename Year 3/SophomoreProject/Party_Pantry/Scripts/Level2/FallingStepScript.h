// /******************************************************************************/
// /*!
// \file         FallingStepScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th Feb, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the falling step script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _FALLINGSTEPSCRIPT_H_
#define _FALLINGSTEPSCRIPT_H_
#include "../../../Engine/ECS/component.h"

struct scriptBase;
struct FallingStepScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "FallingStepScript" }; //name identifier

	entityID playerID;//stores the player id 
	const float radiusOfDetection = 100.f;//setting the radius of detection for the boulder script 

	const float offSetDetectionX = 200.f;//offset to trigger only after player is a good amount of way on the bridge 
	const float offSetDetectionXOld = 150.f;//offset to trigger only after player is a good amount of way on the bridge 
	const float shrinkBridgeBuffer = 50.f;
	const float offSetFallingDetectionX = 70.f;//offset the falling off while on the bridge 

	const float offSetDetectionXOldLeft = 95.f;//offset to trigger only after player is a good amount of way on the bridge 
	

	//original positions
	float originalScaleX;
	float originalScaleY;
	float originalPosX;
	float originalPosY;
	
	bool shrinkingBridge = false;//toggle for the shrinking bridge 
	float scaleShrinking = 280.f;//the rate at which the bridge shrinks 
	bool shrinkCompleted = false;//tracks if the shrinking has been completed 

	//functions
	FallingStepScript(const char* scriptName = "FallingStepScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 