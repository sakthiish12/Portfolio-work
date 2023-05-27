// /******************************************************************************/
// /*!
// \file         MushroomDynamicCameraScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _MUSHROOMDYNAMICCAMERASCRIPT_H_
#define _MUSHROOMDYNAMICCAMERASCRIPT_H_

#include "../../../Engine/Messaging/ObserverMessage.h"

struct scriptBase;
struct MushroomDynamicCameraScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "MushroomDynamicCameraScript" }; //name identifier

	bool isActivated = false;//tracks if the script is activated 

	float originalCameraHeight;//tracks the original camera height 
	const float heightIncreasing = 50.f;//the rate of increasing the height
	const float heightChildIncreasingScale = 300.f;//this should apply for all the child 
	const float idealHeight = 500.f;//ideal height when encounter the mushroom
	entityID playerID;//tracks the player's id 

	//original scale of the camera 
	float originalScaleX;
	float originalScaleY;

	bool mushroomBossDead = false;//tracks if the mushroomboss is dead 

	//functions
	MushroomDynamicCameraScript(const char* scriptName = "MushroomDynamicCameraScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

};
#endif 