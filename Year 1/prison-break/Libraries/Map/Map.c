/*********************************************************************************
Map.c
Author: Chong Jun Yi
Code Lines: 88
Brief: Map checking functionality for the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include "Map.h"
#include "../Console/Console.h"/*Render*/
#include "../Global/Global.h"
#include "../TxtFileReader/TxtFileReader.h"
#include "../Item/Item.h"
#include "../Human/Human.h"	/*Get facing location*/
#include "../Guard/Guard.h"
#include "../States/Game.h"		/*Notification*/

COORD localMapOffset = { 0 }; /*Scrolling map, determine what section of map shown*/
COORD playerOffSet = { 0,0 }; /*Normally player will be at centre of screen, until reach map edge where his display will be offset*/

int itemCode = 0;

/*Return 0 if no collision, Return 1 and above if collision*/
int Map_CheckCollision(int posX, int posY, int currLvl, int whoCheck)
{
	int i = 0;
	int animationFrame = 0;

	/*Game Play area starts from 1, 1 due to game border*/
	--posX;
	--posY;

	/*Check against next level border*/
	if (levelGraphicData[currLvl][posX][posY][0] == 236 && whoCheck == 0) {
		if (levelGraphicData[currLvl][posX][posY][1] == LIGHTGREEN) {
			for (i = 0; i < activeInventoryCount; ++i)
			{
				if (strcmp(activeInventoryList[i].name, "LVLKey") == 0){ /*Check if player has key to proceed*/
					return 0;
				}
			}
		}
		CreateNotification(4, 0, -1);
		return 1;
	}

	/*Check against Level Walls*/
	/*White Background means item zone, check if ascii is not null and BG is not white*/
	if (levelGraphicData[currLvl][posX][posY][0] != 0 && levelGraphicData[currLvl][posX][posY][2] != WHITE) {
		return 1;
	}

	/*Check item zone, if curr animation frame has any ascii for collision*/
	if (levelGraphicData[currLvl][posX][posY][2] == WHITE) {
		for (i = 0; i < activeItemCount; ++i){
			/*Check if which item the zone belong to*/
			if (posX >= activeItemList[i].zoneOrig.X && posY >= activeItemList[i].zoneOrig.Y
				&& posX <= activeItemList[i].zoneEnd.X && posY <= activeItemList[i].zoneEnd.Y ) {

				if (activeItemList[i].interactType == interact_passive) 
				{
					break;
				}

				/*Get animation frame to compare*/
				animationFrame = activeItemList[i].graphicIndex % 16;
				itemCode = levelGraphicData[currLvl][posX][posY][0] / 16;

				switch (itemCode) 
				{
				/*Split based on item code becos we dont know how many sprite each item type has*/
					case 0xD:/*Door*/
						if (itemGraphicData[animationFrame][posX - activeItemList[i].zoneOrig.X][posY - activeItemList[i].zoneOrig.Y][0] != 0)
							return 1;
						break;
				}
			}
		}
	}

	/*Check against PlayArea Boundary, not necessary cos player will be at centre of screen*/
	if (posX == 0 || posX == 100 || posY == 0 || posY == 100) {
		return 1;
	}

	/*Player check if collide into guards*/
	if (whoCheck == 0) {
		for (i = 0; i < activeGuardCount; ++i) {
			if (activeGuardList[i].pos.X == posX+1 && activeGuardList[i].pos.Y == posY+1) {
				return 1;
			}
		}
	}
	return 0; /*No collision at that point*/
}

/*Maths calculation for offset for map and player offset*/
void ScrollingMap(){
	localMapOffset.X = (PlayerChar.pos.X - (Global_playArea.X / 2)) > 0 ? (PlayerChar.pos.X - (Global_playArea.X / 2)) : 0;
	localMapOffset.Y = (PlayerChar.pos.Y - (Global_playArea.Y / 2)) > 0 ? (PlayerChar.pos.Y - (Global_playArea.Y / 2)) : 0;

	/*if at edge of screen*/
	if (localMapOffset.X == 0){
		if (PlayerChar.pos.X <= Global_screenSize.X / 2){
			playerOffSet.X = PlayerChar.pos.X - Global_playArea.X / 2;
		}
		else{

		}
	}
	/*No player offset since map is not at edge, player remain at center of screen*/
	else{
		playerOffSet.X = 0;
	}

	/*if at edge of screen*/
	if (localMapOffset.Y == 0){
		if (PlayerChar.pos.Y <= Global_screenSize.Y / 2){
			playerOffSet.Y = PlayerChar.pos.Y - Global_playArea.Y / 2;
		}
		else{

		}
	}
	/*No player offset since map is not at edge, player remain at center of screen*/
	else{
		playerOffSet.Y = 0;
	}
}

/*Check if a position is within a specified zone*/
int CheckZoneCollision(int posX, int posY, COORD zoneOrig, COORD zoneEnd) 
{
	if (posX >= zoneOrig.X) {
		if (posX <= zoneEnd.X) {
			if (posY >= zoneOrig.Y) {
				if (posY <= zoneEnd.Y) {
					return 1;
				}
			}
		}
	}
	return 0;
}

int WithinPlayArea(int posX, int posY, int leftEdgeOffset, int rightEdgeOffset) {
	if (posX - localMapOffset.X <= Global_playArea.X + rightEdgeOffset) {
		if (posX - localMapOffset.X >= 0 + leftEdgeOffset) {
			if (posY - localMapOffset.Y <= Global_playArea.Y + rightEdgeOffset) {
				if (posY - localMapOffset.Y >= 0 + leftEdgeOffset) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/*Get the position of a pixel in front of anything facing*/
COORD GetFacingPosition_General(short posX, short posY, Direction dir, int distance)
{
	COORD facingCoord = { 0,0 };
	facingCoord.X = posX, 
	facingCoord.Y = posY ;

	switch (dir)
	{
	case DIR_RIGHT:
		facingCoord.X += (short)distance;
		break;
	case DIR_UP:
		facingCoord.Y -= (short)distance;
		break;
	case DIR_LEFT:
		facingCoord.X -= (short)distance;
		break;
	case DIR_DOWN:
		facingCoord.Y += (short)distance;
		break;
	}
	return facingCoord;
}