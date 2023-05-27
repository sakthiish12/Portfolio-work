// /******************************************************************************/
// /*!
// \file         ThroneDialogueScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _THRONEDIALOGUESCRIPT_H_
#define _THRONEDIALOGUESCRIPT_H_

struct scriptBase;
struct ThroneDialogueScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "ThroneDialogueScript" }; //name identifier

	entityID playerID;//tracks the player's id 
	entityID textID;//tracks the text id 
	entityID currentDialogueID;//tracls the current dialogue id 

	bool activeDialogue = false;

	const float posYOffset = 220.f;//offset for the dialogue texture
	const float proximityRadius = 200.f;//radius to toggle 
	const float proximityRadiusX = 50.f;//radius to toggle 
	bool isTextLoaded = false;//tracks if the dialogue is loaded 

	//dialogue tracking 
	size_t currentDialogueCount = 1;//starting dialouge count 

	//functions
	ThroneDialogueScript(const char* scriptName = "ThroneDialogueScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 