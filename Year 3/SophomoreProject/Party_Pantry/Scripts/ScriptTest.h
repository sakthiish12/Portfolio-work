// /******************************************************************************/
// /*!
// \file         ScriptTest.h
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         8th Oct, 2021
// \brief        ScriptTest to show how scripting works
// \lines        14 (epis)
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _SCRIPT_TEST_H_
#define _SCRIPT_TEST_H_

struct scriptBase;
struct TestingScriptCPP : scriptBase
{
	//CPP Scripting
	//base data
	//std::string name{"TestingScriptCPP"}; //name identifier
	int testCounter = 0;
	//functions
	TestingScriptCPP(const char* scriptName = "TestingScriptCPP");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 