/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef HUMAN_H
#define HUMAN_H

#include "../Global/Global.h" /*COORD from window.h*/
#include "../Item/Item.h"

typedef struct HUMAN 
{
	char name[20];			/*Name of human: Guard, Player, NPC*/
	int graphicIndex;		/*The index from graphics array*/
	COORD pos;				/*Current DISPLAY position of human*/
	fCOORD fpos;			/*Current EXACT position of human*/
	int moveSpeed;			/*Movement Speed per realtime sec of human*/
	int direction;			/*Sets the facing direction using global enum*/
	Item inventory[5];		/*Inventory*/
	float cooldownTime;		/*CooldownTime*/
}Human;

int bIsPlayerWalking;

/*Declare all the human variables*/
struct HUMAN PlayerChar;

/**
 * \brief	Spawn the player at a given location
 */
void Spawn_PlayerChar(short x, short y);

/**
 * \brief	Move the human by their COORD pos
 */
void Human_Movement(enum DIRECTION dir, struct HUMAN *human, float deltaTime);

/*Get the position of a pixel in front of Human facing*/
COORD GetFacingLocation(struct HUMAN* human, int distance);

void CheckCollisionAgainstPlayer(int x, int y);

int IsPlayerProjectileHit();

void ResetPlayerProjectileHit();
#endif // !HUMAN_H

