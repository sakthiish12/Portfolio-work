// /******************************************************************************/
// /*!
// \file         BlenderTextCountScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
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
#ifndef _BLENDERTEXTSCRIPT_H_
#define _BLENDERTEXTSCRIPT_H_

struct scriptBase;
struct BlenderTextScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BlenderTextScript" }; //name identifier
	int testCounter = 0;

	int startingCount;

	//functions
	BlenderTextScript(const char* scriptName = "BlenderTextScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 