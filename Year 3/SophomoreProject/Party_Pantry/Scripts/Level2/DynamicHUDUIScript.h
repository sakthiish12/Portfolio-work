// /******************************************************************************/
// /*!
// \file         DynamicHUDUIScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         15th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration of the Lever activation script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _DYNAMICHUDUISCRIPT_H_
#define _DYNAMICHUDUISCRIPT_H_

#include "../../../Engine/Messaging/ObserverMessage.h"

struct scriptBase;
struct DynamicHUDUIScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "DynamicHUDUIScript" }; //name identifier

	bool isActivated = false;//tracks if the script is activated 

	float cameraRatio; //ratio to update by 

	//original scale & transform
	float originalScaleX;
	float originalScaleY;
	float originalPosX;
	float originalPosY;

	//final scale & transform 
	float finalScaleX;
	float finalScaleY;

	//controls for increasing 
	const float increasingScaleX = 10.f;
	//const float increasingScaleY;

	entityID playerID;//tracks the player's id 

	//functions
	DynamicHUDUIScript(const char* scriptName = "DynamicHUDUIScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

};
#endif 