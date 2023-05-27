/*********************************************************************************
Instructions.c
Author: Benjamin
Code Lines: 83
Brief: Instruction functionality for the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <Windows.h> /*asynckeystate, */
#include "Instructions.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../States/Game.h"
#include "../States/MainMenu.h"
#include "../Global/Global.h"
#include "..//Item/Item.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************

bool toggleInstructionPages = 1;

//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Instructions_EnterState()
{
}

void Instructions_ExitState()
{
}
//*********************************************************************************
//									INPUT
//*********************************************************************************
void Instructions_ProcessInput()
{
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		StateMachine_ChangeState(State_MainMenu);
	}

	/*Move through instruction pages*/
	if((GetAsyncKeyState(VK_RIGHT) & 1) || (GetAsyncKeyState('D') & 1))
	{
		--toggleInstructionPages;
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 1) || (GetAsyncKeyState('A') & 1))
	{
		++toggleInstructionPages;
	}
}

//*********************************************************************************
//									UPDATE
//*********************************************************************************

void Instructions_Update()
{

}


//*********************************************************************************
//									RENDER
//*********************************************************************************
void Instructions_Render()
{
	int yKeyLineCount = 4;
	int xLineCount = 0;
	int ySwitchLineCount = 16;
	int i = 7;

	if (toggleInstructionPages)
	{
		/*Printing of Divider*/
		while (i < 19)
		{
			for (int j = 5; j < 55; j++)
			{
				Console_SetRenderBuffer_String(j, i, "-");
			}
			i += 4;
		}

		/*Printing of Title Header*/
		Console_SetRenderBuffer_String(5, 2, "MOVEMENTS");
		Console_SetRenderBuffer_String(5, 3, "=========");
		Console_SetRenderBuffer_String(9, 5, "W");
		Console_SetRenderBuffer_String(7, 6, "A S D");
		Console_SetRenderBuffer_String(9, 9, "E");
		Console_SetRenderBuffer_String(9, 13, "-");
		Console_SetRenderBuffer_String(9, 18, "P");

		/*Printing of Interactable Icons*/
		Console_SetRenderBuffer_String(22, 2, "ICON");
		Console_SetRenderBuffer_String(22, 3, "====");
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(24, 5, 12);   /*HUMAN*/
		Console_SetRenderBuffer_Attribute(MAGENTA, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(21, 9, 11);   /*KEY*/
		Console_SetRenderBuffer_Attribute(GREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(23, 9, 168);  /*LVLKEY*/
		Console_SetRenderBuffer_Attribute(YELLOW, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(25, 9, 4);    /*SWITCH*/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(27, 9, 219);  /*ROCK*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(22, 13, 17);  /*BLINKER*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(26, 13, 16);  /*BLINKER*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(24, 12, 30);  /*BLINKER*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(24, 14, 31);  /*BLINKER*/
		Console_SetRenderBuffer_String(24, 18, "-");

		/*Printing of Description*/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(38, 2, "DESCRIPTION");
		Console_SetRenderBuffer_String(38, 3, "===========");
		Console_SetRenderBuffer_String(33, 5, "MOVEMENT OF THE PLAYER");
		Console_SetRenderBuffer_String(33, 6, "");
		Console_SetRenderBuffer_String(33, 9, "INTERACTION OF ITEMS");
		Console_SetRenderBuffer_String(33, 10, "");
		Console_SetRenderBuffer_String(33, 13, "BLINKER SHOWS");
		Console_SetRenderBuffer_String(33, 14, "CHARACTER DIRECTION");
		Console_SetRenderBuffer_String(33, 18, "PAUSE THE GAME");

		Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(34, 23, "> Next Page");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(27, 23, "<PAGE 1");
		Console_SetRenderBuffer_String(17, 23, "Prev Page");
	}
	else
	{
		/*Printing of Divider*/
		while (i < 19)
		{
			for (int j = 5; j < 55; j++)
			{
				Console_SetRenderBuffer_String(j, i, "-");
			}
			i += 4;
		}

		/*Printing of Title Header*/
		Console_SetRenderBuffer_String(5, 2, "ITEM");
		Console_SetRenderBuffer_String(5, 3, "=====");
		Console_SetRenderBuffer_String(5, 6, "KEY");
		Console_SetRenderBuffer_String(5, 10, "LVLKEY");
		Console_SetRenderBuffer_String(5, 14, "ROCK");
		Console_SetRenderBuffer_String(5, 18, "SWITCH");
		Console_SetRenderBuffer_String(22, 2, "ICON");
		Console_SetRenderBuffer_String(22, 3, "====");

		/*Going through and printing all colour KEYS icons except black*/
		for (int count = 0x0001; count <= 0x000F; count++)
		{
			Console_SetRenderBuffer_Attribute(count, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(21 + xLineCount, yKeyLineCount, 11); /*(x.pos, y.pos, Ascii Code)*/
			if (xLineCount < 4)
			{
				++xLineCount;
			}
			else if (xLineCount == 4)
			{
				xLineCount = 0;
				++yKeyLineCount;
			}
		}
		/*Printing of other ITEM ICONS*/
		Console_SetRenderBuffer_Attribute(GREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(23, 9, 168); /*LvlKey*/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(23, 13, 4); /*Rock*/

		/*Going through and printing all colour SWITCH icons except black*/
		for (int count = 0x0001; count <= 0x000F; count++)
		{
			Console_SetRenderBuffer_Attribute(count, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(19 + xLineCount, ySwitchLineCount, 219); /*(x.pos, y.pos, Ascii Code)*/
			if (xLineCount < 8)
			{
				xLineCount += 2;
			}
			else if (xLineCount == 8)
			{
				xLineCount = 0;
				ySwitchLineCount += 2;
			}
		}

		/*Description of items*/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(38, 2, "DESCRIPTION");
		Console_SetRenderBuffer_String(38, 3, "===========");
		Console_SetRenderBuffer_String(33, 5, "COLOUR KEYS OPEN");
		Console_SetRenderBuffer_String(33, 6, "SIMILAR COLOURED DOORS");
		Console_SetRenderBuffer_String(33, 9, "LVLKEY IS NEEDED TO");
		Console_SetRenderBuffer_String(33, 10, "GO TO THE NEXT LEVEL");
		Console_SetRenderBuffer_String(33, 13, "PRESS 'Q' TO THROW");
		Console_SetRenderBuffer_String(33, 14, "THE ROCK");
		Console_SetRenderBuffer_String(33, 17, "COLOUR SWITCHES OPEN");
		Console_SetRenderBuffer_String(33, 18, "SIMILAR COLOURED DOORS");

		Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(17, 23, "Prev Page");
		Console_SetRenderBuffer_String(27, 23, "<");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(28, 23, "PAGE 2> Next Page");
	}
		Console_SetRenderBuffer_String(9, 25, "Press");
		Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(15, 25, "<SPACE>");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(23, 25, "to continue to the Main Menu");
}


