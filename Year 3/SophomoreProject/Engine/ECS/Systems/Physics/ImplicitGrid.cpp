// /******************************************************************************/
// /*!
// \file         ImplicitGrid.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         12th Nov, 2021
// \brief        Broadphase check for physic collision
// \lines        137 Lines
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "ImplicitGrid.h"
#include <memory>

#include "../../Graphics/Source/BatchRenderer2D.h"
/*
The cell should be big enough, if not there maybe multiple repeated narrow check on the same pair.



*/
#undef min
#undef max
#include <limits>

void ImplicitGrid::updateBorderChanges()
{
    SIZE_X = (WORLD_X_MAX - WORLD_X_MIN);
    SIZE_Y = (WORLD_Y_MAX - WORLD_Y_MIN);
    CELLSIZE_X = SIZE_X / GRID_COL;
    CELLSIZE_Y = SIZE_Y / GRID_ROW;
}

ImplicitGrid::ImplicitGrid()
{
    memset(&rowArr, 0, GRID_ROW);
    memset(&colArr, 0, GRID_COL);
    memset(&cells, 0, GRID_ROW * GRID_COL);
}


void ImplicitGrid::setupGrid(const std::vector<entityID>& entityList)
{
    //for each object, turn on the relevant array index with its bitset
    for (auto const& e : entityList)
    {
        AABB* aabb = nullptr;
        Poly* poly = nullptr;
        int minXObj = std::numeric_limits<int>::max();
        int maxXObj = std::numeric_limits<int>::min();
        int minYObj = minXObj;
        int maxYObj = maxXObj;
        if (ecs.entityHas<AABB>(e))
        {
            aabb = &ecs.getComponent<AABB>(e);
            minXObj = static_cast<int>(aabb->min.x);
            maxXObj = static_cast<int>(aabb->max.x);
            minYObj = static_cast<int>(aabb->min.y);
            maxYObj = static_cast<int>(aabb->max.y);
        }
        else if (ecs.entityHas<Poly>(e))
        {
            poly = &ecs.getComponent<Poly>(e);

            for (vec2 pt : poly->points)
            {
                if (pt.x > maxXObj)
                    maxXObj = static_cast<int>(pt.x);
                if (pt.x < minXObj)
                    minXObj = static_cast<int>(pt.x);
                if (pt.y > maxYObj)
                    maxYObj = static_cast<int>(pt.y);
                if (pt.y < minYObj)
                    minYObj = static_cast<int>(pt.y);
            }
        }
        else
        {
            //no colliders found
            //skip
            continue;
        }

        int minX = static_cast<int>((minXObj - WORLD_X_MIN) / CELLSIZE_X);
        int maxX = static_cast<int>((maxXObj - WORLD_X_MIN) / CELLSIZE_X);
        int minY = static_cast<int>((minYObj - WORLD_Y_MIN) / CELLSIZE_Y);
        int maxY = static_cast<int>((maxYObj - WORLD_Y_MIN) / CELLSIZE_Y);
        //possibility of access oob.
        //otherwise should be correct
        //This could be costly
        if (minX < 0)
            minX = 0;
        if (minY < 0)
            minY = 0;
        if (maxX < 0)
            maxX = 0;
        if (maxY < 0)
            maxY = 0;
        if (minX > GRID_COL - 1)
            minX = GRID_COL - 1;
        if (minY > GRID_ROW - 1)
            minY = GRID_ROW - 1;
        if (maxX > GRID_COL - 1)
            maxX = GRID_COL - 1;
        if (maxY > GRID_ROW - 1)
            maxY = GRID_ROW - 1;
        do {
            rowArr[minY].set(e);
            //if (e == 1)
            //    std::cout << "Entity : " << e << " is in Ycell " << minX << std::endl;

            //Leaving it here for debugging purposes
            if (minY > maxY)
            {
                //ISSUE HERE: SHOULDN'T come here
                //std::cout << "Issue in implicit grid " << minX << std::endl;
                break;
            }
        } while (minY++ != maxY);

        do {
            colArr[minX].set(e);
            //if (e == 1)
            //    std::cout << "Entity : " << e << " is in Xcell " << minX << std::endl;
            if (minX > maxX)
            {
                //ISSUE HERE: SHOULDN'T come here
                //std::cout << "Issue in implicit grid " << minX << std::endl;
                break;
            }
        } while (minX++ != maxX);
        
    }
}

void ImplicitGrid::checkCollidables()
{
    collidablePairsIDContainer.clear();
    //std::cout << "clear " << std::endl;
    //y
    for (int i = 0; i < GRID_ROW; ++i)
    {
        //x
        for (int j = 0; j < GRID_COL; ++j)
        {
            //AND result are the pairs/obj/grp to check
            //cells[y][x]
            cells[i][j] = rowArr[i] & colArr[j] ;//Probably need confirm if this is working correctly, maybe via diff size of x and y
            //if a cell has more than 1 object, send their id to go for narrowphase
            /*if (cells[i][j][1])
                std::cout << "player is in " << j << " , " << i << std::endl;*/
            if (cells[i][j].count() > 1)
            {
                //Looping thorugh the bit to gather collidables
                auto bit = cells[i][j];
                int size = (int)cells[i][j].size();
                for (int k = 0; k < size - 1; ++k)
                {
                    if (bit[k] == 1)
                    {
                        for (int l = k + 1; l < size; ++l)
                        {
                            if (bit[l] == 1)
                            {
                                collidablePairsIDContainer.push_back({ k, l });
                                /*if (printDebugInfoToConsole)
                                    std::cout << "colliding pairs ID: " << k << " , " << l << std::endl;*/
                            }
                        }
                    }
                }
            }
            cells[i][j].reset();
        }
        rowArr[i].reset();
    }
    //deleting here coz data is still needed by the inner loop above
    for (int j = 0; j < GRID_COL; ++j)
    {
        colArr[j].reset();
    }
    //drawGridLine();
}

std::array<std::array<std::bitset<MAX_OBJECT>, GRID_COL>, GRID_ROW>& ImplicitGrid::getCellArray()
{
    return cells;
}

void ImplicitGrid::drawGridLine()
{
    for (int i = 0; i < GRID_ROW; ++i)
    {
        float y = WORLD_Y_MIN + CELLSIZE_Y * i;
        BatchRenderer2D::GetInstance().AddRotateLine(WORLD_X_MIN, y, WORLD_X_MAX, y, 0.2f, 0.0f, 0.0f);
    }

    for (int j = 0; j < GRID_COL; ++j)
    {
        float x = WORLD_X_MIN + CELLSIZE_X * j;
        BatchRenderer2D::GetInstance().AddRotateLine(x, WORLD_Y_MIN, x, WORLD_Y_MAX, 0.2f, 0.0f, 0.0f);
    }

}
