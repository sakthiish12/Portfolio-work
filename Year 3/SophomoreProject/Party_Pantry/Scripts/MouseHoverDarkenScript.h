// /******************************************************************************/
// /*!
// \file         MouseHoverDarkenScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains struct declaration of the mouse hover script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _MOUSEHOVERDARKENSCRIPT_H_
#define _MOUSEHOVERDARKENSCRIPT_H_

struct scriptBase;
struct MouseHoverDarkScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "MouseHoverDarkScript" }; //name identifier
	int testCounter = 0;

	float initialScaleX;//stores the initial scale x
	float initialScaleY;//stores the initial scale y 
	const float offSetScale{ 20.f };
	float scaledUpScaleX;//stores the scaled up scale x 
	float scaledUpScaleY;//stores the scaled up scale y

	//functions
	MouseHoverDarkScript(const char* scriptName = "MouseHoverDarkScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 