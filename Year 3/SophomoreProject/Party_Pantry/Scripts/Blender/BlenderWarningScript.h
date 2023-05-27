// /******************************************************************************/
// /*!
// \file         BlenderWarningScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         28th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _BLENDERWARNINGSCRIPT_H_
#define _BLENDERWARNINGSCRIPT_H_

struct scriptBase;
struct BlenderWarningScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BlenderWarningScript" }; //name identifier

	//effects
	float originalScaleX;
	float originalScaleY;
	float targetScaleX;
	float targetScaleY;
	bool effectTrigger = false;
	bool isShrinking = false;
	const float enlargingRate = 45.f;

	//functions
	BlenderWarningScript(const char* scriptName = "BlenderWarningScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 