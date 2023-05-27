// /******************************************************************************/
// /*!
// \file         BlenderProximityScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration of the blender proximity script
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BLENDERPROXIMITYSCRIPT_H_
#define _BLENDERPROXIMITYSCRIPT_H_

struct scriptBase;
struct BlenderProximityScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BlenderProximityScript" }; //name identifier
	
	bool collected = false; //indicates if the blender has already been collected
	bool pulsatingFadeIn = false;//tracks the pulsating 

	const float radiusOfDetection = 100.f;//setting the radius of detection for the proximity script 
	bool textLoaded = false;//indicates if the text is already loaded 
	entityID textId;//stores the text id 
	entityID playerID;//stores the player id 

	//functions
	BlenderProximityScript(const char* scriptName = "BlenderProximityScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 