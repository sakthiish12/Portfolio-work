// /******************************************************************************/
// /*!
// \file         Tornado.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         9th Apr, 2022
// \brief        This file contains the declarations for the tornado's logic
// \lines        21
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef TORNADO_H
#define TORNADO_H

struct scriptBase;
struct Tornado : scriptBase
{
	int damage{ 7 };
	float stunMultiplier{ 1.0f };

	float remainingLifetime{ 2.0f };
	const float shotDelay{ 0.1f };
	float timeTillNextShot{ 0.0f };
	
	Tornado(const char* scriptName = "Tornado");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:
	const float flipInterval{ 0.05f };
	float timeTillNextFlip{ 0.00f };
};

#endif