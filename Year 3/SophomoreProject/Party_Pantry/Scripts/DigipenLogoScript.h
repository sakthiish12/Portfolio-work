// /******************************************************************************/
// /*!
// \file         .cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         10th Apr, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _DIGIPENLOGOSCRIPT_H_
#define _DIGIPENLOGOSCRIPT_H_

struct scriptBase;
struct DigipenLogoScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "DigipenLogoScript" }; //name identifier
	bool active = true;//indicates if it's active 
	bool fadeInOut = true; //true fade in , false fade out 

	const float speedOfTransition = 0.35f;//the speed of the transition

	//functions
	DigipenLogoScript(const char* scriptName = "DigipenLogoScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 