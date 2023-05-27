/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef MAP_H
#define MAP_H
#include "../Global/Global.h" /*COORD from window.h*/

COORD localMapOffset; /*Scrolling map, determine what section of map shown*/
COORD playerOffSet;

/**
 * \brief	Define spaces for gamescreen, UI, ect
 */

 /*Return 0 if no collision, Return 1 and above if collision on that level grid*/
int Map_CheckCollision(int posX, int posY, int currLvl, int whoCheck);

/*Maths calculation for offset for map and player offset*/
void ScrollingMap();

/*Check if a position is within a specified zone*/
int CheckZoneCollision(int posX, int posY, COORD zoneOrig, COORD zoneEnd);

int WithinPlayArea(int posX, int posY, int leftEdgeOffset, int rightEdgeOffset);

/*Get the position of a pixel in front of anything facing*/
COORD GetFacingPosition_General(short posX, short posY, Direction dir, int distance);

#endif // MAP_H