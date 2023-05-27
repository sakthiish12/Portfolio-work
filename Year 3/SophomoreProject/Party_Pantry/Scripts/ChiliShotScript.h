// /******************************************************************************/
// /*!
// \file         ChiliShotScript.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         6th Dec, 2021
// \brief        This file contains the declarations for enemy chili shot's logic
// \lines        13 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef CHILISHOTSCRIPT_H
#define CHILISHOTSCRIPT_H

struct scriptBase;
struct ChiliShotScript : scriptBase
{
	ChiliShotScript(const char* scriptName = "ChiliShotScript");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif