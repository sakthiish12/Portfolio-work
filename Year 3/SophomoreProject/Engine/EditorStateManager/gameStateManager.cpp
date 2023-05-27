// /******************************************************************************/
// /*!
// \file         gameStateManager.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Definition of editor state manager functions
// \lines        107 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//TODO Documentation
//Game state manager

#include <chrono> //Frame time
#include "gameStateManager.h"
#include "../StopWatches/StopWatch.h"
#include "../StopWatches/EngineTime.h"
namespace gameStateManager
{
	//Utilisation of anonymous namespace within namespace
	//to hide externally-linked information that cannot be seen from outside.
	namespace
	{
		//TODO change these states as needed.
		gameState* previousState = nullptr;
		gameState* currentState = nullptr;
		gameState* nextState = nullptr;

		//Whether the flag has been raised for the game to be quit.
		bool toQuit = false;

		//Whether the flag has been raised for the state to be reset.
		bool toRestart = false;

		//Whether the physics timestep is fixed or not
		//If not fixed, follows a semi-variable timestep,
		//where physics dt varies by frame length, but may be clamped.
		//If fixed, lets the rendering be an interpolation between
		//two consecutive physics steps.
		//Rendering time step will always be varied.
		//More information here
		//https://gafferongames.com/post/fix_your_timestep/
		//bool usingFixedDTTimestep = false;

		//Minimum length of a physics time step, should be positive.
		//Will be adhered to if usingFixedDTTimestep is set to false.
		float minimumPhysicsTimestep = 0.0000000001f;

		//Maximum length of a physics time step, should be positive and
		//significantly greater than the minimum time step.
		//Will be adhered to if usingFixedDTTimestep is set to false.
		float maximumPhysicsTimestep = 1.0f;

		//Length of a fixed physics step. Should be positive.
		//Will be adhered to if usingFixedDTTimestep is set to true.
		float fixedPhysicsTimestep = 0.01f;

		double accumulatedTime = 0.0;
	}
	//maybe can be a non const variable if needed for player to set frame rate as option
	double fixedDeltaTime = 1 / 60.0;
	
	//initialised as true as will be used more preferbly than false
	bool usingFixedDTTimestep = true; 
	//For physics to access
	int currentNumberOfSteps = 0;
	

	//Game State Manager Loop
	//This loop is vital to the game because it runs for as long as the game
	//runs. Before entering, initialise vital systems for game. After exiting,
	//free these vital systems before finishing the main() function and
	//terminating the game.
	void run(gameState* firstState)
	{

		// Delta Time Tracker
		StopWatch stopWatch;
		stopWatch.StartTimer();
		//Set quit and restart flags to false
		toQuit = false;
		toRestart = false;

		//Set the first state to run
		previousState = firstState;
		currentState = firstState;
		nextState = firstState;

		while (!toQuit)
		{
			if (toRestart)
			{
				//Restart game states
				currentState = previousState;
				nextState = previousState;
				toRestart = false;
			}
			else
			{
				//Moving on to a new game state
				//Call load function of currentState game state
				currentState->load();
			}
			//Call init function of currentState state
			currentState->init();

			float frameTime = fixedPhysicsTimestep; // Have an initial value

			//Start state loop
			while (currentState == nextState && !toQuit && !toRestart)
			{
				//Use of new C++ timing functions:
				//https://en.cppreference.com/w/cpp/chrono
				std::chrono::duration<float> deltaTime;
				auto startTime = std::chrono::high_resolution_clock::now();
				if (usingFixedDTTimestep)
				{
					//TODO Recheck if it could be done. Keep in mind that in order to pull this off, the physics component of needs previous timestep data too.
					//Fixed physics timestep.
					//Varying render step renders interpolation
					//between two physics steps.

					currentNumberOfSteps = 0;
					accumulatedTime += frameTime;//adding the actual game loop time
					while (accumulatedTime >= fixedDeltaTime)
					{
						accumulatedTime -= fixedDeltaTime;//this will save the exact accumulated time differences, among all game loops
						++currentNumberOfSteps;
					}
				}
				//else
				{
					//Handle inputs from player.
					currentState->input(frameTime);

					//Update currentState game state's physics and other data.
					currentState->update(frameTime);

					//Draw and render currentState game state events.
					currentState->render(frameTime);
					stopWatch.LapTimer(); // Lap Timer
					Time::UpdateTime(stopWatch); // Update Time Framework

					//Semi-fixed physics timestep.
					//Clamped between two extreme values.
					//Same timing as render step.
					//dt value that should be adjusted.
					auto endTime = std::chrono::high_resolution_clock::now();
					deltaTime = endTime - startTime;
					startTime = std::chrono::high_resolution_clock::now();
					frameTime = deltaTime.count();

					//Prevent negative dts or division by zero.
					if (frameTime <= minimumPhysicsTimestep)
						frameTime = minimumPhysicsTimestep;

					//Cap out dt at this value.
					if (frameTime > maximumPhysicsTimestep)
						frameTime = maximumPhysicsTimestep;
				}
			}

			//Free once out of state loop
			currentState->free();

			//Check if not to restart state.
			//If so, unload game state.
			if (!toRestart)
				currentState->unload();

			//Shift forward game states
			//previousState game state takes currentState game state
			//currentState game state takes nextState game state
			previousState = currentState;
			currentState = nextState;
		}
	}

	//Set the flag to restart the current game state
	void restartState()
	{
		toRestart = true;
	}

	//Set the flag to halt running of the game state manager and quit the game.
	void quitGame()
	{
		toQuit = true;
	}

	//Set the next game state to be executed
	//This will tell the game state manager to
	//halt execution of the current game state and prepare
	//execution of the next one.
	void setNextState(gameState* _nextState)
	{
		nextState = _nextState;
	}

	void setPhysicsTimeStepFixedness(const bool newFixedness)
	{
		usingFixedDTTimestep = newFixedness;
	}

	void setMinPhysicsTimeStep(const float newMin)
	{
		//Only accept positive inputs.
		if (newMin > 0.0f)
			minimumPhysicsTimestep = newMin;
	}

	void setMaxPhysicsTimeStep(const float newMax)
	{
		//Only accept inputs greater than minimum time step
		if (newMax > minimumPhysicsTimestep)
			maximumPhysicsTimestep = newMax;
	}

	void setFixedPhysicsTimeStep(const float newStep)
	{
		//Only accept positive inputs.
		if (newStep > 0.0f)
			fixedPhysicsTimestep = newStep;
	}
}

//Singleton classes should not be used
//http://gameprogrammingpatterns.com/singleton.html