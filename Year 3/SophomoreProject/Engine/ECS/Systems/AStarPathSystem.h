// /******************************************************************************/
// /*!
// \file         AStarPathSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declaration for the A* pathfinding system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef ASTARPATH_SYS
#define ASTARPATH_SYS

#include "../system.h"
#include "../../AI Pathfinding/AStartPathFinding.h"//astar path finding 
#include "../../ECS/ECS.h"//ecs 
#include <list>//list
#include <unordered_set>//hash set 
#include <algorithm>//algorithm
#include <math.h>//math h 

#define HorizontalDistance 10
#define DiagonalDistance 14

//try to have entity 0 as a player controlled obj
class AStarPathFindingSys : public systemBase
{

public:

	AStarPathFindingSys(bool requireComponentMatch = false);
	~AStarPathFindingSys() {}

	//delete fncs
	AStarPathFindingSys(AStarPathFindingSys& rhs) = delete;
	AStarPathFindingSys(const AStarPathFindingSys& rhs) = delete;
	AStarPathFindingSys& operator=(const AStarPathFindingSys& rhs) = delete;
	AStarPathFindingSys& operator=(const AStarPathFindingSys& rhs) const = delete;

	//init 
	void init() {
		mInitialized = true;//set to initialized 
		mGrid = &PathFindingGrid::GetInstance();//getting the grid 
	}

	//update 
	void update(float dt);

	//updates the nodes 
	void updateNodes();

	//getting the distance between node a and node b 
	int getDistance(const PathFindingNode& nodeA, const PathFindingNode& nodeB);

	//retracing the path from 
	void retracePath(PathFindingNode* startingNode, PathFindingNode* endingNode);


	//finds the shortest path from start pos to end pos 
	void findPath(Mathf::Vector2 startPos, Mathf::Vector2 endPos);


private:
	bool mInitialized{ false };//if the grid is intialized 
	PathFindingGrid* mGrid;//pathfinding grid 
};

extern std::shared_ptr<AStarPathFindingSys> AStarPathFindingSysPtr;

#endif //ASTARPATH_SYS