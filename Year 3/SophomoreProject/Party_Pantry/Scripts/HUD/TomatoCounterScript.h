// /******************************************************************************/
// /*!
// \file         TomatoCounterScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         10th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the tomato scene text script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _TOMATOCOUNTERSCRIPT_H_
#define _TOMATOCOUNTERSCRIPT_H_

struct scriptBase;
struct TomatoCountScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "TomatoCountScript" }; //name identifier

	size_t tomatoesCollected = 0;//tracks the number of tomatoes collected

	int prevTomatoesCollected = -1;//tracks the previouse number of tomatoes 

	//effects
	float originalScaleX;
	float originalScaleY;
	float targetScaleX;
	float targetScaleY;
	bool effectTrigger = false;
	bool isShrinking = false;
	const float enlargingRate = 0.8f;

	//functions
	TomatoCountScript(const char* scriptName = "TomatoCountScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 