// /******************************************************************************/
// /*!
// \file         AStarPathFinding.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration for the A* pathfinding 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef ASTAR
#define ASTAR

#include "../Math/Vector/Vector2.h"//vec2 
#include "../Math/MathFunctions/Mathf.h"//functions 

#include <vector>//vector 
#include <functional>
#include <queue>
#include <iostream>
#include "../Graphics/Source/BatchRenderer2D.h"//batch rendering 
#include <list>//list 


/******************************************************
* @ Brief : TEST (Wilfred)
*******************************************************/

struct PathFindingNode {

	bool walkable; //true if node is walkable 
	Mathf::Vector2 worldPosition;//world position of the node 
	int mGridX;//keeps track of it's own grid position
	int mGridY;//keeps track of it's own grid position

	PathFindingNode* parentNode; //points to the parent node 

	int gCost; //distance between the current node and the start node.
	int hCost; //estimated distance from the current node to the end node.

	PathFindingNode() { } //default constructor 
	PathFindingNode(bool _walkable, const Mathf::Vector2& _worldPos, int& gridx, int& gridy) : walkable(_walkable), worldPosition(_worldPos), mGridX(gridx), mGridY(gridy) { }//init node 

	//calculat fcost - total cost of the node.
	int fCost() {
		return gCost + hCost;//calculate the fcost of the particular node 
	}
};

class PathFindingGrid {

private:
	Mathf::Vector2 gridWorldSize; //world size of the entire grid 
	Mathf::Vector2 gridWorldStartingPos{ 0.f,0.f };//starting world position of the grid 
	float nodeRadius; //how much each individual node covers 
	bool unwalkableMask; //indicates if this grid node is walkable or unwalkable

	PathFindingNode** nodeGrid;//2d array of nodes 
	float nodeDiameter; //the diameter of the nodes 
	int gridmapSizeX, gridmapSizeY;//grid map size 
public:
	std::vector<std::list<PathFindingNode*>> pathListVec;//contains the list of path *testing*
	bool pathListSet;//indicates if the path list has been set 
	bool debugState;//checks if it's debug state and draw grid lines

public:
	//get instance 
	static PathFindingGrid& GetInstance() { static PathFindingGrid instance; return instance; }//accessor instance 

	PathFindingGrid();

	//start method 
	void initGridMap(const Mathf::Vector2 worldsize, const float radius = 20.f);

	//creating the grid 
	void createGridMap();

	//this function draws the grid map 
	void drawGridMap();

	PathFindingNode* nodeFromWorldPoint(const Mathf::Vector2& worldPosition);

	//getting the grid points from world point 
	Mathf::iVector2 gridPointFromWorldPoint(const Mathf::Vector2& worldPosition);

	//setting the particular node whether it is walkable 
	void setNodeWalkable(const int& xIndex, const int& yIndex, bool walkableToggle);

	//reset the entire nodes of the grid map 
	void resetGridMap();

	//getting the particular node at the grid position 
	PathFindingNode* getNode(const int& x, const int& y);


	std::list<PathFindingNode*> getNeighbours(PathFindingNode& currentNode);


	//destructor 
	~PathFindingGrid();
};

/******************************************************
* @ Brief : TEST (Wilfred)
*******************************************************/
#endif // !ASTAR
