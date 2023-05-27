/*********************************************************************************
Pathfind.c
Author: Xie Hao Zhe (Astar pathfinding)
Code Lines: 44 (9 + 5 + 7 + 23)
Author: Chong Jun Yi (Resolve bugs from release mode - NODE linked list was broken during recursive function)
Code Lines: Approx 10 lines
Brief: Algorithm for Astar pathfinding which the AI guards will call upon
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/

#include <stdio.h> /*NULL*/
#include <Windows.h>
#include "Pathfind.h"
#include "Guard.h"
#include <math.h> /*Round*/
#include "../Map/Map.h"
#include "../Console/Console.h"
#include "../Global/Global.h" /*Debug Mode*/

/*Set up the A* algorithms node lists*/
struct NODE openList[HEIGHT][WIDTH] = { 0 };
struct NODE closedList[HEIGHT][WIDTH] = { 0 };
struct NODE pathList[HEIGHT][WIDTH] = { 0 };

struct NODE startNode = { 0 };
struct NODE goalNode = { 0 };
struct NODE newNode = { 0 };
struct NODE lastClosed = { 0 };

/*Declaring and initializing lengths for Node arrays*/
int openLength = 0;
int closedLength = 0;
int pathLength = 0;

int timeToCheck = 0;

/*******************************************************************************
							NODE FUNCTIONS
*******************************************************************************/

/*Initialises the A* algorithms node lists*/
void InitNodeList() {
	InitNodes(openList);
	InitNodes(closedList);
	InitNodes(pathList);
}

/*Initialises all nodes to default values*/
void InitNodes(struct NODE list[][WIDTH]) 
{
	int h, w;
	for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			ClearListNode(list, h, w);
			list[h][w].x = w;
			list[h][w].y = h;
		}
	}
}

/*Reset Nodes value*/
void ClearNode(struct NODE* node) 
{
	node->parent = NULL;
	node->exists = 0;
	node->id = -1;
	node->f = -1;
	node->g = -1;
	node->h = -1;
}

/*Reset Nodes value*/
void ClearListNode(struct NODE list[][WIDTH], int row, int col) 
{
	list[row][col].parent = NULL;
	list[row][col].exists = 0;
	list[row][col].id = -1;
	list[row][col].f = -1;
	list[row][col].g = -1;
	list[row][col].h = -1;
}

/*Finds node with specific id number index and returns it*/
void GetNode(struct NODE list[][WIDTH], int findIndex, struct NODE *output) 
{
	int h = 0, w = 0;

	for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			if (list[h][w].id == findIndex) {
				*output = list[h][w];
				(*output).parent = list[h][w].parent;
				/*DebugMode_Pathfind("WUUUUU", list[h][w].x);
				DebugMode_Pathfind("WUUUUU Parent", list[h][w].parent->x);
				DebugMode_Pathfind("WUUUUU Parent 2", list[h][w].parent->parent->x);
				DebugMode_Pathfind("WUUUUU Parent 3", list[h][w].parent->parent->parent->x);*/
				return;
			}
		}
	}
	*output = (struct NODE){ 0 };
	output->exists = 0;	output->id = -1;
}

/*Copies data from parent into parent of Node copy*/
void SetParentForNode(struct NODE *copy, struct NODE *parent) {
	copy->parent = parent;
	copy->parent->exists = parent->exists;
	copy->parent->x = parent->x;
	copy->parent->y = parent->y;
	copy->parent->g = parent->g;
	copy->parent->h = parent->h;
	copy->parent->f = parent->f;
	copy->parent->id = parent->id;
}

/*Adds a node to a list by saving it's id and index in the list*/
void AddListNode(struct NODE list[][WIDTH], int *length, struct NODE *node) {

	if (node->parent != NULL) {
		list[node->y][node->x].parent = node->parent; 
		list[node->y][node->x].parent->exists = node->parent->exists;
		list[node->y][node->x].parent->x = node->parent->x;
		list[node->y][node->x].parent->y = node->parent->y;
		list[node->y][node->x].parent->g = node->parent->g;
		list[node->y][node->x].parent->h = node->parent->h;
		list[node->y][node->x].parent->f = node->parent->f;
	}
	else {
		list[node->y][node->x].parent = NULL;
	}
	list[node->y][node->x].exists = 1;
	list[node->y][node->x].x = node->x;
	list[node->y][node->x].y = node->y;
	list[node->y][node->x].g = node->g;
	list[node->y][node->x].h = node->h;
	list[node->y][node->x].f = node->f;

	/*Saves id as a representative of index in list*/
	list[node->y][node->x].id = *length;
	++(*length);
}

/*Removes a node from a list*/
void RemoveListNode(struct NODE list[][WIDTH], int* length, const struct NODE *node) {
	int h, w;
	/*If node exists*/
	if (node->exists == 1) 
	{
		/*If node is not the last in list*/
		if (node->id != (*length) - 1) 
		{
			/*Iterates through every node in list*/
			for (h = 0; h < HEIGHT; h++) 
			{
				for (w = 0; w < WIDTH; w++) 
				{
					/*If node has id greater than one being removed*/
					if (list[h][w].id > node->id) 
					{
						/*Decrement id*/
						list[h][w].id--;
					}
				}
			}
		}
		/*Clear values in the node*/
		ClearListNode(list, node->y, node->x);

		// Reset parent ... can remove
		//list[node.y][node.x].parent = NULL;
		
		/*Decrements length*/
		--(*length);
	}
}
/*Arranges original nodes with a valid id number
 from low to high, into 1D array to be sorted*/
void ArrangeID(struct NODE *arrange, struct NODE original[][WIDTH], int length) 
{
	int a = 0, h = 0, w = 0;
	int bmoveToNextIndex = 0;

	for (a = 0; a < length; ++a)
	{
		bmoveToNextIndex = 0;

		for (h = 0; h < HEIGHT; ++h)
		{
			for (w = 0; w < WIDTH; ++w) 
			{
				/*If node id matches the search criteria*/
				if (original[h][w].id == a) 
				{
					/*Saves node in according to index of node id*/
					*(arrange + a) = original[h][w];
					(arrange + a)->parent = original[h][w].parent;
					
					if (goalNode.x == 35 && goalNode.y == 13) {
						/*if (original[h][w].x == 35 && original[h][w].y == 13) {
							DebugMode_General("THE INDEX", a);
							DebugMode_General("THE LENGTH", length);
							DebugMode_General("??????????_Y", (arrange + a)->x);
							DebugMode_General("??????????_Y", (arrange + a)->y);
							DebugMode_General("??????????_X", (arrange + a)->parent->x);
							DebugMode_General("??????????_Y", (arrange + a)->parent->y);
							timeToCheck = 1;
						}*/
					}

					bmoveToNextIndex = 1;
				}
				if (bmoveToNextIndex) 
					break;	
			}
			if (bmoveToNextIndex) 
				break;		
		}
	}
}

/*Reverses Node id's in a grid*/
void ReverseID(struct NODE path[][WIDTH], int path_Length) {
	/*s starts at 0, e starts at last index in path*/
	int s, e = path_Length - 1;
	struct NODE start, end;

	/*s increments while e decrements*/
	for (s = 0; s < e; ++s) {
		/*Gets node at id s and e*/
		GetNode(path, s, &start);
		GetNode(path, e, &end);

		/*Swaps id's of nodes start and end*/
		path[start.y][start.x].id = e;
		path[end.y][end.x].id = s;
		e--;
	}
}
/*Calculates mathematical values on a single open node*/
void CalculateNodeValues(struct NODE parentNode, struct NODE goal, int possibleX, int possibleY, int* open_Length)
{
	/*Declaring and initalizing variables used for calculations*/
	int gCost = 0;
	int hCost = 0;
	int fCost = 0;
	int hxDiff = 0;
	int hyDiff = 0;

	/*Creates a new open node (to be looked at)*/
	ClearNode(&newNode);

	/*Sets coordinates of the open node*/
	newNode.x = possibleX;
	newNode.y = possibleY;
	
	//if (parentNode.exists == 1) 
	// Sets the parent for the open node
	SetParentForNode(&newNode, &parentNode);
	

	/*Calculate G cost*/
	/*Cost of moving from parent node to one open node*/
	gCost = newNode.parent->g + 1;
	newNode.g = gCost;

	/*Calculate H cost*/
	/*Estimated cost to move from the open node to the goal node*/
	hxDiff = abs(goal.x - newNode.x);
	hyDiff = abs(goal.y - newNode.y);
	hCost = hxDiff + hyDiff;
	newNode.h = hCost;

	/*Calculate F cost (combined G and H)*/
	fCost = gCost + hCost;
	newNode.f = fCost;

	/*Add the open node to the open list*/
	AddListNode(openList, open_Length, &newNode);


}

/*This is the start function for every new pathfinding*/
void StartPathfinding(struct GUARD *guard, int curLvl)
{
	
	/*Initialise Node arrays*/
	InitNodeList();
	
	/*Sets the nodes of start and goal, in this case AI position to last seen player position*/
	startNode.x = guard->pos.X;
	startNode.y = guard->pos.Y;
	goalNode.x = guard->detectedLoc.X;
	goalNode.y = guard->detectedLoc.Y;

	/*Add start node to the closed list*/
	AddListNode(closedList, &closedLength, &startNode);

	FindPath(startNode, goalNode, startNode, (struct NODE){0}, goalNode.x, goalNode.y, curLvl);

	/*Pathfinding Done*/
	InformPath(guard);
	openLength = 0;
	closedLength = 0;
	pathLength = 0;
}

/*Main recursive function to find open nodes, new parent, and repeat*/
/*Note that ParentPNode is required as release mode does not store the node linked list parent properly (not null too)*/
void FindPath(Node start_Node, Node goal_Node, Node parentNode, Node parentPNode, int endX, int endY, int curLvl)
{
	int i;
	int possibleX = 0, possibleY = 0;
	/*if (goal_Node.x == 35 && goal_Node.y == 13) {
		if (timeToCheck) {
			DebugMode_General("F!#@$", openList[13][35].x);
			DebugMode_General("F!#@$", openList[13][35].y);
			DebugMode_General("F!#@$", openList[13][35].parent->x);
			DebugMode_General("F!#@$", openList[13][35].parent->y);
			DebugMode_General("F!#@$", parentNode.x);
			DebugMode_General("F!#@$", parentNode.y);
			DebugMode_General("F!#@$", parentNode.parent->x);
			DebugMode_General("F!#@$", parentNode.parent->y);
		}
	}*/
	/*if (goal_Node.x == 35 && goal_Node.y == 13) {
		if (openList[13][36].exists) {
			DebugMode_General("1111111111_X", openList[13][36].x);
			DebugMode_General("1111111111_Y", openList[13][36].y);
			if (openList[13][36].parent != NULL) {
				DebugMode_General("2222222222_X", openList[13][36].parent->x);
				DebugMode_General("2222222222_Y", openList[13][36].parent->y);
			}

		}
	}*/


	/*Checks all 4 directions to add to open List*/
	for (i = 0; i < 4; i++) {
		/*Uses cosine and sine functions to get circle of points (at 90 degree intervals) around parent*/
		possibleX = (int)(parentNode.x + round(-1 * cos(PI_2 * i)));
		possibleY = (int)(parentNode.y + round(-1 * sin(PI_2 * i)));

		/*If coordinates are outside of boundaries*/
		if (possibleX < 0 || possibleY < 0 || possibleX >(WIDTH - 1) || possibleY >(HEIGHT - 1))
		{
			continue;
		}

		/*Does not make a new open node IF
		 - Node is a wall
		 - Node is already in the open list
		 - Or the node is already in the closed list*/
		if (Map_CheckCollision(possibleX, possibleY, curLvl, 1) != 0 || closedList[possibleY][possibleX].exists == 1 || openList[possibleY][possibleX].exists == 1)
		{
			continue;
		}

		/*Create an open node, calculate the values and add it to the open list*/
		CalculateNodeValues(parentNode, goal_Node, possibleX, possibleY, &openLength);
		/*if (goal_Node.x == 35 && goal_Node.y == 13) {
			if (possibleX == 35 && possibleY == 13) {
				DebugMode_General("||||||||_X", openList[13][35].x);
				DebugMode_General("||||||||_Y", openList[13][35].y);
				DebugMode_General("||||||||_X", openList[13][35].parent->x);
				DebugMode_General("||||||||_Y", openList[13][35].parent->y);
			}
		}*/

	}
	
	

	/*Sets new parent as openNode with the lowest F cost*/
	LowestF(openLength, &parentNode); /*Open list*/

	
	if (parentNode.exists != 1) {
		
		return;
	}
	/*If parent node is the goal!*/
	else if (parentNode.x == endX && parentNode.y == endY)
	{
		/*Adds parent to closed list*/
		AddListNode(closedList, &closedLength, &parentNode);
		/*Remove parent from open list*/
		RemoveListNode(openList, &openLength, &parentNode);
		
		/*Get the last node in the closed list*/
		GetNode(closedList, closedLength-1, &lastClosed);

		/*Connect path by tracing back parents, starting at lastClosed Node*/
		ConnectPath(pathList, &pathLength, &lastClosed);
		return;
	}
	
	/*Adds parent to closed list*/
	AddListNode(closedList, &closedLength, &parentNode);

	/*Remove parent from open list*/
	RemoveListNode(openList, &openLength, &parentNode);
	/*if (goal_Node.x == 35 && goal_Node.y == 13) {
		if (timeToCheck) {
			DebugMode_General("F=====", openList[13][35].x);
			DebugMode_General("F=====", openList[13][35].y);
			DebugMode_General("F=====", openList[13][35].parent->x);
			DebugMode_General("F=====", openList[13][35].parent->y);
			DebugMode_General("F=====", parentNode.x);
			DebugMode_General("F=====", parentNode.y);
			DebugMode_General("F=====", parentNode.parent->x);
			DebugMode_General("F=====", parentNode.parent->y);
		}
	}*/

	/*if (goal_Node.x == 35 && goal_Node.y == 13) {
		if (openList[13][36].exists) {
			DebugMode_General("0000000_X", openList[13][36].x);
			DebugMode_General("0000000_Y", openList[13][36].y);
			if (openList[13][36].parent != NULL) {
				DebugMode_General("44545454_X", openList[13][36].parent->x);
				DebugMode_General("44545454_Y", openList[13][36].parent->y);
			}

		}
	}*/

	/*Recursive call to function, replacing parent as the open node with lowest fCost*/
	FindPath(start_Node, goal_Node, parentNode, *(parentNode.parent), endX, endY, curLvl);

}

/*Parent pointers are followed from goal until it reaches start*/
void ConnectPath(struct NODE path[][WIDTH], int *pathLen, struct NODE* parent)
{
	struct NODE* tempNode = parent;
	while (tempNode->parent != NULL) {
		/*Adds parent to the path list*/
		AddListNode(path, pathLen, tempNode);


		if (goalNode.x == 35 && goalNode.y == 13) {
			//DebugMode_General("CurrX", tempNode->x);
			//DebugMode_General("CurrY", tempNode->y);
			//DebugMode_General("P_X", tempNode->parent->x);
			//DebugMode_General("P_Y", tempNode->parent->y);
		}

		/*Parent Node is overwritten by parent Node of itself*/
		tempNode = &closedList[tempNode->parent->y][tempNode->parent->x];
		tempNode->parent = closedList[tempNode->y][tempNode->x].parent;

		if (goalNode.x == 35 && goalNode.y == 13) {
			//DebugMode_General(">>>>>>_X", closedList[tempNode->y][tempNode->x].parent->x);
			//DebugMode_General(">>>>>>_Y", closedList[tempNode->y][tempNode->x].parent->y);
			
		}
	}
	/*Flips id's in path because they were recorded in reverse*/
	ReverseID(path, *pathLen);
}

void InformPath(struct GUARD* guard)
{
	short h = 0, w = 0;
	int id = 0, oldid = 0;
	int bPathfound = 0;
	int bBreakOut = 0;
	
	while (!bPathfound) {
		oldid = id;
		for (h = 0; h < HEIGHT; ++h) {
			for (w = 0; w < WIDTH; ++w) {
				if (pathList[h][w].id == id) {
					guard->pathList[id].X = w;
					guard->pathList[id].Y = h;
					if (id >= guardPathLimit-1) {
						bBreakOut = 1;
						break;
					}
					++id;
				}
			}
			if (bBreakOut) 
				break;	
		}
		if (oldid == id) 
			bPathfound = 1;
	}
	//DebugMode_General("Over Here", id);

	if (id == 0) {
		/*No pathfound*/
	}
}

/*Returns node with lowest F value*/
void LowestF(int length, struct NODE *output)
{
	/*Copies open nodes to new 1D array for sorting*/
	struct NODE* sort = (struct NODE*)malloc(sizeof(struct NODE) * length); /*Array for 100*100 game map*/
	int bswitch = -1;
	struct NODE curr, next;
	int i = 0;
	struct NODE smallestFNode = {0};

	ArrangeID(sort, openList, length);  /*Seems redundant...but see how*/
	/*OLD CODE*/
	/*Sorts open array from low to high*/
	while (bswitch != 0) {
		bswitch = 0;


		if (sort != NULL) {
			/*Iterates each existing open node (length)*/
			for (i = 0; i < length - 1; ++i) {
				curr = *(sort + i);
				next = *(sort + i + 1);

				/*SWAP: if curr Node has greater f than next Node*/
				if (curr.f > next.f) {
					*(sort + i) = next;
					*(sort + i + 1) = curr;
					bswitch = 1;
				}
			}

			smallestFNode = *(sort);
			//smallestFNode.parent = sort->parent;
		}
	}

	/*if (timeToCheck) {
		DebugMode_General("gggggggg_X", sort->x);
		DebugMode_General("gggggggg_Y", sort->y);
		DebugMode_General("gggggggg_X", openList[sort->y][sort->x].parent->x);
		DebugMode_General("gggggggg_Y", openList[sort->y][sort->x].parent->y);
		DebugMode_General("gggggggg_X", smallestFNode.x);
		DebugMode_General("gggggggg_Y", smallestFNode.y);
		DebugMode_General("gggggggg_X", smallestFNode.parent->x);
		DebugMode_General("gggggggg_Y", smallestFNode.parent->y);
	}*/

	free(sort);
	*output = smallestFNode; /*Smallest F value;*/
	output->parent = smallestFNode.parent;


	/*if (goalNode.x == 35 && goalNode.y == 13) {
		if (timeToCheck) {
			DebugMode_General("F!#@$", output->x);
			DebugMode_General("F!#@$", output->y);
			DebugMode_General("F!#@$", output->parent->x);
			DebugMode_General("F!#@$", output->parent->y);

		}
	}*/
}