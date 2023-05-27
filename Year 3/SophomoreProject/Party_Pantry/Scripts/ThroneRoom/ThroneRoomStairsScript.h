// /******************************************************************************/
// /*!
// \file         PauseOptionGameScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         18th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _THRONEROOMSTAIRSSCRIPT_H_
#define _THRONEROOMSTAIRSSCRIPT_H_

struct scriptBase;
struct ThroneRoomStairsScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "ThroneRoomStairsScript" }; //name identifier
	const float radiusOfDetection = 100.f;//setting the radius of detection for the proximity script 
	const float radiusTeleport = 100.f;//setting the radius of the teleportation 
	const float triggerPosY = -340.729f;
	const float triggerTextPosY = -200.f;
	bool textLoaded = false;
	bool pulsatingFadeIn = false;
	entityID textId;//loading of the text

	//functions
	ThroneRoomStairsScript(const char* scriptName = "ThroneRoomStairsScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 