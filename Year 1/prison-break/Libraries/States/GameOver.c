/*********************************************************************************
GameOver.c
Author: Benjamin (UI)
Code Lines: 50 (Approx)
Brief: Statemachine for victory and game over.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <Windows.h> /*asynckeystate, */
#include "GameOver.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../States/MainMenu.h"
#include"../Highscore/Highscore.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************

int gameOverCount = 0;
bool gameWin = 0;

//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void GameOver_EnterState()
{
	gameOverCount++;
}

//*********************************************************************************
//									INPUT
//*********************************************************************************
void GameOver_ProcessInput()
{
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		StateMachine_ChangeState(State_Highscore);
		playerentered = 0;
	}
}
//*********************************************************************************
//									UPDATE
//*********************************************************************************
void GameOver_Update()
{
}

//*********************************************************************************
//									RENDER
//*********************************************************************************
void GameOver_Render()
{
	int y = 10;

	//Winning Screen when player reaches the end
	if (gameWin == 1)
	{
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(20, y++, "MISSION ACCOMPLISHED");
		Console_SetRenderBuffer_String(13, y++, "************************************");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(15, y++, "YOU ATTAINED YOUR FREEDOM BUT...");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(20, y++, "IS IT REALLY WORTH IT?");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(13, y++, "************************************");
	}
	else
	{
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(26, y++, "GAME OVER");
		Console_SetRenderBuffer_String(13, y++, "************************************");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(13, y++, "You are sent back to your jail cell.");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(13, y++, "************************************");
	}

	/*=============================================================================*/
	/*                    INSTRUCTIONS TO GO BACK TO MENU                          */
	/*=============================================================================*/
	Console_SetRenderBuffer_String(18, y, "Press");
	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(24, y, "<SPACE>");
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(32, y++, "to continue");
}

//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void GameOver_ExitState()
{
	highScoreCounter -= 1;
}
