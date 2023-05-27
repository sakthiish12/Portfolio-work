// /******************************************************************************/
// /*!
// \file         LeverActivationScript.h
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
#ifndef _LEVERACTIVATIONSCRIPT_H_
#define _LEVERACTIVATIONSCRIPT_H_

#include "../../Engine/Messaging/ObserverMessage.h"

struct scriptBase;
struct LeverActivationScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "LeverActivationScript" }; //name identifier

	const float radiusOfDetection = 100.f;//setting the radius of detection for the proximity script 
	bool textLoaded = false;//indicates if the text is already loaded 
	entityID textId;//stores the text id 
	entityID playerID;//stores the player id 
	bool leverTriggered = false;//tracks if the lever has been triggered 

	bool cameraPanned = false;//tracks if the camera panning is already done 
	const float cameraPanningSpeed = 100.f;
	const float cameraPanningHeightSpeed = 20.f;
	float cameraOriginalHeight;//stores the camera original height 


	//the end desired camera position and height 
	const float desiredCameraPosX = 2973.515f;
	const float desiredCameraHeight = 320.f;

	//functions
	LeverActivationScript(const char* scriptName = "LeverActivationScript");
	~LeverActivationScript();
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();


	Messaging::Event<bool> LeverHasBeenTriggeredStatus;
};
extern LeverActivationScript* leverScriptPtr;
#endif 