// /******************************************************************************/
// /*!
// \file         FadeInOutScript.h
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
#ifndef _FADEINOUTSCRIPT_H_
#define _FADEINOUTSCRIPT_H_

struct scriptBase;
struct FadeInOutScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "FadeInOutScript" }; //name identifier
	bool active = true;//indicates if it's active 
	bool fadeInOut = true; //true fade in , false fade out 

	const float speedOfTransition = 0.5f;//the speed of the transition

	//functions
	FadeInOutScript(const char* scriptName = "FadeInOutScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 