/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdbool.h>/*bool var type*/
#include <windows.h>

static bool Global_bExitRequested; /*If player wants to exit game*/

COORD Global_playArea; /*Map area size*/
COORD Global_screenSize; /*Application window size*/
extern const COORD Global_fontSizeOrig; /*original font size*/
COORD Global_fontSize; /*Smallest is 4*/
bool Global_bAssetsLoaded; /*If text files are loaded*/

/*********************************************************************************
/								Global enum/struct
*********************************************************************************/
typedef enum DIRECTION /*Directions for uses for guard, human*/
{
	DIR_RIGHT = 0,
	DIR_UP = 1,
	DIR_LEFT = 2,
	DIR_DOWN = 3
}Direction;

typedef struct FCOORD /*Float coordinate*/
{
	float X;
	float Y;
}fCOORD;

/*Mainly for game notification since it uses a int array to display extended ascii char*/
extern const int colorSymbol[16][8];

/**
 * \brief	Call to exit the game program
 * \brief	Set boolean exit requested	
 */
void Global_ExitProgram();

/**
 * \brief	Check if game is still running/not request exit
 */
bool Global_bIsGameRunning();

/**
 * \brief	Call after game assets have been loaded
 */
void Global_AssetsAreLoaded();

/**
 * \brief	Force the actual screensize in case some player pc have smaller screen
 */
void Global_ConformAccurateScreenSize(short ssWidth, short ssHeight);

/*Used to check values in release mode*/
void DebugMode_General(char* string, int value);
#endif // GLOBAL_H
