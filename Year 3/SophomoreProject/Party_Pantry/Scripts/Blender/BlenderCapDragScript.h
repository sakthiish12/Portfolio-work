// /******************************************************************************/
// /*!
// \file         BlenderCapDragScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the struct declaration for the blender cap drag script 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BLENDERCAPDRAGSCRIPT_H_
#define _BLENDERCAPDRAGSCRIPT_H_

struct scriptBase;
struct BlenderCapDragScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "BlenderCapDragScript" }; //name identifier
	int testCounter = 0;
	bool withinRange = false;//tracks if the blender cap is within the range 
	//capped position 
	const float cappedPosX = 8.741f;
	const float cappedPosY = 282.371f;
	const float cappedRadiusOffset = 30.f;

	//functions
	BlenderCapDragScript(const char* scriptName = "BlenderCapDragScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 