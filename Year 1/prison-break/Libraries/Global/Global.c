/*********************************************************************************
Global.c
Author: Sakthiish (Global variables)
Code Lines: 18 ( 5 + 6 + 2 + 2 + 3) 
Author: Chong Jun Yi (Notification variables)
Code Lines: 30 (6 + 17 + 7(header direction enum))
Brief: Global variables for the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <windows.h>
#include <stdio.h> /*DEBUG*/
#include <stdbool.h>/*bool var type*/
#include "Global.h"
#include "../Console/Console.h" /*DEBUG*/

//*********************************************************************************
//								HOUSEKEEPING RULES
//		1) Global can start with Caps due to global scope
//		2) Global functions identifier/name written with _
//		3) Global variable identifier/name written without _
//		4) boolean var indicate 'b', pointer var indicate 'p'
//*********************************************************************************

//*********************************************************************************
//							GAME PROGRAM EXIT SYSTEM
//*********************************************************************************

static bool Global_bExitRequested = false; /*If player wants to exit game*/

COORD Global_playArea = {32,24}; /*Map area size*/
COORD Global_screenSize = { 60, 32}; /*Application window size*/
const COORD Global_fontSizeOrig = { 20, 24 }; /*original font size*/
COORD Global_fontSize = { 20, 24}; /*Smallest is 4*/
bool Global_bAssetsLoaded = false; /*If text files are loaded*/

/*Mainly for game notification since it uses a int array to display extended ascii char*/
const int colorSymbol[16][8] = {
	{'B', 'L', 'A', 'C', 'K'},
	{'B','L','U','E'},
	{'G','R','E','E','N'},
	{'C','Y','A','N'},
	{'R','E','D'},
	{'M','A','G','E','N','T','A'},
	{'B','R','O','W','N'},
	{'L','-','G','R','A','Y'},
	{'D','-','G','R','A','Y'},
	{'L','B','L','U','E'},
	{'L','G','R','E','E','N'},
	{'L','C','Y','A','N'},
	{'L','R','E','D'},
	{'L','M','A','G','E','N','T','A'},
	{'Y','E','L','L','O','W'},
	{'W','H','I','T','E'}
};

/*********************************************************************************
Global_ExitProgram()
Author: Sakthiish
Code Lines: 2
Brief: Exit the game application
**********************************************************************************/
void Global_ExitProgram()
{
	Global_bExitRequested = true;
}

/*********************************************************************************
Global_bIsGameRunning()
Author: Sakthiish
Code Lines: 2
Brief: Determine if game is running
**********************************************************************************/
bool Global_bIsGameRunning()
{
	return (!Global_bExitRequested);
}

/*********************************************************************************
Global_AssetsAreLoaded()
Author: Sakthiish
Code Lines: 2
Brief: If text files are loaded
**********************************************************************************/
void Global_AssetsAreLoaded()
{
	Global_bAssetsLoaded = true;
}

/*********************************************************************************
Global_ConformAccurateScreenSize()
Author: Sakthiish
Code Lines: 3
Brief: For some players screen who cannot support the intended size
**********************************************************************************/
void Global_ConformAccurateScreenSize(short ssWidth, short ssHeight)
{
	Global_screenSize.X = ssWidth;
	Global_screenSize.Y = ssHeight;
}

/*********************************************************************************
DebugMode_General()
Author: Chong Jun Yi
Code Lines: 6
Brief: Used to check values in release mode
**********************************************************************************/
void DebugMode_General(char* string, int value) {
	Console_SetCursorPos(1, 29);
	printf("                           "); /*Clear old values*/
	Console_SetCursorPos(1, 29);
	printf("%s - %d", string, value);
	Console_SetCursorPos(0, 0);
	Sleep(1000);
}