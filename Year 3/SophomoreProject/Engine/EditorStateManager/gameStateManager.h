// /******************************************************************************/
// /*!
// \file         gameStateManager.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Declaration of editor state manager functions
// \lines        19 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//TODO Documentation

#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H
#pragma once
#include "EditorStates/gameState.h"

//Use a namespace instead: https://stackoverflow.com/a/112451
//

namespace gameStateManager
{
	//Game State Manager Loop
	//This loop is vital to the game because it runs for as long as the game
	//runs. Before entering, initialise vital systems for game. After exiting,
	//free these vital systems before finishing the main() function and
	//terminating the game.
	void run(gameState* firstState);

	//Set the flag to restart the current game state
	void restartState();

	//Set the flag to halt running of the game state manager and quit the game.
	void quitGame();

	//Set the next game state to be executed
	//This will tell the game state manager to
	//halt execution of the current game state and prepare
	//execution of the next one.
	void setNextState(gameState* nextState);

	//PHYSICS TIMESTEP CONTROL
	//Set whether to use fixed or semi-fixed physics timestep.
	void setPhysicsTimeStepFixedness(const bool newFixedness);

	//Set new value of minimum timestep. Should be positive.
	//Do not set this value too high or else the simulation may become
	//inaccurate due to lack of resolution.
	void setMinPhysicsTimeStep(const float newMin);

	//Set new value of maximum timestep.
	//Should be positive and more than minimum timestep.
	//Do not set this value too low or else the simulation may not be
	//able to catch up (time to simulate one physics frame is greater than
	//maximum physics time step)
	void setMaxPhysicsTimeStep(const float newMax);

	//Set new value of fixed timestep.
	//Note that this only changes the timestep resolution
	//and not the actual speed of simulation.
	void setFixedPhysicsTimeStep(const float newStep);

	extern int currentNumberOfSteps;
	extern bool usingFixedDTTimestep;
	extern double fixedDeltaTime;
}

//Singleton classes should not be used
//http://gameprogrammingpatterns.com/singleton.html

/*class gameStateManager
{
public:
	static gameStateManager& getInstance();

	//Set the next state of the game state manager
	void setNextState(gameState* nextState);
	
	//Game State Manager handling game states as appropriate.
	//To only be called once in the main function.
	void run();

	void restartState();

	void quit();

	gameState* getCurrentState() const;
	gameState* getPreviousState() const;
	gameState* getNextState() const;

private:
	//Private constructor due to gameStateManager being a singleton
	gameStateManager();

	//Delete copy/move constructors so extra instances cannot be created.
	//Singletons should not be cloneable
	gameStateManager(const gameStateManager&) = delete;
	gameStateManager(gameStateManager&&) = delete;

	//Singletons should not be assignable
	gameStateManager& operator=(const gameStateManager&) = delete;
	gameStateManager& operator=(gameStateManager&&) = delete;

	//Singleton GSM
	static gameStateManager* GSM;

	//Indicators of game states to handle
	//TODO should they be of this type or be ints instead?
	static gameState* current, * previous, * next;

	//Indicator of whether to quit game
	static bool toQuit;

	//Indicator of whether to restart current state
	static bool toRestart;
};*/

#endif
