// /******************************************************************************/
// /*!
// \file         MeleeSpatula.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         10th Apr, 2022
// \brief        This file contains the declarations for melee spatula's logic
// \lines        15
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef MELEESPATULA_H
#define MELEESPATULA_H

struct scriptBase;
struct MeleeSpatula : scriptBase
{
	//Varaibles
	int damage = 1;
	float stunMultiplier = 1.0f;

	MeleeSpatula(const char* scriptName = "MeleeSpatula");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif