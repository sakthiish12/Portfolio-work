// /******************************************************************************/
// /*!
// \file         ChilliesCounterScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         24th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the mushroom scene text script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _CHILLIESCOUNTERSCRIPT_H_
#define _CHILLIESCOUNTERSCRIPT_H_

struct scriptBase;
struct ChilliesCountScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "ChilliesCountScript" }; //name identifier

	size_t mushroomsCollected = 0;//tracks the number of mushrooms collected
	int prevMushroomCollected = -1;//tracks the previous number of mushrooms collected 

	//effects
	float originalScaleX;
	float originalScaleY;
	float targetScaleX;
	float targetScaleY;
	bool effectTrigger = false;
	bool isShrinking = false;
	const float enlargingRate = 0.8f;

	//functions
	ChilliesCountScript(const char* scriptName = "ChilliesCountScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 