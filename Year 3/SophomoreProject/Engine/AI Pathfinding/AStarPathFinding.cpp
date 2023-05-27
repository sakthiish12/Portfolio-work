// /******************************************************************************/
// /*!
// \file         AStarPathFinding.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the A* pathfinding 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "AStartPathFinding.h"//astar 
#include "../Serializer/Settings/AStarGridMapSerializer.h"//A* pathfinding grid map 
#include "../Serializer/SerializeManager.h"//serializer manager 
#include "../AssetManager/AssetManager.h"//asset manager 


AStarGridMapSettings AStarGridMap;
SerializeManager serializeManagerAStar;


PathFindingGrid::PathFindingGrid() {
	serializeManagerAStar.DeserializeFromFile("Config/AStarGridMapSettings.json", AStarGridMap);
	nodeRadius = AStarGridMap.m_Radius;//setting the node radius 
	gridWorldSize = AStarGridMap.m_Scale;//setting the world size 
	initGridMap(AStarGridMap.m_Scale, AStarGridMap.m_Radius);//init  the grid map	
	gridWorldStartingPos = AStarGridMap.m_GridPos;//starting position of the grid map 
}


void PathFindingGrid::initGridMap(const Mathf::Vector2 worldsize, const float radius) {
	gridWorldSize = worldsize;//getting the world size 
	nodeRadius = radius;//getting the radius

	nodeDiameter = nodeRadius * 2;
	gridmapSizeX = static_cast<int>(round(gridWorldSize.x / nodeDiameter));//get how many nodes we can fit in the world size x, round to nearest integer 
	gridmapSizeY = static_cast<int>(round(gridWorldSize.y / nodeDiameter));//get how many nodes we can fit in the world size y, round to nearest integer 
	createGridMap();//creating the grid map 
}

//creating the grid 
void PathFindingGrid::createGridMap() {
	this->nodeGrid = new PathFindingNode * [gridmapSizeY];//allocate size for node grid 
	for (int i = 0; i < gridmapSizeY; ++i) {
		this->nodeGrid[i] = new PathFindingNode[gridmapSizeX];//allocate size for node grid 
	}

	//init all the nodes in the grid 
	for (int i{ 0 }; i < gridmapSizeY; ++i) {
		for (int j{ 0 }; j < gridmapSizeX; ++j) {
			this->nodeGrid[i][j] = PathFindingNode(true, { gridWorldStartingPos.x + (j * nodeDiameter) + nodeRadius, gridWorldStartingPos.y + (i * nodeDiameter) + nodeRadius }, i, j);//init the nodes
		}
	}

}


void PathFindingGrid::drawGridMap() {

	float gridWorldEndPosX = gridWorldStartingPos.x + (gridmapSizeX * nodeDiameter);//getting the grid end pos 
	float gridWorldEndPosY = gridWorldStartingPos.y + (gridmapSizeY * nodeDiameter);//getting the grid end pos 

	//draw starting boundary 
	BatchRenderer2D::GetInstance().AddRotateLine(gridWorldStartingPos.x, gridWorldStartingPos.y, gridWorldEndPosX, gridWorldStartingPos.y);//bottom line 
	BatchRenderer2D::GetInstance().AddRotateLine(gridWorldEndPosX, gridWorldStartingPos.y, gridWorldEndPosX, gridWorldEndPosY);//right line 
	BatchRenderer2D::GetInstance().AddRotateLine(gridWorldEndPosX, gridWorldEndPosY, gridWorldStartingPos.x, gridWorldEndPosY);//top line 
	BatchRenderer2D::GetInstance().AddRotateLine(gridWorldStartingPos.x, gridWorldStartingPos.y, gridWorldStartingPos.y, gridWorldEndPosY);//left line 

	//drawing horizontal lines
	for (size_t i{ 0 }; i < gridmapSizeY; ++i) {
		BatchRenderer2D::GetInstance().AddRotateLine(gridWorldStartingPos.x, gridWorldStartingPos.y + (i * nodeDiameter), gridWorldEndPosX, gridWorldStartingPos.y + (i * nodeDiameter), 0.f, 0.f, 0.f);//bottom line 
	}

	//drawing vertical lines
	for (size_t i{ 0 }; i < gridmapSizeX; ++i) {
		BatchRenderer2D::GetInstance().AddRotateLine(gridWorldStartingPos.x + (i * nodeDiameter), gridWorldStartingPos.y, gridWorldStartingPos.x + (i * nodeDiameter), gridWorldEndPosY, 0.f, 0.f, 0.f);//left line 
	}

	//drawing for the path 
	for (std::list<PathFindingNode*> pathList : pathListVec) {
		if (pathList.size() != 0) {
			std::list<PathFindingNode*>::iterator pathListIter = pathList.begin();//setting iterator 
			bool breakx = false, breaky = false;
			//iterate through the entire path 
			while (pathListIter != pathList.end()) {
				breakx = false, breaky = false;//reset 
				for (size_t i{ 0 }; i < gridmapSizeY && breaky == false; ++i) {
					for (size_t j{ 0 }; j < gridmapSizeX && breakx == false; ++j) {
						//if it's the path, highlight in black 
						if ((*pathListIter)->mGridX == j && (*pathListIter)->mGridY == i) {
							//BatchRenderer2D::GetInstance().AddQuad(gridWorldStartingPos.x + (j * nodeDiameter), gridWorldStartingPos.y + (i * nodeDiameter), 1.f, 0.f, nodeDiameter, nodeDiameter, { 0.f,0.f,0.f,1.f });//draw green square
							pathListIter++;//increment the path list 
							breakx = true; breaky = true;//break out of for loop
						}
					}
				}
			}
		}
	}

	//drawing of the indicator 
	for (size_t i{ 0 }; i < gridmapSizeY; ++i) {
		for (size_t j{ 0 }; j < gridmapSizeX; ++j) {
			//if it's walkable, draw this 
			if (nodeGrid[j][i].walkable == true) {
				//BatchRenderer2D::GetInstance().AddQuad(gridWorldStartingPos.x + (j * nodeDiameter), gridWorldStartingPos.y + (i * nodeDiameter), 1.f, 0.f, nodeDiameter, nodeDiameter, { 0.f,1.f,0.f,1.f });//draw green square
			}
			else {
				//BatchRenderer2D::GetInstance().AddQuad(gridWorldStartingPos.x + (j * nodeDiameter), gridWorldStartingPos.y + (i * nodeDiameter), 1.f, 1.0f, nodeDiameter, nodeDiameter, {1.f,0.f,0.f,1.f});//draw red square 
			}
		}
	}


}

PathFindingNode* PathFindingGrid::nodeFromWorldPoint(const Mathf::Vector2& worldPosition) {
	float percentX = (worldPosition.x) / gridWorldSize.x;//getting the percentage of x 
	float percentY = (worldPosition.y) / gridWorldSize.y;//getting the percentage of y
	percentX = Mathf::Clamp(percentX, 0.f, 1.f);//clamp it between 0 and 1 so as to not go out of range 
	percentY = Mathf::Clamp(percentY, 0.f, 1.f);//clamp it between 0 and 1 so as not to go out of range 

	int x = static_cast<int>(round((gridmapSizeX - 1) * percentX));//getting the x coord
	int y = static_cast<int>(round((gridmapSizeY - 1) * percentY));//getting the y coord

	//setNodeWalkable(x, y, false);//test

	return &nodeGrid[x][y];//return the node 

}


Mathf::iVector2 PathFindingGrid::gridPointFromWorldPoint(const Mathf::Vector2& worldPosition) {

	float percentX = (worldPosition.x) / gridWorldSize.x;//getting the percentage of x 
	float percentY = (worldPosition.y) / gridWorldSize.y;//getting the percentage of y
	percentX = Mathf::Clamp(percentX, 0.f, 1.f);//clamp it between 0 and 1 so as to not go out of range 
	percentY = Mathf::Clamp(percentY, 0.f, 1.f);//clamp it between 0 and 1 so as not to go out of range 

	int x = static_cast<int>(round((gridmapSizeX - 1) * percentX));//getting the x coord
	int y = static_cast<int>(round((gridmapSizeY - 1) * percentY));//getting the y coord

	return { x,y };
}


PathFindingNode* PathFindingGrid::getNode(const int& x, const int& y) {
	return &nodeGrid[x][y];//returns the node pointer 
}

void PathFindingGrid::resetGridMap() {
	//reset every node
	for (size_t i{ 0 }; i < gridmapSizeY; ++i) {
		for (size_t j{ 0 }; j < gridmapSizeX; ++j) {
			nodeGrid[i][j].walkable = true;//reset walkable 
			nodeGrid[i][j].hCost = 0;//reset walkable 
			nodeGrid[i][j].gCost = 0;//reset walkable 

		}
	}

	PathFindingGrid::GetInstance().pathListVec.clear();//clearing the vector filled with list of path
	PathFindingGrid::GetInstance().pathListSet = false;//reset 
}

void PathFindingGrid::setNodeWalkable(const int& xIndex, const int& yIndex, bool walkableToggle) {
	if (xIndex >= 0 && yIndex >= 0 && xIndex <= gridmapSizeX && yIndex <= gridmapSizeY) {
		nodeGrid[xIndex][yIndex].walkable = walkableToggle;//set the node 
	}
}

std::list<PathFindingNode*> PathFindingGrid::getNeighbours(PathFindingNode& currentNode) {
	std::list<PathFindingNode*> neighbours; //list of neighbours 

	//search the 3 by 3 surrounding of the node 
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) {
				continue;//continue search
			}

			int checkX = currentNode.mGridX + x;
			int checkY = currentNode.mGridY + y;

			if (checkX >= 0 && checkX < gridmapSizeX && checkY >= 0 && checkY < gridmapSizeY) {
				neighbours.push_back(getNode(checkX, checkY));//push back node into the neighbouring node 
			}
		}
	}

	return neighbours;//returns the list of neighbours 
}

PathFindingGrid::~PathFindingGrid() {

	for (int i = 0; i < gridmapSizeY; ++i) {
		delete[] nodeGrid[i];//delete the nodes in the columns 
	}

	//delete the nodes in the rows 
	delete[] nodeGrid;//
}
