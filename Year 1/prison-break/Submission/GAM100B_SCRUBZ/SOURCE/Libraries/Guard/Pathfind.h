/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef PATHFIND_H
#define PATHFIND_H
#define HEIGHT 200
#define WIDTH 200
#define PI_2 3.14159/2
#include "../Guard/Guard.h"

/*Node struct for pathfinding*/
typedef struct NODE {
	struct NODE *parent;
	int exists;
	int x;		/*x coordinate on node grid*/
	int y;		/*y coordinate on node grid*/
	int g;		/*G VALUE*/
	int h;		/*H VALUE*/
	int f;		/*F VALUE*/
	int id;
} Node;

struct PATH {
	int x;
	int y;
};

/*Declaring Node arrays*/
struct NODE openList[HEIGHT][WIDTH];
struct NODE closedList[HEIGHT][WIDTH];
struct NODE pathList[HEIGHT][WIDTH];

struct PATH path_Traversal[100];

struct NODE startNode;
struct NODE goalNode;
struct NODE newNode;

/*Declaring and initializing lengths for Node arrays*/
int openLength;
int closedLength;
int pathLength;

void InitNodeList();

void InitNodes(struct NODE list[][WIDTH]);

void ClearNode(struct NODE* node);

void ClearListNode(struct NODE list[][WIDTH], int row, int col);

void GetNode(struct NODE list[][WIDTH], int findIndex, struct NODE *output);

void SetParentForNode(struct NODE* copy, struct NODE *orig);

void AddListNode(struct NODE list[][WIDTH], int* length, struct NODE *node);
void RemoveListNode(struct NODE list[][WIDTH], int* length, const struct NODE *node);

void ArrangeID(struct NODE *arrange, struct NODE original[][WIDTH], int length);

void ReverseID(struct NODE path[][WIDTH], int pathLength);

void CalculateNodeValues(struct NODE parent, struct NODE goal, int possibleX, int possibleY, int* openLength);

void StartPathfinding(struct GUARD *guard, int curLvl);

void FindPath(Node startNode, Node goalNode, Node parentNode, Node parentPNode, int endX, int endY, int curLvl);

void ConnectPath(struct NODE path[][WIDTH], int* pathLength, struct NODE *parent);

void InformPath(struct GUARD* guard);

void LowestF(int length, struct NODE* output);

#endif // !PATHFIND_H
