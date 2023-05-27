/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef GAMEMENU_H
#define GAMEMENU_H

/*NOTIFICATION STRUCT - text, format, color, pos*/
typedef struct NOTIFICATION {
	char text[54];
	int format1[8];
	int format1_Color;
	int format1_Pos;
}Notification;

/*Notification List that displays on screen*/
Notification notificationIndex[3]; /* Row - text*/
float notificationTime[3]; /* The remaining time before notification disappear*/
int numOfNotification; /*Current active notification*/
int lastNotification; /*Last notification text, so no overlaps*/
Notification gameNotificationList[15]; /*Standard template for notification designed for various moments*/

/*Draw the main game screen BORDERS*/
void GameMenu_PlayAreaBorder();

/*Draw the inventory screen BORDERS*/
void GameMenu_InventoryBorder(int length, int width);

/*Display the boxes for inventory items*/
void GameMenu_InventoryDisplayBox(int height, int width, int row, int col);

/*Perform calculation for time to display on Countdown timer*/
void GameMenu_TimeUI(float time, float lvlTimeLimit);

/*Display any textual info - Nearby items, use item on level, AI changes*/
void GameMenu_Textbox(char* string, char* string2, char* string3, char* string4);

/*Item added into inventory*/
void GameMenu_RenderInventoryItems(int x, int y, int slot);

/*Display any noteworthy gameplay events, player or AI actions to reduce confusion*/
void GameMenu_NotificationBox();
#endif // !GAMEMENU_H

