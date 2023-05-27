// /******************************************************************************/
// /*!
// \file         BlenderOutputScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration of the blender output script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BLENDEROUTPUTSCRIPT_H_
#define _BLENDEROUTPUTSCRIPT_H_

struct scriptBase;
struct BlenderOutputScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BlenderOutputScript" }; //name identifier

	//functions
	BlenderOutputScript(const char* scriptName = "BlenderOutputScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 