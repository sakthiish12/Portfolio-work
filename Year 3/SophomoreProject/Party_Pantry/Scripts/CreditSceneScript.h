// /******************************************************************************/
// /*!
// \file         CreditSceneScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         4th April, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _CREDITSCENESCRIPT_H_
#define _CREDITSCENESCRIPT_H_

struct scriptBase;
struct CreditSceneScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "CreditSceneScript" }; //name identifier
	float endingPosY = -4552.410f;
	float transitionSpeed = 60.f;
	float startingPosY;

	//functions
	CreditSceneScript(const char* scriptName = "CreditSceneScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 