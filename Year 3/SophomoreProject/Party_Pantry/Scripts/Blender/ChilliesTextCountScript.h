// /******************************************************************************/
// /*!
// \file         ChilliesTextCountScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         16th March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains struct declaration of the blender text count script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _CHILLIESBLENDERTEXTSCRIPT_H_
#define _CHILLIESBLENDERTEXTSCRIPT_H_

struct scriptBase;
struct ChilliesBlenderTextScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "ChilliesBlenderTextScript" }; //name identifier

	int startingCount;

	//functions
	ChilliesBlenderTextScript(const char* scriptName = "ChilliesBlenderTextScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 