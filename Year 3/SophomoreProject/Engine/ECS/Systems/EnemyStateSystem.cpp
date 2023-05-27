// /******************************************************************************/
// /*!
// \file         EnemyStateSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "EnemyStateSystem.h"//enemy state 
#include "../../AI Pathfinding/AStartPathFinding.h"//A* 


std::shared_ptr<EnemyStateSys> EnemyStateSysPtr;//externs
#define enemyMovementSpeed 0.5f

//ecs fnc 
EnemyStateSys::EnemyStateSys(bool requireComponentMatch) {
	static_cast<void>(requireComponentMatch);//unreferenced param 

}

void EnemyStateSys::ChangeState(Com_EnemyStates::states newState) {
	UNREFERENCED_PARAMETER(newState);
	//Com_EnemyStateOne& state = get<Com_EnemyStateOne>();
	//// Exit current state
	//(this->*_fp_states[static_cast<int>(state._current_state) * 3 + 2])();
	//// Enter new state
	//(this->*_fp_states[static_cast<int>(newState) * 3])();
	//state._current_state = newState;


	
}

//init fnc 
void EnemyStateSys::init() {

}

void EnemyStateSys::update(float dt)
{
	if (gameStateManager::usingFixedDTTimestep)
	{
		dt = static_cast<float>(gameStateManager::fixedDeltaTime);
		//Loop used in systems that have time-based formula
		for (int step = 0; step < gameStateManager::currentNumberOfSteps; ++step)
		{
			updateSteps(dt);
		}
	}
	else
	{
		updateSteps(dt);
	}
}

//update fnc 
void EnemyStateSys::updateSteps(float dt) {
	static_cast<void>(dt);//unreferenced variable 

	size_t iterator = 0;//set the iterator 
	for (auto const& e : getEntitiesRegistered()) {
		auto& oEntityType = ecs.getComponent<Com_type>(e);//getting the entity type 
		auto& oEnemyState = ecs.getComponent<Com_EnemyStates>(e);//getting the enemy states
		auto& oHealth = ecs.getComponent<Com_Health>(e);//getting the health of the entity 
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting the transform of the entity 
		auto& oFindPath = ecs.getComponent<Com_FindPath>(e);//getting the find path
		//auto& oSprite = ecs.getComponent<spriteCom>(e);//getting the sprite com

		(void)oEntityType;
		if (oHealth.health <= 0) {
			return;//it's dead 
		}

		//PathFindingGrid::GetInstance().pathListVec;//getting the path list vector 

		//(this->*_fp_states[static_cast<int>(oEnemyState._current_state) * 2 + 1])();//running the state 
		(this->*_fp_states[static_cast<int>(oEnemyState._current_state) + 1])();//running the state 

		//TO DO : port this over to it's own state function 
		Mathf::iVector2 enemyGridPos = PathFindingGrid::GetInstance().gridPointFromWorldPoint({oTransform.pos.x,oTransform.pos.y});
		Mathf::iVector2 playerGridPos = PathFindingGrid::GetInstance().gridPointFromWorldPoint({oFindPath.targetPos.x, oFindPath.targetPos.y});


		//if it has not reached yet
		if (PathFindingGrid::GetInstance().pathListSet == true) {
			if ((PathFindingGrid::GetInstance().pathListVec[iterator]).size() > 1) {
				//look at the node list and see where to move 
				std::list<PathFindingNode*>::iterator listIterator = (PathFindingGrid::GetInstance().pathListVec[iterator]).begin();//get the iterator 
				listIterator++;//point to the next 
				int parentNodePosX = (*(listIterator))->parentNode->mGridX;//getting the grid pos x of parent node 
				int parentNodePosY = (*(listIterator))->parentNode->mGridY;//getting the grid pos y of parent node 

				//look at the parent node and see where to move, TO DO: abstract this move function
				//bool animationSet = false;

				//move left 
				if (enemyGridPos.x - 1 == parentNodePosX && enemyGridPos.y == parentNodePosY) {
					oTransform.pos.x-=enemyMovementSpeed;//move left 
				}
				//move right 
				if (enemyGridPos.x + 1 == parentNodePosX && enemyGridPos.y == parentNodePosY) {
					oTransform.pos.x += enemyMovementSpeed;//move right 
				}
				//move up 
				if (enemyGridPos.x == parentNodePosX && enemyGridPos.y + 1 == parentNodePosY) {
					oTransform.pos.y += enemyMovementSpeed;//move up 
				}
				//move down 
				if (enemyGridPos.x == parentNodePosX && enemyGridPos.y - 1 == parentNodePosY) {
					oTransform.pos.y -= enemyMovementSpeed;//move down 
				}

				//diagonal movement 
				//bottom left 
				if (enemyGridPos.x - 1 == parentNodePosX && enemyGridPos.y - 1 == parentNodePosY) {
					oTransform.pos.x -= enemyMovementSpeed;//move left 
					oTransform.pos.y -= enemyMovementSpeed;//move down 
				}
				//bottom right 
				if (enemyGridPos.x + 1 == parentNodePosX && enemyGridPos.y - 1 == parentNodePosY) {
					oTransform.pos.x += enemyMovementSpeed;//move right 
					oTransform.pos.y -= enemyMovementSpeed;//move down 
				}
				//top left 
				if (enemyGridPos.x - 1 == parentNodePosX && enemyGridPos.y + 1 == parentNodePosY) {
					oTransform.pos.x -= enemyMovementSpeed;//move left 
					oTransform.pos.y += enemyMovementSpeed;//move up 
				}
				//top right 
				if (enemyGridPos.x + 1 == parentNodePosX && enemyGridPos.y + 1 == parentNodePosY) {
					oTransform.pos.x += enemyMovementSpeed;//move right 
					oTransform.pos.y += enemyMovementSpeed;//move up 
				}
			}
			iterator++;//increment the iterator 
		}
	}
}

//state idle enter 
void EnemyStateSys::IDLE_ENTER() {
	//int test = 0;
}

//state idle update 
void EnemyStateSys::IDLE_UPDATE() {
	//int test = 0;
	//if not reached, set enemy state to move update 
	//auto& oEntityType = ecs.getComponent<Com_type>(e);//getting the entity type 
}

//state idle exit 
void EnemyStateSys::IDLE_EXIT() {
	//int test = 0;
}

//state move enter 
void EnemyStateSys::MOVE_ENTER() {
	//int test = 0;
}

//state move update 
void EnemyStateSys::MOVE_UPDATE() {
	//int test = 0;
}

//state move exit 
void EnemyStateSys::MOVE_EXIT() {
	//int test = 0;
}

//state attack enter 
void EnemyStateSys::ATTACK_ENTER() {
	//int test = 0;
}

//state attack update 
void EnemyStateSys::ATTACK_UPDATE() {
	//int test = 0;
}

//state attack exit 
void EnemyStateSys::ATTACK_EXIT() {
	//int test = 0;
}