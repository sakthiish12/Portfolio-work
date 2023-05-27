// /******************************************************************************/
// /*!
// \file         BoulderScript.h
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
#ifndef _BOULDERSCRIPT_H_
#define _BOULDERSCRIPT_H_
#include "../../../Engine/ECS/component.h"

struct scriptBase;
struct BoulderScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BoulderScript" }; //name identifier
	entityID playerID;//stores the player id 
	const float radiusOfDetectionY = 80.f;//setting the radius of detection for the boulder script 
	const float radiusOfDetectionX = 600.f;//setting the radius of detection for the boulder script 
	bool boulderActive = false;// tracks if the boulder is active 
	bool stageCompleted = false;//tracks if the player completed the boulder portion

	const float boulderEndPosX = 3215.f;

	//starting position of the boulder
	float startingPosX;
	float startingPosY;

	float rotationSpeed = 40.f;//rotation speed of the boulder 
	float maxRotationSpeed = 80.f;//max rotation speed 
	float rotationSpeedIncrement = 12.f;//increment for the rotation speed 

	float translateSpeed = 190.f;//speed moving towards right
	float maxTranslateSpeed = 250.f;//speed moving towards right max
	float translationSpeedIncrement = 80.f;//increment for the translation speed 

	//constants for the shake range 
	const float shakeUpperRange = 4.f;
	const float shakeLowerRange = 4.f;

	//functions
	BoulderScript(const char* scriptName = "BoulderScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 