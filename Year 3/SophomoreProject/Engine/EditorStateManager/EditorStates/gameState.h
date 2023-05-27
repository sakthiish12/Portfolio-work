// /******************************************************************************/
// /*!
// \file         gameState.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Declarations of editor states for editor state manager
// \lines        99 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Declaration of base editor state object which dictates
//editor flow loop control.
//Also lists declarations of editor's states.

#ifndef GAMESTATE_H
#define GAMESTATE_H
#pragma once
#include "../../StopWatches/CategoryStopWatch.h"
struct SceneMetrics
{
    CategoryStopWatch systemExecutionTimings;
};

//Game State Superclass. Not to be directly used.
class gameState
{
public:
    //START FUNCTIONS
    //Called upon state entry

    //Loads the state's necessary resource data (such as audio, textures, etc.)
    //and initializes it.
    //Do take note that reading from the hard drive for loading relevant assets
    //will be FAR slower than loading from RAM. It is for that reason that
    //the load() should not be called upon state restarting.
    //Called upon loading up the game state after unloading a different state.
    virtual void load() = 0;

    //Prepares the state's entities (game objects) in order to be
    //used for the first time. Should not load any resource data.
    //Called either:
    //After loading the game state OR
    //After restarting the game state
    virtual void init() = 0;

    //LOOP FUNCTIONS
    //These three functions should be in a constant loop until a call is made
    //to restart the state, unload it, or quit the game.
    //These functions should be called in order of input > update > render

    //Read inputs from the player in here.
    //Update the input system accordingly. If input is:
    //a boolean state (mouse clicks, button presses, mouse wheel scrolls)
    // 	   Note present state (on or off)
    // 	   Note state in previous frame (on or off, note difference if any)
    //a variable state (mouse position)
    //	   Note present state (current position)
    //	   Note state in previous frame (position, note difference if any)
    virtual void input(float dt) = 0;

    //Update both physics and collision. It is best to do them simultaneously
    //as both of them usually go hand in hand.
    //A method to do this is to
    //1)    Predict where objects will be at the next frame
    //      should their travel path be uninterrupted by any collision.
    //2)    Evaluate if collisions happened among collidable objects along
    //      their respective paths.
    //3)    Evaluate collision responses if any.
    //4)    Modify and finalise object physics (pos, vel and accel) for the
    //      next frame.
    //Physics calculation can be done by integration methods.
    //The most simple useful method of integration is by
    //semi-implicit euler.
    //i.e., Given a = f/m,
    //v += a * dt
    //s += v * dt, in this order.
    //More integration methods here:
    //https://gafferongames.com/post/integration_basics/
    //https://en.wikipedia.org/wiki/Symplectic_integrator
    //https://en.wikipedia.org/wiki/Hamiltonian_system
    virtual void update(float dt) = 0;

    //Render relevant objects here by calling graphics functions.
    //Practice culling methods to prevent unneccessary rendering that will
    //slow the game down.
    virtual void render(float dt) = 0;

    //END FUNCTIONS
    //Called upon state exit

    //Cleans up the state's object instances. May clean or save scoring system.
    //Makes the state ready to be unloaded or initialized again.
    //No data should be dumped in this cycle function.
    //Whatever was initialised in init() should be uninitialised in free().
    virtual void free() = 0;

    //Called when the state should be terminated
    //(transitioning from one game state to another or quiting the game).
    //Dumps all data back that was loaded in the load cycle function.
    //Cleans assets(free / release from memory).
    //Whatever was loaded in load() should be unloaded in unload().
    virtual void unload() = 0;
};

//Was used for testing the ECS and State Manager. Presently unused
class testState : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern testState g_testState;

class renderTestState : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern renderTestState g_renderTestState;

class testStateTransition : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern testStateTransition g_testStateTransition;


class physicsTestState : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern physicsTestState g_physicTestState;

class imguiTestState : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
    // Performance Metrics
    CategoryStopWatch m_CategoryStopWatch, m_PreviousCategoryStopWatch;

    SceneMetrics GetMetrics();
};
extern imguiTestState g_imguiTestState;

class officialTest : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern officialTest g_officialTest;


class SerializeOfficial : public gameState
{
public:
    void load();
    void init();
    void input(float dt);
    void update(float dt);
    void render(float dt);
    void free();
    void unload();
};
extern SerializeOfficial g_SerializeOfficial;

#endif //GAMESTATE_H
