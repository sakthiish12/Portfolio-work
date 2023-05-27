// /******************************************************************************/
// /*!
// \file         SoundsScript.cpp
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
#ifndef _SOUNDS_SCRIPT_H_
#define _SOUNDS_SCRIPT_H_



struct scriptBase;
struct SoundScript : scriptBase
{
	//CPP Scripting
	//base data
	//std::string name{"TestingScriptCPP"}; //name identifier
	int testCounter = 0;
	//functions
	SoundScript(const char* scriptName = "SoundScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 