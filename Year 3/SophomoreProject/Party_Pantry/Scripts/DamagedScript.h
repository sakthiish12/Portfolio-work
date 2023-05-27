// /******************************************************************************/
// /*!
// \file         DamagedScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains function declaration of the damaged script  
// \lines        15
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _DAMAGEDSCRIPT_H_
#define _DAMAGEDSCRIPT_H_

struct scriptBase;
struct DamagedScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "DamagedScript" }; //name identifier
	int prevHealth;//tracks the previous health 
	const float effectTimer = 0.5f;//how long the effect last
	float currentTimer = 0.f;//keeps track of the current time 

	//constants for the shake range 
	const float shakeUpperRange = 3.f;
	const float shakeLowerRange = 3.f;


	//functions
	DamagedScript(const char* scriptName = "DamagedScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 