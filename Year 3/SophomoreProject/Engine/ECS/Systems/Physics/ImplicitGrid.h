// /******************************************************************************/
// /*!
// \file         ImplicitGrid.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         12th Nov, 2021
// \brief        Broadphase check for physic collision
// \lines        34 Lines
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _IMPLICITGRID_H_
#define _IMPLICITGRID_H_

#include <bitset>
#include <array>
#include <vector>

#include <vector>
#include "../../component.h"
#include "../../ECS.h"

constexpr unsigned int MAX_OBJECT = 1500;
constexpr unsigned int GRID_ROW = 20;
constexpr unsigned int GRID_COL = 20;

struct collidablePairID
{
    unsigned int first;
    unsigned int second;
    float penetration = 0.f;
    vec2 collisionNormal{ 0.f, 0.f }; // from 'first' point of view
    bool static1 = true;
    bool static2 = true;
    bool hasRB1 = false;
    bool hasRB2 = false;
    bool collided = false;

    //for polygon rotation
    //fixed rotation, -1 rotate to left, 1 rotate to right, 0 dont rotate
    //condition for rotate, if mid of collider velocity dir is outside of the collided area, rotate
    short rotate1 = 0;
    short rotate2 = 0;

    collidablePairID(int E1, int E2) : first{ static_cast<unsigned int>(E1) }, second{ static_cast<unsigned int>(E2) } {}
    collidablePairID(unsigned int E1, unsigned int E2) : first{ E1 }, second{ E2 } {}
};

//using collidablePairID = std::pair<unsigned int, unsigned int>;

struct ImplicitGrid
{
    float WORLD_X_MAX = 10000;
    float WORLD_X_MIN = -10000;
    float WORLD_Y_MAX = 10000;
    float WORLD_Y_MIN = -10000;

    float SIZE_X = (WORLD_X_MAX - WORLD_X_MIN);
    float SIZE_Y = (WORLD_Y_MAX - WORLD_Y_MIN);
    float CELLSIZE_X = SIZE_X / GRID_COL;
    float CELLSIZE_Y = SIZE_Y / GRID_ROW;

    bool printDebugInfoToConsole = false;

    //maybe can skip saving this two? straight to AND result
    std::array<std::bitset<MAX_OBJECT>, GRID_ROW> rowArr{};
    std::array<std::bitset<MAX_OBJECT>, GRID_COL> colArr{};
    //may be collidablePair can be a vec or list ?
    std::array< std::array<std::bitset<MAX_OBJECT>, GRID_COL>, GRID_ROW> cells{};
    std::vector<collidablePairID> collidablePairsIDContainer;
   // std::vector<collidablePairID> collidedPairsIDContainer;

    //maybe a fn to link to player?

    //call this if world max and min is changed.
    void updateBorderChanges();

    ImplicitGrid();
    //void initData();
    void setupGrid(const std::vector<entityID>& entityList); // set up bits in rowArr and colArr
    void checkCollidables(); // check the cells 
    std::array< std::array<std::bitset<MAX_OBJECT>, GRID_COL>, GRID_ROW>& getCellArray();
    
    void drawGridLine();
};



#endif
