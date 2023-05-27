/*********************************************************************************
Human.c
Author: Choong Jun Yi 
Code Lines: 50 (Approx)
Author: Sakthiish (Graphics)
Code Lines: 50 (Approx)
Brief: Human struct functionality in the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
//*********************************************************************************
//						     Human Struct
//			With name. to use just use person _______<- put a name here
//*********************************************************************************

#include <windows.h>/*COORD*/
#include <math.h>
#include "Human.h"
#include "../Global/Global.h"

struct HUMAN PlayerChar =
{
	"PlayerChar",	/*Name*/
	0,				/*Graphic Index*/
	{0},			/*Positon*/
	{0},			/*Float position*/
	12,				/*Movespeed*/
	DIR_UP,		    /*Facing direction*/
	{0},			/*Inventory*/
	0.0f,			/*CooldownTime*/
};

int bIsPlayerWalking = 0;
int bIsPlayerHit = 0;
/*********************************************************************************
/								Initalisation Functions
/********************************************************************************/

/*Spawn player character at location*/
void Spawn_PlayerChar(short x, short y)
{
	PlayerChar.pos.X = x;
	PlayerChar.pos.Y = y;
	PlayerChar.fpos.X = (float)x;
	PlayerChar.fpos.Y = (float)y;
}

/*********************************************************************************
/							General Functions for Humans
/********************************************************************************/

/*Euler Movement based on a direction*/
void Human_Movement(enum DIRECTION dir, struct HUMAN *human, float deltaTime)
{
	
	switch (dir)
	{
		case DIR_RIGHT:
			if (deltaTime > 100.0f) {
				human->pos.X += 1; /*Move one space directly*/
				human->fpos.X = human->pos.X;
			}
			else {
				human->fpos.X += human->moveSpeed * 0.001f * deltaTime; /*Move with deltatime*/
			}
			human->pos.X = (short)roundf(human->fpos.X);
			human->direction = DIR_RIGHT;
			break;
		case DIR_UP:
			if (deltaTime > 100.0f) {
				human->pos.Y -= 1;
				human->fpos.Y = human->pos.Y;
			}
			else {
				human->fpos.Y -= human->moveSpeed * 0.001f * deltaTime;
			}
			human->pos.Y = (short)roundf(human->fpos.Y);
			human->direction = DIR_UP;
			break;
		case DIR_LEFT:
			if (deltaTime > 100.0f) {
				human->pos.X -= 1;
				human->fpos.X = human->pos.X;
			}
			else {
				human->fpos.X -= human->moveSpeed * 0.001f * deltaTime;
			}		
			human->pos.X = (short)roundf(human->fpos.X);
			human->direction = DIR_LEFT;
			break;
		case DIR_DOWN:
			if (deltaTime > 100.0f) {
				human->pos.Y += 1;
				human->fpos.Y = human->pos.Y;
			}
			else {
				human->fpos.Y += human->moveSpeed * 0.001f * deltaTime;
			}

			human->pos.Y = (short)roundf(human->fpos.Y);
			human->direction = DIR_DOWN;
			break;
	}
}

/*Get the position of a pixel in front of Human facing*/
COORD GetFacingLocation(struct HUMAN* human, int distance)
{
	COORD facingCoord; 

	facingCoord = human->pos;
	switch (human->direction)
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

void ResetPlayerProjectileHit() {
	bIsPlayerHit = 0;
}

int IsPlayerProjectileHit() {
	return bIsPlayerHit;
}

/*If bullet hits player*/
void CheckCollisionAgainstPlayer(int x, int y) {
	if (x == PlayerChar.pos.X && y == PlayerChar.pos.Y) 
		bIsPlayerHit = 1;
	
}