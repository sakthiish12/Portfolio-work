// /******************************************************************************/
// /*!
// \file         TaskListScript.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains  the struct declaration of the task list script 
// \lines        21
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _LEVELTWOTASKLISTSCRIPT_H_
#define _LEVELTWOTASKLISTSCRIPT_H_

struct scriptBase;
struct LevelTwoTasklistScript : scriptBase
{
	//CPP TaskListScript
	//base data
	int numOfTomatoesToCapture = 5;
	const int totalTaskToCapture = 5;//the number of task to capture
	int tomatoesCaptured = 0;//number of tomatoes captured
	int prevNumTomatoesCaptured = 0;//keeps track of the previous number of tomatoes captured
	std::string Text{ "Find the Cave " };//contains the text 
	std::string Text1{ "Find The King " };//contains the text 
	std::string Text2{ "Absorb 3 Tomatoes " };//contains the text 
	std::string Text3{ "Absorb 6 Chillies " };//contains the text 
	std::string Text4{ "Defeat King Cordyceps " };//contains the text 
	std::string Text5{ "Find your way!" };//contains the text 
	

	std::string name{ "LevelTwoTasklistScript" }; //name identifier

	//the starting scale 
	float startingScaleX;
	float startingScaleY;
	float startingPosX;
	float startingPosY;

	vec2 anchorTopRight;//stores the anchor top right 

	//the opening scale 
	float openingScaleX;
	float openingScaleY;
	float openingPosX;
	float openingPosY;

	//indicates if the text has been loaded 
	bool isLoaded = false;
	bool taskDone1 = false;//tracks if the task is done 
	entityID taskTextID;//tracks the ID of the tasktext
	entityID playerEntityID;//tracks the ID of the player 

	//functions
	LevelTwoTasklistScript(const char* scriptName = "LevelTwoTasklistScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 