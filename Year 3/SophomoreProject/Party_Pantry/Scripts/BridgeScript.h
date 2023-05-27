// /******************************************************************************/
// /*!
// \file         BridgeScript.h
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
#ifndef _BRIDGESCRIPT_H_
#define _BRIDGESCRIPT_H_

#include "../../Engine/Messaging/ObserverMessage.h"

struct scriptBase;
struct BridgeScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BridgeScript" }; //name identifier
	bool activated = false;//tracks the first entrance of the script 
	//const float radiusOfDetection = 300.f;//setting the radius of detection for the proximity script 
	const float sizeIncreasingScale = 40.f;
	float startingXPos;//tracks the starting x pos 
	float expectedEndingXpos;
	entityID playerID;//stores the player id 

	const float maxLength = 400.f;//tracks the max scale x of the bridge 
	bool completedOpening = false;//tracks if the bridge is fully opened 

	//functions
	BridgeScript(const char* scriptName = "BridgeScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

	void updateLeverStatus(bool);
	bool connectedToEvents = false;
	Messaging::Delegate<BridgeScript, bool> LeverHasBeenTriggeredStatus { *this, &BridgeScript::updateLeverStatus };
};

#endif 