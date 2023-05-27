// /******************************************************************************/
// /*!
// \file         CameraTransitionScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the fade in and fade out script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _CAMERATRANSITIONSCRIPT_H_
#define _CAMERATRANSITIONSCRIPT_H_

struct scriptBase;
struct CameraTransitionScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "CameraTransitionScript" }; //name identifier
	bool toggleActive = true;//indicates if the transition is active 

	bool firstMovement = true;//camera panning first movement 
	bool secondMovement = false;//camera panning second movement 
	bool panningActivated = false;//tracks if the camera panning has already been activated 

	float currentTime = 0.f;
	float timeToChange = 3.f;//3 seconds to change to next level 
	bool firstClicked = false;//tracks if the mouse is already first clicked 
	bool secondClicked = false;

	bool skippedPanning = false;
	const float endingPosX = 1117.f;
	const float endingPosY = -780.f;

	bool fadeInOut = true;//fade in 
	const float speedOfTransition = 0.15f;

	//functions
	CameraTransitionScript(const char* scriptName = "CameraTransitionScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 