// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        133
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _TELEPORTERSCRIPT_H_
#define _TELEPORTERSCRIPT_H_

struct scriptBase;
struct TeleportScript : scriptBase 
{
	//CPP TaskListScript
	//base data
	std::string name{ "TeleportScript" }; //name identifier
	bool bossDead = false;
	const float radiusOfDetection = 100.f;//setting the radius of detection for the proximity script 
	const float radiusTeleport = 100.f;//setting the radius of the teleportation 
	bool textLoaded = false;//indicates if the text is already loaded 
	entityID textId;//stores the text id 
	entityID playerID;//stores the player id 

	const float textOffSetY = 0.f;//off set for text 

	Mathf::vec2 emitterVelocity = { 0.f,700.f };//velocity of the emitter
	Mathf::vec3 emitterPos;//emitter's position 
	const float emitterRangeX = 50.f;
	const float emitterRangeY = 120.f;

	//functions
	TeleportScript(const char* scriptName = "TeleportScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif