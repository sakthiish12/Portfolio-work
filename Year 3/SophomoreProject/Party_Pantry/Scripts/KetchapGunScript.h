// /******************************************************************************/
// /*!
// \file         KetchapGunScript.h
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        KetchapGunScript that 
// \lines        14 Lines 
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _KETCHAPGUNSCRIP_H_
#define _KETCHAPGUNSCRIP_H_

struct scriptBase;
struct KetchapGunScript : public scriptBase
{
	float radius = 0.5f;
	//functions
	KetchapGunScript(const char* scriptName = "KetchapGunScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 