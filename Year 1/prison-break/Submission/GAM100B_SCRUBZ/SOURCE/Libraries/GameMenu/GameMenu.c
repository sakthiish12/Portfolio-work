/*********************************************************************************
GameMenu.c
Author: Chong Jun Yi (Dynamic Gameplay Info)
Code Lines: 55 (20 + 5 + 7 + 23)
Author: Benjamin (Static UI elements and borders)
Code Lines: 81 (6 + 13 + 15 + 22 + 25)
Brief: Add on source code for gameplay UI to game.c, mostly for rendering.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <math.h>							/*roundf*/
#include "GameMenu.h"
#include "../Console/Console.h"				/*Rendering*/
#include "../Global/Global.h"				/*Global Screensize*/
#include "../Clock/Clock.h"					/*for countdown timer*/
#include "../Item/Item.h"					/*Render inventory items*/


/*Notification Data */
Notification notificationIndex[3] = { 0 }; /* Row - text*/
float notificationTime[3] = {-1.0f, -1.0f, -1.0f}; /* Row - time*/
int numOfNotification = 3;
int lastNotification = -1;
Notification gameNotificationList[15] = {
	{"Press SPACE to skip and disable TUTORIAL", {'S', 'P', 'A', 'C', 'E'}, LIGHTGREEN, 6},
	{"Look for the prison level exits ", {236, 236, 236, 236, 236, 236, 236, 236}, LIGHTGREEN, 32},
	{"A          door has opened", {0}, 0, 2},
	{"A          door has closed", {0}, 0, 2},
	{"The 'LVLKey' is required to unlock level exit", {'L','V','L','K','e','y'}, LIGHTGREEN, 5},
	{"You do not have the correct key", {0}, 0, 0},
	{"Guard has spotted you", {0}, 0, 0},
	{"Guard is looking for you", {0}, 0, 0},
	{"Guard has lost sight of you", {0}, 0, 0},
	{"Guard is investigating rock", {0}, 0, 0},
	{"Guard is investigating your position", {0}, 0, 0},
	{"You took too long, the guards realised your position", {0}, 0, 0},
	{"Watch out for   guards that can shoot bullets", {234}, LIGHTRED, 14},
	{"Watch out for   guards that can see behind", {157}, LIGHTRED, 14},
	{"Your item fell out, go back and find it", {0}, 0, 0}
};

/*********************************************************************************
/								RENDER UI BORDERS
/*********************************************************************************

/*********************************************************************************
GameMenu_PlayAreaBorder()
Author: Benjamin
Code Lines: 13
Brief: Draw BORDERS for the gameplay map
**********************************************************************************/
/*Draw the main game screen BORDERS*/
void GameMenu_PlayAreaBorder()
{
	int i = 0;

	Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
	/*Corners*/
	Console_SetRenderBuffer_Char(0, 0, 219);								/* 219 is ASCII character	*/
	Console_SetRenderBuffer_Char(0, Global_playArea.Y, 219);				/* for the walls			*/
	Console_SetRenderBuffer_Char(Global_playArea.X, 0, 219);
	Console_SetRenderBuffer_Char(Global_playArea.X, Global_playArea.Y, 219);
	for (i = 1; i < Global_playArea.X; ++i)
	{
		Console_SetRenderBuffer_Char(i, 0, 219);					//top wall
		Console_SetRenderBuffer_Char(i, Global_playArea.Y, 219);	//bottom wall
	}
	for (i = 1; i < Global_playArea.Y; ++i)
	{
		Console_SetRenderBuffer_Char(0, i, 219);					//left wall
		Console_SetRenderBuffer_Char(Global_playArea.X, i, 219);	//right wall
	}
}

/*********************************************************************************
GameMenu_InventoryBorder()
Author: Benjamin
Code Lines: 15
Brief: Draw the inventory screen BORDERS
**********************************************************************************/
void GameMenu_InventoryBorder(int length, int width)
{
	int i = 0;

	Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);	
					
	/*Corners*/
	Console_SetRenderBuffer_Char(Global_playArea.X + 1, 0, 218);		/*Top Left*/	/*Dash ascii char*/
	Console_SetRenderBuffer_Char(Global_screenSize.X - 2, 0, 191);		/*Top Right*/
	Console_SetRenderBuffer_Char(Global_playArea.X + 1, length, 192);	/*Bottom Left*/
	Console_SetRenderBuffer_Char(Global_screenSize.X - 2, length, 217);	/*Bottom Right*/
	{
		for (i = 0; i < length - 1; ++i)  /*Printing of the border*/
		{
			Console_SetRenderBuffer_Char(Global_playArea.X + 1, i + 1, '|');		//Left
			Console_SetRenderBuffer_Char(Global_screenSize.X - 2, i + 1, '|');		//Right
		}
		for (i = 0; i < width; ++i)  /*Printing of the border*/
		{
			Console_SetRenderBuffer_Char(i + Global_playArea.X + 2, 0, '-');		//Top
			Console_SetRenderBuffer_Char(i + Global_playArea.X + 2, Global_screenSize.Y - 15, '-'); //Inventory Bottom
			Console_SetRenderBuffer_Char(i + Global_playArea.X + 2, 2, '-');		//Line seperating time from inventory
			Console_SetRenderBuffer_Char(i + Global_playArea.X + 2, Global_playArea.Y, '-');		//Textbox Bottom
		}
	}
}

/*********************************************************************************
GameMenu_InventoryDisplayBox()
Author: Benjamin
Code Lines: 22
Brief: Display the boxes for inventory items
**********************************************************************************/
void GameMenu_InventoryDisplayBox(int height, int width, int row, int col)
{
	int r = 0, c = 0;/*For Loop*/
	//int w = 0, h = 0;
	COORD boxOrig = {0,0};
	
	boxOrig.X = (short)Global_playArea.X + 3;
	boxOrig.Y = (short)3 ;

	Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
	for (r = 0; r < row; ++r) {
		for (c = 0; c < col; ++c) {
			/*Draw the box*/
			/*Corners ONLY STYLE*/
			Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(boxOrig.X, boxOrig.Y, 218);					/*Top Left*/
			Console_SetRenderBuffer_Char(boxOrig.X + width, boxOrig.Y, 191);			/*Top Right*/
			Console_SetRenderBuffer_Char(boxOrig.X, boxOrig.Y + height, 192);			/*Bottom Left*/
			Console_SetRenderBuffer_Char(boxOrig.X + width, boxOrig.Y + height, 217);	/*Bottom Right*/
			GameMenu_RenderInventoryItems(boxOrig.X+1, boxOrig.Y+1, (r*col)+c);

			///*Full border style*/
			//for (w = 0; w < width; ++w) {
			//	for (h = 0; h < height; ++h) {
			//		Console_SetRenderBuffer_Char(boxOrig.X, boxOrig.Y + h+1, '|'); //Left
			//		Console_SetRenderBuffer_Char(boxOrig.X + width, boxOrig.Y + h+1, '|'); //right
			//		Console_SetRenderBuffer_Char(boxOrig.X + w+1, boxOrig.Y, '-');  //Top
			//		Console_SetRenderBuffer_Char(boxOrig.X + w+1, boxOrig.Y + height, '-');  //Bottom
			//	}
			//}
			boxOrig.X += (short)(width + 3);
		}
		boxOrig.X -= (short)((width + 3) * col);
		boxOrig.Y += (short) (height + 2);
	}
}

/*********************************************************************************
/								Game Menu Logic Elements
/*********************************************************************************
/*********************************************************************************
GameMenu_TimeUI()
Author: Benjamin
Code Lines: 25
Brief: Perform calculation for time, and display ALARM Countdown timer
**********************************************************************************/
void GameMenu_TimeUI(float time, float lvlTimeLimit) 
{
	char timeDisplay[8]; /*Max of 8 char to display time*/

	if (time >= lvlTimeLimit)
	{
		/*Flashing Effect*/
		if ((int)time % 2 == 0)
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		else
			Console_SetRenderBuffer_Attribute(YELLOW, BLACK, DEFAULTLVB);
	}
	else
	{
		Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
	}

	/*Time title*/
	Console_SetRenderBuffer_String(Global_playArea.X + 3, 1, "Time: ");
	if (time < lvlTimeLimit)
	{
		_itoa_s((int)(lvlTimeLimit - time), timeDisplay, 8, 10); /*can set other timer by changing lvlTimeLimit*/
		Console_SetRenderBuffer_String(Global_playArea.X + 9, 1, timeDisplay);
	}
	else if (time >= lvlTimeLimit)
	{
		_itoa_s((int)(time - lvlTimeLimit), timeDisplay, 8, 10); /*timer will count up from 0*/
		Console_SetRenderBuffer_String(Global_playArea.X + 9, 1, timeDisplay);
	}
}

/*********************************************************************************
/						RENDER DYNAMIC GAMEPLAY INFO
/*********************************************************************************
/*********************************************************************************
GameMenu_Textbox()
Author: Chong Jun Yi
Code Lines: 5
Brief: Display any ITEM textual info - Nearby items, Item desciption
**********************************************************************************/
void GameMenu_Textbox(char* string, char* string2, char* string3, char* string4)
{
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	/*Nearby Item name*/
	Console_SetRenderBuffer_String(35, Global_screenSize.Y - 14, string);
	/*3 lines for item desciption*/
	Console_SetRenderBuffer_String(35, Global_screenSize.Y - 12, string2);
	Console_SetRenderBuffer_String(35, Global_screenSize.Y - 11, string3);
	Console_SetRenderBuffer_String(35, Global_screenSize.Y - 10, string4);
}

/*********************************************************************************
GameMenu_RenderInventoryItems()
Author: Chong Jun Yi
Code Lines: 7
Brief: Render any inventory item name within a constraint box space
**********************************************************************************/
void GameMenu_RenderInventoryItems(int x, int y, int slot)
{
	int maxWidth = 3, maxRow = 2, i = 0, j = 0;

	if (activeInventoryCount <= slot) { /*If slot is out of range, terminate function*/
		return;
	}

	/*Display inventory item name in its respective color code*/
	Console_SetRenderBuffer_Attribute(activeInventoryList[slot].usageCode, BLACK, DEFAULTLVB);
	for (j = 0; j < maxRow; ++j) {
		for (i = 0; i < maxWidth; ++i) { /*Make sure the word overflow properly*/
			Console_SetRenderBuffer_Char(x+i, y+j, activeInventoryList[slot].name[j*maxWidth+i]);
		}
	}
}
/*********************************************************************************
GameMenu_NotificationBox()
Author: Chong Jun Yi
Code Lines: 23
Brief: Display any noteworthy gameplay events, player or AI actions to reduce confusion
**********************************************************************************/
void GameMenu_NotificationBox() {
	int w = 0, h = 0, i = 0;
	//COORD notificationSize = {30,5};
	int length = 0;

	/*Notification Border*/
	Console_SetRenderBuffer_Attribute(BLACK, RED, DEFAULTLVB);
	for (h = Global_playArea.Y + 1; h < Global_screenSize.Y; ++h) {
		Console_SetRenderBuffer_Char(0, h, 10);
		Console_SetRenderBuffer_Char(Global_screenSize.X-2, h, 10);
	}
	for (w =1; w < Global_screenSize.X-2; ++w) {
		Console_SetRenderBuffer_Char(w, Global_playArea.Y + 1, 10);
		Console_SetRenderBuffer_Char(w, Global_screenSize.Y-1, 10);
	}
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(4, Global_playArea.Y + 1, "Notification");

	/*Actual notification message*/
	for (i = 0; i < numOfNotification; ++i) {
		length = (int)strlen(notificationIndex[i].text);
		if (length != 0) {
			/*Print notification*/
			for (w = 0; w < length && w < (int)roundf(notificationTime[i]); ++w) {
				Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, DEFAULTLVB);
				Console_SetRenderBuffer_Char(2+w, Global_playArea.Y + 2+i, notificationIndex[i].text[w]);
			}
			/*Print out format*/
			for (w = 0; w < 8 && notificationIndex[i].format1[w] != 0; ++w ) { 
				if (w + notificationIndex[i].format1_Pos <= (int)roundf(notificationTime[i])) { /*Typing effect*/
					Console_SetRenderBuffer_Attribute(notificationIndex[i].format1_Color, BLACK, DEFAULTLVB);
					Console_SetRenderBuffer_Char(2 + w + notificationIndex[i].format1_Pos, Global_playArea.Y + 2 + i,
						(char)(notificationIndex[i].format1[w]));
				}
			}
		}
	}
}

