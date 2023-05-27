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
#ifndef _THRONEROOMTELEPORTERSCRIPT_H_
#define _THRONEROOMTELEPORTERSCRIPT_H_

struct scriptBase;
struct ThroneRoomTeleporterScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "ThroneRoomTeleporterScript" }; //name identifier
	const float radiusOfDetection = 100.f;//setting the radius of detection for the proximity script 
	const float radiusTeleport = 100.f;//setting the radius of the teleportation 

	Mathf::vec2 emitterVelocity = { 0.f,700.f };//velocity of the emitter
	Mathf::vec3 emitterPos;//emitter's position 
	const float emitterRangeX = 50.f;
	const float emitterRangeY = 120.f;

	//functions
	ThroneRoomTeleporterScript(const char* scriptName = "ThroneRoomTeleporterScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 