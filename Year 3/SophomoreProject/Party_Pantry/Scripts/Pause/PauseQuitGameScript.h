// /******************************************************************************/
// /*!
// \file         PauseQuitGameScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration pause quit game script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PAUSEQUITGAMESCRIPT_H_
#define _PAUSEQUITGAMESCRIPT_H_

struct scriptBase;
struct PauseQuitGameScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "PauseQuitGameScript" }; //name identifier

	entityID yesButtonID;//entity id to track the yes button 
	entityID noButtonID;//entity id to track the no button 
	entityID backgroundID;//entity id to track the no button 
	entityID playerID;//entity id of the player 


	bool isEntityLoaded = false;//tracks if the entities are loaded
	bool isEntityShowing = false;//tracks if the entities are being shown 



	//functions
	PauseQuitGameScript(const char* scriptName = "PauseQuitGameScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 