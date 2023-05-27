/*********************************************************************************
Tutorial.c
Author: Chong Jun Yi (Tutorial stages logic)
Code Lines: 150
Author: Sakthiish (Game items, Graphics)
Code Lines: 50
Brief: Add on logic/rendering for tutorial phase of main game
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <string.h> /*strcpy*/
#include <math.h> /*roundf*/
#include "Tutorial.h"
#include "../Global/Global.h"	
#include "../Console/Console.h"	/*Rendering*/
#include "../Item/Item.h"		/*Thief spawn*/
#include "../Clock/Clock.h"		/*delta Time Movement*/
#include "../Guard/Guard.h"		/*Spawn a guard to chase player*/
#include "../TxtFileReader/TxtFileReader.h"
#include "../Map/Map.h"			/*local map offset*/
#include "../States/Game.h"		/*Notification*/

int tutorialStages = 0;			/*Different teaching stages of games*/
/*stage 0 -> stage 1: Picked up peanuts*/
/*stage 1 -> stage 2: Pay at cashier (linked with item.c)*/
/*stage 2 -> stage 3: Stepped into exit trigger*/
/*stage 3 -> stage 4: When Thief exit level*/
/*stage 4 -> stage 5: When Police catch player*/
/*stage 5 -> stage 0: Spacebar after tutorial end page*/
int bDisableTutorial = 0;		/*Skilled players can skip the tutorial*/

/*Display tutorial text*/
char textTutTips[20] = "";
char textTutTips2[20] = "";
char* midTextPtr = NULL;		/*Split the tutorial in to 2 display line*/

/*Thief movement - not using Astar*/
COORD thiefPath[5] = {
	{23,28},
	{17,28},
	{17,32},
	{14,32},
	{14,42}
};
int thiefPathIndex = 0;

/*Trigger zones to show tutorial text*/
struct TUTORIALTRIGGER tutorialTriggerList[7] = {
	{"Press WASD to move around", {8,33}, {18,36}},
	{"Press E to pick up items", {7,24}, {30,32}},
	{"Head to the cashier", {0,0}, {0,0}},
	{"Press E to pay for peanuts", {0,0}, {0,0}},
	{"Return to the minimart exit", {0,0}, {0,0}},
	{"Hey! That thief stole something!", {0,0}, {0,0}},
	{"You must be the thief!", {0,0}, {0,0}},
};

COORD minimartExitOrig = {10,33};
COORD minimartExitEnd = {15,33};
int minimartExitState = 0;

/*Minimart advertisement*/
char mmAdvertisment1[50] = "    THERE IS NO EXIT BACK HERE -.-    ";
COORD mmAdvert1Orig = { 14, 11}; //26 length
float mmAdvertLeftOffset1 = 0.0f;
int mmAdvertSpeed1 = 10;

void TutorialEnterState() {
	CreateNotification(0, 0, -1);
}

void TutorialReset() {
	tutorialStages = 0;
	thiefPathIndex = 0;
}

/*Logic update for every frame in tutorial mode*/
void TutorialUpdates(COORD playerPos)
{

	/*Clear the tutorial display*/
	strncpy_s(textTutTips, 20, "", 0);
	strncpy_s(textTutTips2, 20, "", 0); 

	/*****Actively monitor for tutorial stage specific events to happen/*****/
	if (tutorialStages == 0) {
		/*If player has picked up peanuts*/
		if (strcmp(activeInventoryList[0].name, "Peanut") == 0) {
			DisableTutorialTrigger(1);
			EnableTutorialTrigger(2, 8, 13, 25, 30);
			EnableTutorialTrigger(3, 7, 13, 8, 23);
			OnTutorialStageChange(1);
		}
	}
	else if (tutorialStages == 3) {
		/*Cutscene - Theft and player falsely accues*/
		for (int i = 0; i < activeItemCount; ++i) {
			if (strcmp(activeItemList[i].name, "Human") == 0){
				/*Make the thief run away*/
				Tutorial_ThiefMovement(&activeItemList[i], i, playerPos);
			}
		}
	}

	/*Check if player has reached tutorial trigger*/
	for (int i = 0; i < sizeof(tutorialTriggerList)/ sizeof(tutorialTriggerList[0]); ++i)
	{
		if (CheckZoneCollision(playerPos.X, playerPos.Y, tutorialTriggerList[i].origZone, tutorialTriggerList[i].endZone)) 
		{
			/*Collision when leaving the shop*/
			if (i == 5 && tutorialStages == 2) {
				/*Trigger tutorial stages - thief running out*/
				OnTutorialStageChange(3);
			}

			/***** Prepare Text if player is on tutorial trigger *****/
			LoadTutorialText(i);
		}
	}

	ToggleMinimartExitEntry(playerPos);
	ShopAdvertisementLogic();
	
}

void TutorialRender() {
	int y = 10; /*Start y coordinate for tutorial end screen*/

	if (tutorialStages == 5) {
		Console_ClearRenderBuffer(); /*Clear the screen of gameplay stuff*/
		/*=============================================================================*/
		/*                        End Of Tutorial Screen								   */
		/*=============================================================================*/

		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(20, y++, "END OF TUTORIAL");
		Console_SetRenderBuffer_String(10, y++, "************************************");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(7, y++, "You are wrongly arrested and sent to prison");
		Console_SetRenderBuffer_String(0, y++, "");
		Console_SetRenderBuffer_String(10, y++, "************************************");

		/*=============================================================================*/
		/*                    INSTRUCTIONS TO GO BACK TO MENU                          */
		/*=============================================================================*/
		Console_SetRenderBuffer_String(10, y, "Press");
		Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(16, y, "<SPACE>");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(24, y++, "to start your escape");
		Console_SwapRenderBuffer();
		return;
	}
	else {
		/***** Render Text if player is on tutorial trigger *****/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(35, Global_screenSize.Y - 12, textTutTips);
		Console_SetRenderBuffer_String(35, Global_screenSize.Y - 11, textTutTips2);
		ShopAdvertisementOverlay();
	}
	
}

void Tutorial_ThiefMovement(struct ITEM *thief, int itemIndex, COORD playerPos) {
	int distX = 0, distY = 0;
	int speed = 7;

	/*Movement to waypoints*/
	distX = thiefPath[thiefPathIndex].X - thief->pos.X;
	distY = thiefPath[thiefPathIndex].Y - thief->pos.Y;
	if (distX != 0) {
		thief->fpos.X += (distX / abs(distX)) * Clock_GetDeltaTime() * 0.001f * speed;
	}
	if (distY != 0) {
		thief->fpos.Y += (distY / abs(distY)) * Clock_GetDeltaTime() * 0.001f * speed;
	}
	/*Update Display position*/
	thief->pos.X = (short)roundf(thief->fpos.X);
	thief->pos.Y = (short)roundf(thief->fpos.Y);

	/*If thief reach way points*/
	if (thief->pos.X == thiefPath[thiefPathIndex].X && thief->pos.Y == thiefPath[thiefPathIndex].Y) {
		++thiefPathIndex;

		/*If thief reach destination*/
		if (thiefPathIndex >= 5) {
			OnTutorialStageChange(4);
			/*Despawn the theif*/
			Item_Despawn(itemIndex);
			/*Spawn the police to catch player - falsely accused*/
			Spawn_Guard1(30, 28, playerPos.X, playerPos.Y, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1);
			activeGuardList[0].moveSpeed = 8; /*Cant really override here...its based on a statemachine*/
		}
	}
}


void OnTutorialStageChange(int newStage) {
	/*Any specific actions before changing to next stage*/
	if (newStage == 2) {
		/*After interact with cashier, change text trigger before going to next one*/
		DisableTutorialTrigger(2);
		DisableTutorialTrigger(3);
		EnableTutorialTrigger(4, 8, 14, 35, 30);
		EnableTutorialTrigger(5, 10, 30, 20, 30);
	}
	else if (newStage == 3) {
		/*Lock the minimart*/
		for (int j = 0; j < activeItemCount; ++j) {
			if (strcmp(activeItemList[j].name, "Mini Mart") == 0) {
				activeItemList[j].usageCode = LIGHTRED;
			}
		}
		DisableTutorialTrigger(4);
	}
	else if (newStage == 4) {
		/*Police comes*/
		EnableTutorialTrigger(6, 10, 30, 20, 30);
	}
	tutorialStages = newStage;
}

/*Enable the tutorial trigger by movings its zone coordinate to accessible areas*/
void EnableTutorialTrigger(int triggerIndex, short zoneOrigX, short zoneOrigY, short zoneEndX, short zoneEndY) {
	tutorialTriggerList[triggerIndex].origZone.X = zoneOrigX;
	tutorialTriggerList[triggerIndex].origZone.Y = zoneOrigY;
	tutorialTriggerList[triggerIndex].endZone.X = zoneEndX;
	tutorialTriggerList[triggerIndex].endZone.Y = zoneEndY;
}

/* Disable by moving its collision zone out of player reach*/
void DisableTutorialTrigger(int triggerIndex) {
	tutorialTriggerList[triggerIndex].origZone.X = 0;  
	tutorialTriggerList[triggerIndex].origZone.Y = 0;  
	tutorialTriggerList[triggerIndex].endZone.X = 0;  
	tutorialTriggerList[triggerIndex].endZone.Y = 0;   
}

/*When player interact with cashier with peanuts*/
void Tutorial_CashierEvent() {
	if (tutorialStages == 1) {
		OnTutorialStageChange(2);
	}
}

/*Place the string in display array, split into 2 string if necessary*/
void LoadTutorialText(int index) {
	int j = 0; /*for loop*/
	int lengthStr = 0;

	lengthStr = (int)strlen(tutorialTriggerList[index].text);
	/*Find out where to split the text into 2 lines*/
	for (j = 19; j > 0; --j) {
		if (tutorialTriggerList[index].text[j] == ' ') {
			break;
		}
	}
	midTextPtr = &tutorialTriggerList[index].text[j + 1];
	strncpy_s(textTutTips, 19, tutorialTriggerList[index].text, j);
	strncpy_s(textTutTips2, 19, midTextPtr, lengthStr - (__int64)j);
}

/*Toggle the arrow of minimart exit and entry whether player is inside store or outside*/
/*0 = Entry unstep, 1 = Entry step, 2 = Exit unstep, 3 = Exit step*/
void ToggleMinimartExitEntry(COORD playerPos) {
	int j = 0;

	/*If player is stepping on it*/
	if (CheckZoneCollision(playerPos.X, playerPos.Y, minimartExitOrig, minimartExitEnd)) {
		if (minimartExitState == 0) {
			minimartExitState = 1;
		}
		else if (minimartExitState == 2) {
			minimartExitState = 3;
		}
	}
	else {
		if (minimartExitState == 1) {
			for (j = 0; j < activeItemCount; ++j) {
				if (strcmp(activeItemList[j].name, "Mini Mart") == 0) {
					activeItemList[j].graphicAdd = &itemGraphicData[9][0][0][0];
					activeItemList[j].numOfUsage = 2;
				}
			}
			minimartExitState = 2;
		}
		else if (minimartExitState == 3) {
			for (j = 0; j < activeItemCount; ++j) {
				if (strcmp(activeItemList[j].name, "Mini Mart") == 0) {
					activeItemList[j].graphicAdd = &itemGraphicData[8][0][0][0];
					activeItemList[j].numOfUsage = 0;
				}
			}
			minimartExitState = 0;
		}

	}
}

/*Offset the advertisement to the left*/
void ShopAdvertisementLogic() {
	mmAdvertLeftOffset1 += Clock_GetDeltaTime() * 0.001f * mmAdvertSpeed1;
	if (mmAdvertLeftOffset1 > 41.0f) {
		mmAdvertLeftOffset1 = 0.0f;
	}
}

/*Render the shop advertisement over the game level*/
void ShopAdvertisementOverlay() {
	int mmadvert1Length = (int)strlen(mmAdvertisment1);
	int movingIndex = 0;

	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	for (int i = 0; i < mmadvert1Length; ++i) {
		movingIndex = i - (int)roundf(mmAdvertLeftOffset1);
		if (movingIndex < -5) {
			movingIndex += mmadvert1Length;
		}
		if (movingIndex >= 0) {
			if (WithinPlayArea(mmAdvert1Orig.X + movingIndex, mmAdvert1Orig.Y, 1, -1)) {
				/*if (movingIndex < -mmadvert1Length) {
					movingIndex = (movingIndex * -1) - mmadvert1Length;
				}*/
				if (movingIndex < 26) {
					Console_SetRenderBuffer_Char(mmAdvert1Orig.X + movingIndex - localMapOffset.X, mmAdvert1Orig.Y - localMapOffset.Y, mmAdvertisment1[i]);
				}
				
			}
		}
	}
}