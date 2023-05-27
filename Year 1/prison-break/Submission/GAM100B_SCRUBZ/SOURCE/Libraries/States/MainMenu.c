/*********************************************************************************
MainMenu.c
Author: Chong Jun Yi
Code Lines: 75 (Approx)
Author: Benjamin
Code Lines: 100 (Approx)
Brief: Main Menu functions and statemachine.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <Windows.h>
#include <math.h>					/*Round*/
#include "MainMenu.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../Global/Global.h"
#include "../TxtFileReader/TxtFileReader.h"
#include "../Clock/Clock.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************

int pageNavigationIndex = 0;
int numOfNavigationElement = 6;

float tipBlinkAnimTime = 0.0f;
float tipBlinkAnimDuration = 0.8f;

float runningManAnimTime = 0;
float runningManAnimSpeed = 4.0f;
float runningManFameMax = 6.0f;

float gameLogoAnimaTime = 0.0f;
//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void MainMenu_EnterState()
{
	pageNavigationIndex = 0;
}

void MainMenu_ExitState()
{
}

//*********************************************************************************
//									INPUT
//*********************************************************************************
void MainMenu_ProcessInput()
{
	if (!Global_bAssetsLoaded) {
		return;
	}

	/*Move up and down to choose the next page*/
	if (GetAsyncKeyState('W') & 1) 
	{
		ChangeNavigationElement(-1);
	}
	else if (GetAsyncKeyState('S') & 1) 
	{
		ChangeNavigationElement(1);
	}
	else if (GetAsyncKeyState(VK_UP) & 1)
	{
		ChangeNavigationElement(-1);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 1)
	{
		ChangeNavigationElement(1);
	}

	/*Select the next page*/
	if (GetAsyncKeyState(VK_SPACE) &1) {
		switch (pageNavigationIndex) {
		case 0:
			StateMachine_ChangeState(State_Game);
			break;
		case 1:
			StateMachine_ChangeState(State_Instructions);
			break;
		case 2:
			StateMachine_ChangeState(State_Highscore);
			break;
		case 3:
			StateMachine_ChangeState(State_Settings);
			break;
		case 4:
			StateMachine_ChangeState(State_Credits);
			break;
		case 5:
			Global_ExitProgram();
			break;
		}
	}
}

//*********************************************************************************
//									UPDATE
//*********************************************************************************

void MainMenu_Update()
{
	if (!Global_bAssetsLoaded) {
		return;
	}
	
	/**************************ANIMATION timer updates**************************/
	tipBlinkAnimTime += 0.001f * Clock_GetDeltaTime();
	if (tipBlinkAnimTime>tipBlinkAnimDuration) {
		tipBlinkAnimTime = -tipBlinkAnimDuration;
	}

	runningManAnimTime += runningManAnimSpeed * 0.001f * Clock_GetDeltaTime();
	if (runningManAnimTime >= runningManFameMax-0.5f) {
		runningManAnimTime -= runningManFameMax;
	}

	gameLogoAnimaTime += 20 * 0.001f * Clock_GetDeltaTime();
}


//*********************************************************************************
//									RENDER
//*********************************************************************************
void MainMenu_Render()
{
	int w = 0, h = 0;

	if (!Global_bAssetsLoaded) {
		return;
	}

	/*Render Game Logo*/
	for (w = 0; w < 60; ++w) 
	{
		for (h = 0; h < 18; ++h)
		{
			Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
			if (h == (int)roundf(gameLogoAnimaTime) % 32) {
				Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, DEFAULTLVB);
			}
			Console_SetRenderBuffer_Char(w + 1, h + 1, (char)uISplashScreenData[0][w][h][0]);
		}
		
	}

	Console_SetRenderBuffer_Attribute(GREEN, BLACK, DEFAULTLVB);
	/*Render Running Man*/
	for (w = 0; w < 6; ++w)
	{
		for (h = 0; h < 4; ++h)
		{
			Console_SetRenderBuffer_Char((int)round(runningManAnimTime) * 2 + 40 + w, 8 + h, (char)uISplashScreenData[1 + (int)round(runningManAnimTime)][w][h][0]);
		}
	}

	/*Render Navigation Pages Title*/
	for (int i = 0; i < numOfNavigationElement; ++i)
	{
		if (pageNavigationIndex == i) {
			/*Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_String(3, 20+i, "    ");*/
			Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB); /*Unsed 181*/
		}
		else
		{
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, RED, GRID_FULL | UNDERSCORE);
		}

		switch (i)
		{
			/*Play*/
		case 0:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 20, "Play Campaign ");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 20, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 20, 240);
			
			break;
			/*Instructions*/
		case 1:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 21, "Instructions ");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 21, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 21, 240);
				
			break;
			/*Stats*/
		case 2:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 22, "Stats");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 22, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 22, 240);
			break;
			/*Settings*/
		case 3:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 23, "Settings");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 23, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 23, 240);
			break;
			/*Credits*/
		case 4:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 24, "Credits ");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 24, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 24, 240);
			break;
			/*Exit*/
		case 5:
			Console_SetRenderBuffer_String(4 + 2 * IsNavSelect(i), 25, "Quit ");
			Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL | UNDERSCORE);
			Console_SetRenderBuffer_Char(2, 25, 32);
			Console_SetRenderBuffer_Char(3 - 1 * IsNavSelect(i), 25, 240);
			break;
		}
	}

	/*Interaction tips*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	if (tipBlinkAnimTime > 0) {
		Console_SetRenderBuffer_String(Global_screenSize.X - 24, 25, "Press       to Continue"); /*23*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(Global_screenSize.X - 18, 25, "SPACE");
	}

	/*Arrow navigation tip*/
	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
	if (pageNavigationIndex >= 1) {
		Console_SetRenderBuffer_Char(1, 21, 30);
	}
	if (pageNavigationIndex < numOfNavigationElement - 1) {
		Console_SetRenderBuffer_Char(1, 24, 31);
	}
}



void ChangeNavigationElement(int offset)
{
	if (pageNavigationIndex + offset >= numOfNavigationElement
		|| pageNavigationIndex + offset < 0) 
	{
		return;
	}
	pageNavigationIndex += offset;
}

int IsNavSelect(int i) {
	return pageNavigationIndex == i;
}