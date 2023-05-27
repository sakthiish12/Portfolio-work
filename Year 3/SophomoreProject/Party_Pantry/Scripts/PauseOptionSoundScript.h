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
#ifndef _PAUSEOPTIONSOUNDSCRIPT_H_
#define _PAUSEOPTIONSOUNDSCRIPT_H_

struct scriptBase;
struct PauseOptionSoundScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseOptionSoundScript" }; //name identifier

	entityID yesButtonID;//entity id to track the yes button 
	entityID noButtonID;//entity id to track the no button 
	entityID backgroundID;//entity id to track the no button 
	entityID playerID;//entity id of the player 
	bool isLoaded = false;
	entityID taskTextID;//tracks the ID of the tasktext
	bool isEntityLoaded = false;//tracks if the entities are loaded
	bool isEntityShowing = false;//tracks if the entities are being shown 
	bool BGMplaying = true;


	//functions
	PauseOptionSoundScript(const char* scriptName = "PauseOptionSoundScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 