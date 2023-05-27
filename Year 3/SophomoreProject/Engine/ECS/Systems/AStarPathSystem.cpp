// /******************************************************************************/
// /*!
// \file         AStarPathSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the A* pathfinding system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "AStarPathSystem.h"//astar 
#include <vector>//vector 
#include <functional>
#include <queue>
#include <iostream>

std::shared_ptr<AStarPathFindingSys> AStarPathFindingSysPtr;//declare 

/**********************PATH FINDING SYSTEM*********************/
AStarPathFindingSys::AStarPathFindingSys(bool requireComponentMatch) {
	UNREFERENCED_PARAMETER(requireComponentMatch);
}

void AStarPathFindingSys::update(float dt) {
	static_cast<void>(dt);//unreferenced variable 
	for (auto const& e : getEntitiesRegistered()) {
		auto& oEntityType = ecs.getComponent<Com_type>(e);//getting the entity type 
		auto& oFindPath = ecs.getComponent<Com_FindPath>(e);//getting the entity type 
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting the transform of the entity 

		//if the entity is a player 
		if (oEntityType.type == Com_type::player) {
			oFindPath.targetPos.x = oTransform.pos.x;//update the target position 
			oFindPath.targetPos.y = oTransform.pos.y;//update the target position 
			oFindPath.targetPosInitialized = true;
		}

		//if the entity type is not player and target position is initialized 
		if (oEntityType.type != Com_type::player && oFindPath.targetPosInitialized == true ) {
			findPath({ oTransform.pos.x , oTransform.pos.y }, { oFindPath.targetPos.x  ,oFindPath.targetPos.y});//find path from enemy to player 
		}
	}

}

void AStarPathFindingSys::updateNodes() {
	//set the unwalkable nodes 
	for (auto const& eInner : getEntitiesRegistered()) {
		auto& oEntityTypeInner = ecs.getComponent<Com_type>(eInner);//getting the entity type 
		//auto& oFindPathInner = ecs.getComponent<Com_FindPath>(eInner);//getting the entity type 
		auto& oTransformInner = ecs.getComponent<transform2D>(eInner);//getting the transform of the entity 

		//getting the grid point 
		Mathf::iVector2 gridPoint = PathFindingGrid::GetInstance().gridPointFromWorldPoint(oTransformInner.pos);

		//make the surround unwalkable
		if (oEntityTypeInner.type == Com_type::player || oEntityTypeInner.type == Com_type::enemy || oEntityTypeInner.type == Com_type::enemyrange) {
			//make the current node unwalkable 
			//PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x, gridPoint.y,false);
		}
		if (oEntityTypeInner.type == Com_type::obstacles) {
			//make the surround 8 nodes unwalkable & current node 
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x, gridPoint.y, false); //current
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x - 1, gridPoint.y - 1, false); //bottom left
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x, gridPoint.y - 1, false); //bottom middle
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x + 1, gridPoint.y - 1, false); //bottom right 
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x + 1, gridPoint.y, false); //right
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x - 1, gridPoint.y, false); //left
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x - 1, gridPoint.y + 1, false); //top left
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x, gridPoint.y + 1, false); //top middle
			PathFindingGrid::GetInstance().setNodeWalkable(gridPoint.x + 1, gridPoint.y + 1, false); //top right 
		}
	}
}

int AStarPathFindingSys::getDistance(const PathFindingNode& nodeA, const PathFindingNode& nodeB) {
	//getting the distances 
	int distanceX = abs(nodeA.mGridX - nodeB.mGridX);
	int distanceY = abs(nodeA.mGridY - nodeB.mGridY);

	//if distance x bigger than distance y 
	if (distanceX > distanceY) {
		return (DiagonalDistance * distanceY) + (HorizontalDistance * (distanceX - distanceY));//getting the distance 
	}
	else {
		return (DiagonalDistance * distanceX) + (HorizontalDistance * (distanceY - distanceX));//getting the distance 
	}
}

void AStarPathFindingSys::retracePath(PathFindingNode* startingNode, PathFindingNode* endingNode) {
	std::list<PathFindingNode*> path;//path for the nodes
	PathFindingNode* currentNode = endingNode;//set current node 

	//retracing the path till we reached the starting node 
	while (currentNode != startingNode) {
		path.push_back(currentNode);//add the current node to the path 
		currentNode = currentNode->parentNode;//set the current node to the parent node to retrace 
	}
	path.reverse();//reverse the list 

	PathFindingGrid::GetInstance().pathListVec.emplace_back(path);//push back the path list into a vector of path list 
	PathFindingGrid::GetInstance().pathListSet = true;//set path true 
}


void AStarPathFindingSys::findPath(Mathf::Vector2 startPos, Mathf::Vector2 endPos) {
	PathFindingNode* startNode = mGrid->nodeFromWorldPoint(startPos);//getting the starting node 
	PathFindingNode* endNode = mGrid->nodeFromWorldPoint(endPos);//getting the end node 

	std::list<PathFindingNode*> openSet;//create a new list of nodes
	std::unordered_set<PathFindingNode*> closedSet; //create a new hash set of closed nodes 

	openSet.emplace_back(startNode);//emplace back the start node 

	while (openSet.size() > 0) {
		std::list<PathFindingNode*>::iterator currentNode = openSet.begin();//set to the first node
		//while it is not the end of the list 
		for (std::list<PathFindingNode*>::iterator node = openSet.begin(); node != openSet.end(); ++node) {
			//if this node is smaller than the current node f cost or .. 
			if ((*(*node)).fCost() < (*(*currentNode)).fCost() || (*(*node)).fCost() == (*(*currentNode)).fCost() && (*(*node)).hCost < (*(*currentNode)).hCost) {
				currentNode = node;//set the current node 
			}
		}

		PathFindingNode* nodePtr = *currentNode;//set the node ptr 
		openSet.erase(currentNode); //remove that node from the openset 
		closedSet.emplace(nodePtr);//emplace current node to closed set 

		//if end/target node is found 
		if (nodePtr == endNode) {
			retracePath(startNode, endNode);
			return;
		}

		std::list<PathFindingNode*> listOfNeighbours = PathFindingGrid::GetInstance().getNeighbours(*nodePtr);//getting the current node's neighbours 

		//iterate through the list of neighbours 
		for (PathFindingNode* NeighbourNode : listOfNeighbours) {
			if (NeighbourNode->walkable == false || closedSet.count(NeighbourNode)) {
				continue;//loop again 
			}

			int newMovementCostToNeighbour = (nodePtr->gCost + getDistance(*nodePtr, *NeighbourNode));//get new movement cost to neighbour by getting the distance between current node and neighbour node 

			//comparing the new movecost to the neighbour or if the open set does not contain the neighbour node 
			if (newMovementCostToNeighbour < NeighbourNode->gCost || (std::find(openSet.begin(), openSet.end(), NeighbourNode) == openSet.end())) {
				NeighbourNode->gCost = newMovementCostToNeighbour;//set the new gcost
				NeighbourNode->hCost = getDistance(*NeighbourNode, *endNode);//get the h cost 
				NeighbourNode->parentNode = nodePtr;//set the parent node 

				//if the open set does not contain the neighbour node 
				if ((std::find(openSet.begin(), openSet.end(), NeighbourNode) == openSet.end())) {
					openSet.push_back(NeighbourNode);//add the neighbour node to the open set 
				}
			}
		}
	}
}