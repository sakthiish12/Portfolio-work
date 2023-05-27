/*********************************************************************************
Item.c
Author: Choong Jun Yi (Tutorial Items, Shooting, Lasers)
Code Lines: 340 (Approx)
Author: Sakthiish (Graphics, Door, Keys, Items)
Code Lines: 340 (Approx)
Code Lines: 680 (16+(15*13)+11+12+4+2+14+4+80+7+4+3+4+3+5+5+4+44+28+27+26+9+10+38+13+11+14+5+22+13+15+32)
Brief: Item functionality in the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <windows.h>/*Coord*/
#include <math.h> /*Round*/
#include "Item.h"
#include "../Global/Global.h"
#include "../Human/Human.h"
#include "../TxtFileReader/TxtFileReader.h"
#include "../Console/Console.h"
#include "../GameMenu/GameMenu.h"
#include "../States/Game.h"
#include "../Clock/Clock.h"
#include "../Map/Map.h"
#include "../Tutorial/Tutorial.h" /*Cashier interaction*/
#include "../DifficultySystem/DifficultySystem.h"
#include "../Guard/Guard.h"			/*stepping on Laser trigger alarms*/
#include "../States/GameOver.h"
#include "../Sounds/GameSounds.h"

/**
*\Members: "char[16] name", "COORD pos", "Graphic array
*/
/*List all our items here*/

/*Items in game*/
Item Item_Char =
{
	"BLANK",			/*Name of Item: */
	"BLANK",			/*Item Description*/
	0,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0,0},				/*current float position*/
	interact_passive,	/*Interact Type*/
	99,					/*Num of usage*/
	0					/*Usage Code, for door and key*/
};

/*Key that can open doors*/
Item Item_Key =
{
	"KEY",				/*Name of Item: */
	"Used to open lock doors",			/*Item Description*/
	11,				/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{3,4},				/*Positon*/
	{0,0},				/*current float position*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	1					/*Usage Code, for door and key*/
};

/*Door that cannot be opened*/
Item Item_DoorStatic =
{
	"Sealed Door",		/*Name of Item: */
	"This door cannot be unlocked", /*Item Description*/
	0,					/*The index from graphics array*/
	{3,3},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0,0},				/*current float position*/
	interact_active,	/*Interact Type*/
	0,					/*Num of usage*/
	11					/*Usage Code, for door and key*/
};

/*Doors that can be opened with key*/
Item Item_DoorUnlock =
{
	"Door",				/*Name of Item: */
	"Use Key or Switch to unlock door",			/*Item Description*/
	0,					/*The index from graphics array*/
	{3,3},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0,0},				/*current float position*/
	interact_active,	/*Interact Type*/
	99,					/*Num of usage*/
	0					/*Usage Code, for door and key*/
};

/*Level key to go to the next level*/
Item Item_LevelKey =
{
	"LVLKey",			/*Name of Item: */
	"Obtain to walk pass level bounds",			/*Item Description*/
	168,				/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{3,4},				/*Positon*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	1					/*Usage Code, for door and key*/
};

Item Item_Switch =
{
	"Switch",			/*Name of Item: */
	"Open/Close a same color door",			/*Item Description*/
	219,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{3,4},				/*Positon*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	1					/*Usage Code, for door and key*/
};

Item Item_Bullet =
{
	"Bullet",			/*Name of Item: */
	"Bullet kills",		/*Item Description*/
	4,					/*The ascii symbol for bullet*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},				/*In this case, the fired point and the end point*/
	{0,0},				/*Positon*/
	{0,0},				/*current float position*/
	interact_passive,	/*Interact Type*/
	7,					/*Num of usage - Speed*/
	7					/*Usage Code, Color for bullet*/
};

Item Item_TutShopEntry =
{
	"Mini Mart",		/*Name of Item: */
	"",					/*Item Descript*/
	0,					/*The ascii symbol for bullet*/
	{2,2},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},				/*In this case, the fired point and the end point*/
	{0,0},				/*Positon*/
	{0,0},				/*current float position*/
	interact_passive,	/*Interact Type*/
	0,					/*Check if steped over: 0 = Entry unstep, 1 = Entry step, 2 = Exit unstep, 3 = Exit step*/
	0					/*Usage Code, Color for bullet*/
};

Item Item_Peanut =
{
	"Peanut",			/*Name of Item: */
	"",					/*Item Descript*/
	235,				/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{20,24},			/*Start and End of Zone*/
	{20,24},
	{20,24},			/*Positon*/
	{0,0},				/*current float position*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	0					/*Usage Code, for door and key*/
};

/*This is a zone item without graphics*/
Item Item_Cashier = 
{
	"Cashier",			/*Name of Item: */
	"",					/*Item Description*/
	0,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{5,14},				/*Start and End of Zone*/
	{7,22},
	{5,14},				/*Positon*/
	{0, 0},				/*current float position*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	0					/*Usage Code, for door and key*/
};

Item Item_Thief =
{
	"Human",			/*Name of Item: */
	"",					/*Item Description*/
	6,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0, 0},				/*current float position*/
	interact_passive,	/*Interact Type*/
	99,					/*Num of usage*/
	0					/*Usage Code, for door and key*/
};

Item Item_Rock =
{
	"Rock",				/*Name of Item: */
	"'Q' To Throw and distract guard",			/*Item Description*/
	35,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0, 0},				/*current float position*/
	interact_passive,	/*Interact Type*/
	4,					/*Speed for rock*/
	9					/*Color for rock */
};

Item Item_Sound =
{
	"Sound",			/*Name of Item: */
	"",					/*Item Description*/
	42,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0, 0},				/*Countdown in FPOS - actual is X, display is Y*/
	interact_passive,	/*Interact Type*/
	0,					/*Radius*/
	9					/*Color*/
};

Item Item_AlarmLaser =
{
	"Alarm Laser",				/*Name of Item: */
	"Crossing this laser will alert guards",			/*Item Description*/
	58,					/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{0,0},				/*Positon*/
	{0, 2},				/*Duration for on off laser - startTime/MaxTime*/
	interact_passive,	/*Interact Type*/
	0,					/*Tempo of laser*/
	LIGHTRED			/*Color for NoiseTile */
};

Item Item_EasterEgg =
{
	"Newspaper",				/*Name of Item: */
	"Will SCRUBS pass GAM100 module?",			/*Item Description*/
	11,				/*The index from graphics array*/
	{1,1},				/*Size of graphic w x h*/
	0,					/*Location of graphic - NULL pointer*/
	{0,0},				/*Start and End of Zone*/
	{0,0},
	{3,4},				/*Positon*/
	{0,0},				/*current float position*/
	interact_active,	/*Interact Type*/
	1,					/*Num of usage*/
	1					/*Usage Code, for door and key*/
};

/*Can use dynamic memory allocation later*/
Item activeItemList[50] = { "", "", 0, {0,0} };
int activeItemCount = 0;

/*Can use dynamic memory allocation later*/
Item activeInventoryList[6] = { "", "",0, {0,0} };
int activeInventoryCount = 0;

Item activeProjectileList[10] = { "", "",0, {0,0} };
int activeProjectileCount = 0;

Item activeSoundList[6] = { "", "", 0, {0,0} };
int activeSoundCount = 0;

bool toggleItemMalfunction = 0;
int randNumCheckItem1;
int randNumCheckItem2;
/*********************************************************************************
/								Initalisation Functions
/********************************************************************************/

/*Spawn item on map and make them active*/
void Spawn_Item(Item *item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int usageC)
{
	item->pos.X = x;
	item->pos.Y = y;
	item->fpos.X = (float)x;
	item->fpos.Y = (float)y;
	item->zoneOrig.X = xOrig;
	item->zoneOrig.Y = yOrig;
	item->zoneEnd.X = xEnd;
	item->zoneEnd.Y = yEnd;
	item->usageCode = usageC;
	activeItemList[activeItemCount] = *item;
	++activeItemCount;
}

/*Mainly to change item within same family - Door and static door*/
void Change_Item(Item* oldItem, Item* newItem) {
	strcpy_s(oldItem->name, 18, newItem->name);
	strcpy_s(oldItem->itemDescrip, 57, newItem->itemDescrip);
	oldItem->numOfUsage = newItem->numOfUsage;
}

/*Checking it item is collided with*/
int Item_CheckCollision()
{
	/*!TakeNote Unfinished and alternative found in map*/
	return 1;
}

/*Return the name of first item in front or overlaping player*/
/*Can only detect 1 item if both position contains item*/
void Item_InteractPassive(int xSelf, int ySelf, char buffer[11], int *itemIndex)
{	
	int i = 0;
	int w = 0, h = 0;
	//char string[10];// = (char*)malloc(sizeof(char) * 10);

	strcpy_s(buffer, 18, ""); //string = "Not Active";
	*itemIndex = -1;

	for (i = 0; i < activeItemCount; ++i)
	{   
		/*Check 8 Squares around player*/
		for (w = -1; w <= 1; ++w) {
			for (h = -1; h <= 1; ++h) {
				if (CheckZoneCollision(xSelf + w, ySelf + h, activeItemList[i].zoneOrig, activeItemList[i].zoneEnd)) {
					strcpy_s(buffer, 18, activeItemList[i].name);
					*itemIndex = i;
					/*Player is directly on top of item*/
					if (CheckZoneCollision(xSelf, ySelf, activeItemList[i].zoneOrig, activeItemList[i].zoneEnd)) {
						FurtherPassiveInteraction(&activeItemList[i]);
					}
					return;
				}
				
			}
		}

		/*OLD VERSION BASED ON FACING DIRECTION*/
		/*if ((activeItemList[i].zoneOrig.X <= x && activeItemList[i].zoneOrig.Y <= y
			&& activeItemList[i].zoneEnd.X >= x && activeItemList[i].zoneEnd.Y >= y)	
			||(activeItemList[i].zoneOrig.X <= xSelf && activeItemList[i].zoneOrig.Y <= ySelf
			&& activeItemList[i].zoneEnd.X >= xSelf && activeItemList[i].zoneEnd.Y >= ySelf))
		{
			strcpy_s(buffer,11, activeItemList[i].name);
			return;
		}*/
	}
}

void FurtherPassiveInteraction(struct ITEM *item) {
	if (strcmp(item->name, "Alarm Laser") == 0) {
		/*Trigger Alarm and inform nearby guards of player position*/
		if (item->fpos.X / item->fpos.Y > 0.0f) {	/*laser is on*/
			PrisonAlarmEvent(100, 1);
		}
		
	}
}


/*Press E to interact with the item*/
void Item_InteractActive(int xSelf, int ySelf)
{
	int i = 0, j = 0;/*for loop*/
	int w = 0, h = 0;
	/*for switch notification*/
	int switchResult = 0;
	/*!TakeNote should know whether we want to interact or pickup -- see item's interact type*/

	/*INTERACT*/
	for (i = 0; i < activeItemCount; ++i)
	{
		/*Check all 8 directions*/
		for (w = -1; w <= 1; ++w) {
			for (h = -1; h <= 1; ++h) {
				/*Check if any item is being interacted with now*/
				if (activeItemList[i].zoneOrig.X <= xSelf+w && activeItemList[i].zoneOrig.Y <= ySelf+h && activeItemList[i].zoneEnd.X >= xSelf + w && activeItemList[i].zoneEnd.Y >= ySelf+h)
					/*activeItemList[i].zoneOrig.X <= x && activeItemList[i].zoneOrig.Y <= y && activeItemList[i].zoneEnd.X >= x && activeItemList[i].zoneEnd.Y >= y)||*/
				{
					if (strcmp(activeItemList[i].name, "Door") == 0)
					{
						/*Check if inventory has the correct key to open door*/
						for (j = 0; j < activeInventoryCount; ++j) {
							if (activeInventoryCount > 0 && activeInventoryList[j].usageCode == activeItemList[i].usageCode
								&& strcmp(activeInventoryList[j].name, "KEY") == 0) {
								/*Need to see which direction player is opening */
								/*Checking if ItemFunction option is on*/
								if (toggleItemMalfunction == 0)
								{
									Item_OpenCloseDoor(&activeItemList[i]);
									RemoveInventoryItem(j);
									FmodPlayGameSound(2);
									
								}
								else if (toggleItemMalfunction == 1)
								{
									randNumCheckItem1 = rand() % 2;
									randNumCheckItem2 = rand() % 2;
									/*If both number are the same, the Item Breaks and Respawn*/
									if (randNumCheckItem1 == randNumCheckItem2)
									{
										ItemMalfunction(j);
										CreateNotification(14, 0, -1);
									}
									else /*It works as per normal*/
									{
										Item_OpenCloseDoor(&activeItemList[i]);
										RemoveInventoryItem(j);
										FmodPlayGameSound(2);
									}
								}
								return;
							}
						}
						CreateNotification(5, 0, -1);
						return;

					}
					else if (strcmp(activeItemList[i].name, "Switch") == 0) {
						/*Use switch to control doors or lights*/
						/**Find the appropriate door or light to activate*/
						for (j = 0; j < activeItemCount; ++j)
						{
							if (j == i) {
								continue;
							}
							if (activeItemList[i].usageCode == activeItemList[j].usageCode && (strcmp(activeItemList[j].name, "Door") == 0)) {
								switchResult = Item_OpenCloseDoor(&activeItemList[j]);
								CreateNotification(3 - switchResult, 
									colorSymbol[activeItemList[i].usageCode], activeItemList[i].usageCode);
								FmodPlayGameSound(3);
								/*Do not break out as there might be multiple color door*/
							}
						}
						return;
					}
					else if (strcmp(activeItemList[i].name, "Cashier") == 0) {
						Tutorial_CashierEvent();
						return;
					}
					else if (strcmp(activeItemList[i].name, "Human") == 0) {
						return;
					}
					else if (strcmp(activeItemList[i].name, "Mini Mart") == 0) {
						return;
					}
					else if (strcmp(activeItemList[i].name, "Alarm Laser") == 0) {
						return;
					}
					else if (strcmp(activeItemList[i].name, "Newspaper") == 0) {
						return;
					}
				}
			}
		}
	}
	
	/*PICKUP*/
	for (i = 0; i < activeItemCount; ++i)
	{
		/*Check all 8 direction*/
		for (w = -1; w <= 1; ++w) {
			for (h = -1; h <= 1; ++h) {
				/*checking whether blinker and item is same position*/
				if (activeItemList[i].pos.X == xSelf+w && activeItemList[i].pos.Y+h == ySelf) /*(activeItemList[i].pos.X == x && activeItemList[i].pos.Y == y) ||*/
				{
					if (activeInventoryCount < 5) {
						if (strcmp(activeItemList[i].name, "Rock") == 0) {
							bInventoryRock = 1;
						}
						else if (strcmp(activeItemList[i].name, "LVLKey")==0) {
							CreateNotification(1, 0, -1);
						}
						Item_ObtainItem(activeItemList[i], i);				/*Add item to inventory logic*/
						FmodPlayGameSound(1);
					}
				}
			}
		}
	}
}

/*Item Despawn from game map*/
void Item_Despawn(int itemIndex)
{
	activeItemList[itemIndex] = activeItemList[itemIndex+1];
	/*Try to do something like push the array down then minus counts*/
	for (++itemIndex; itemIndex < activeItemCount - 1; ++itemIndex) {
		activeItemList[itemIndex] = activeItemList[itemIndex + 1];
	}
	activeItemList[activeItemCount - 1] = (struct ITEM){ 0 };
	--activeItemCount;
	CheckNearbyItem();
}
void Clear_Map_Item() 
{
	for (int i = 0; i < activeItemCount; ++i) 
	{
		activeItemList[i] = (struct ITEM){ 0 };
	}
	activeItemCount = 0;
	
}
/*Holds all function to perform when an item is recieved */
void Item_ObtainItem(struct ITEM item, int itemIndex)
{
	AddInventoryItems(&item);	/*Store item to player inventory*/
	Item_Despawn(itemIndex);		/*Despawn the item from map*/
}

/*Item added into inventory*/
void AddInventoryItems(struct ITEM *item)
{	
	activeInventoryList[activeInventoryCount] = *item;
	++activeInventoryCount;
}

void RemoveInventoryItem(int itemIndex) {
	/*In case only 1 item in inventory, still need to clear*/
	activeInventoryList[itemIndex] = activeInventoryList[itemIndex+1];
	for (++itemIndex; itemIndex < activeInventoryCount - 1; ++itemIndex) { /*Make sure does not go out of range*/
		activeInventoryList[itemIndex] = activeInventoryList[itemIndex+1];
	}
	--activeInventoryCount;
}

void ItemMalfunction(int itemIndex) {
	/*Return item to its original spot*/
	Spawn_Item(&activeInventoryList[itemIndex], activeInventoryList[itemIndex].pos.X, activeInventoryList[itemIndex].pos.Y,
		activeInventoryList[itemIndex].zoneOrig.X, activeInventoryList[itemIndex].zoneOrig.Y, activeInventoryList[itemIndex].zoneEnd.X,
		activeInventoryList[itemIndex].zoneEnd.Y, activeInventoryList[itemIndex].usageCode);
	/*Transfer the char pointer since release mode doesnt transfer properly*/
	/*Remove the item from inventory*/
	RemoveInventoryItem(itemIndex);
}

void ClearInventory() {
	for (int i = activeInventoryCount - 1; i >= 0; --i) {
		activeInventoryList[i] = (struct ITEM){0};
	}
	activeInventoryCount = 0;
}

/*From our level data, identify which are item spots and intialise those items into the activeItemList*/
/*Only initalise activeItemList here*/
void Item_ScanMapForItems(int curLevel)
{
	short w = 0, h = 0;		/*Local GameScreen*/
	short doorColor = 0;		/*Matching doors and keys*/
	//int itemOrigX = 0, itemOrigY = 0;	/*The orig point of the itemzone*/
	int i = 0;				/*For loop*/
	int bIsSameItem = 0;	/*Used to exit the item loop*/
	int animationFrame = 0;	/*Which sprite to use*/

	/*Sadly map is 240 size now*/
	for (h = 0; h < 240; ++h)
	{
		for (w = 0; w < 240; ++w)
		{
			if (levelGraphicData[curLevel][w][h][2] == WHITE) {	/*Item zone detected*/
				/*Check if an existing spawn item exist, and add this zone to its overall area*/
				if (activeItemCount > 0) 
				{
					bIsSameItem = 0;

					for (i = 0; i < activeItemCount; ++i)
					{
						/*Check if the zone is part of a bigger items*/
						if (w >= activeItemList[i].zoneOrig.X && h >= activeItemList[i].zoneOrig.Y
							&& w <= activeItemList[i].zoneEnd.X && h <= activeItemList[i].zoneEnd.Y) 
						{
							/*Update the color of the big item since its orig could have no color info*/
							if (levelGraphicData[curLevel][w][h][1] != BLACK) 
							{
								activeItemList[i].usageCode = levelGraphicData[curLevel][w][h][1];
								if (levelGraphicData[curLevel][w][h][1] == LIGHTCYAN) { /*Update door to static door*/
									Change_Item(&activeItemList[i], &Item_DoorStatic);
								}								
							}
							bIsSameItem = 1;
							break;
						}
					}
					if (bIsSameItem) 
					{
						continue; /*Pixel grid is part of a existing big item, continue to scan level data for new items*/
					}	
				}

				/*Found a New item origin*/
				animationFrame = levelGraphicData[curLevel][w][h][0] % 16;
	
				switch (levelGraphicData[curLevel][w][h][0] / 16)/*Check which item based on hex code*/
				{
					case 0xD: /*Doors*/
						doorColor = (short)levelGraphicData[curLevel][w][h][1];
						/*Static doors*/
						if (doorColor == LIGHTCYAN)
						{
							Item_DoorStatic.graphicIndex = (char)animationFrame;
							Item_DoorStatic.graphicAdd = &itemGraphicData[animationFrame][0][0][0];
							Spawn_Item(&Item_DoorStatic, w, h, w, h, w + Item_DoorStatic.graphicSize.X-1, h + Item_DoorStatic.graphicSize.Y - 1, doorColor);
						}
						/*Unlockable doors*/
						else
						{
							Item_DoorUnlock.graphicIndex = (char)animationFrame;
							if (animationFrame == 2 || animationFrame == 3 || animationFrame == 6 || animationFrame == 7) {
								strcpy_s(Item_DoorUnlock.itemDescrip, 57, "Door is opened");
							}
							Item_DoorUnlock.graphicAdd = &itemGraphicData[animationFrame][0][0][0];
							Spawn_Item(&Item_DoorUnlock, w, h, w, h, w+ Item_DoorUnlock.graphicSize.X - 1, h+ Item_DoorUnlock.graphicSize.Y - 1, doorColor);							
							/*Reset the original struct*/
							if (animationFrame == 2 || animationFrame == 3 || animationFrame == 6 || animationFrame == 7) {
								strcpy_s(Item_DoorUnlock.itemDescrip, 57, "Use Key or Switch to unlock door");
							}
						}
						break;

					case 0xA:/*Tutorial stuff*/
						if (animationFrame == 0) {
							doorColor = (short) levelGraphicData[curLevel][w][h][1];
							Item_TutShopEntry.graphicAdd = &itemGraphicData[8][0][0][0];
							Spawn_Item(&Item_TutShopEntry, w, h, w, h, w + Item_TutShopEntry.graphicSize.X - 1, h + Item_TutShopEntry.graphicSize.Y - 1, 0);
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

void SpawnProjectile(Item *item, short x, short y, enum DIRECTION dir, short range, int speed, int color) {

	item->pos.X = x;
	item->pos.Y = y;
	item->fpos.X = (float)x;
	item->fpos.Y = (float)y;
	item->zoneOrig.X = x;
	item->zoneOrig.Y = y;
	item->numOfUsage = speed;
	item->usageCode = color;
	switch (dir)
	{
		case DIR_RIGHT:
			item->zoneEnd.X = x + range;
			item->zoneEnd.Y = y;
			break;
		case DIR_UP:
			item->zoneEnd.X = x ;
			item->zoneEnd.Y = y - range;
			break;
		case DIR_LEFT:
			item->zoneEnd.X = x - range;
			item->zoneEnd.Y = y;
			break;
		case DIR_DOWN:
			item->zoneEnd.X = x;
			item->zoneEnd.Y = y + range;
			break;
	}
	activeProjectileList[activeProjectileCount] = *item;
	++activeProjectileCount;
	
}

void UpdateProjectile(struct ITEM *item) {
	int dirX = 0, dirY = 0;
	Direction projDir = DIR_RIGHT; /*Initalise value*/
	dirX = item->zoneEnd.X - item->zoneOrig.X;
	dirY = item->zoneEnd.Y - item->zoneOrig.Y;

	/*Update the projectile movement*/
	if (dirX > 0) { /*Moving Right*/
		item->fpos.X += item->numOfUsage * 0.001f * Clock_GetDeltaTime();
		projDir = DIR_RIGHT;
	}
	else if (dirX < 0) { /*Moving Left*/
		item->fpos.X -= item->numOfUsage * 0.001f * Clock_GetDeltaTime();
		projDir = DIR_LEFT;
	}
	else if (dirY < 0) { /*Moving Up*/
		item->fpos.Y -= item->numOfUsage * 0.001f * Clock_GetDeltaTime();
		projDir = DIR_UP;
	}
	else if (dirY > 0) { /*Moving Down*/
		item->fpos.Y += item->numOfUsage * 0.001f * Clock_GetDeltaTime();
		projDir = DIR_DOWN;
	}

	item->pos.X = (short)roundf(item->fpos.X);
	item->pos.Y = (short)roundf(item->fpos.Y);


	/*If collide with walls, despawns*/
	if (Collision_Projectile(item, projDir)) {
		DespawnProjectile(item);
		return;
	}

	/*Check projectile lifespan*/
	/*If reach target, despawn*/
	if (item->pos.X == item->zoneEnd.X && item->pos.Y == item->zoneEnd.Y) {
		if (strcmp(item->name, "Rock")== 0) {
			Spawn_Item(&Item_Rock, item->pos.X, item->pos.Y, item->pos.X, item->pos.Y, item->pos.X, item->pos.Y, LIGHTBLUE);
			Spawn_SoundItem(&Item_Sound, item->pos.X, item->pos.Y, item->pos.X, item->pos.Y, item->pos.X, item->pos.Y, 4, 4);
		}
		DespawnProjectile(item);
	}
}
/*Clear all projectiles*/
void DespawnAllProjectile()
{
	for (int i = activeProjectileCount - 1; i >= 0; --i)
	{
		activeProjectileList[i] = (struct ITEM){ 0 };
	}
	activeProjectileCount = 0;
}

/*Isnt working yet*/
/*Stop the moving bullet into a active item*/
void SpawnRock(struct ITEM* item, short x, short y, enum DIRECTION dir, int speed, int color)
{
	Item_Rock.pos.X = x;
	Item_Rock.pos.Y = y;
	Item_Rock.fpos.X = (float)x;
	Item_Rock.fpos.Y = (float)y;
	Item_Rock.zoneOrig.X = x;
	Item_Rock.zoneOrig.Y = y;
	item->numOfUsage = speed;
	item->usageCode = color;
	switch (dir)
	{
	case DIR_RIGHT:
		Item_Rock.zoneEnd.X = x + 5;
		Item_Rock.zoneEnd.Y = y;
		break;
	case DIR_UP:
		Item_Rock.zoneEnd.X = x;
		Item_Rock.zoneEnd.Y = y - 5;
		break;
	case DIR_LEFT:
		Item_Rock.zoneEnd.X = x - 5;
		Item_Rock.zoneEnd.Y = y;
		break;
	case DIR_DOWN:
		Item_Rock.zoneEnd.X = x;
		Item_Rock.zoneEnd.Y = y + 5;
		break;
	}
}

/*Remove projectile from the game*/
void DespawnProjectile(struct ITEM* item) {
	int i = 0, projIndex = 0;

	/*Find the projectile*/
	for (i = 0; i < activeProjectileCount; ++i)
	{
		if (activeProjectileList[i].pos.X == item->pos.X && activeProjectileList[i].pos.Y == item->pos.Y) {
			projIndex = i;
			break;
		}
	}
	/*Push the array down*/
	for (; projIndex < activeInventoryCount - 1; ++projIndex) { /*Make sure does not go out of range*/
		activeInventoryList[projIndex] = activeInventoryList[projIndex + 1];
	}
	--activeProjectileCount;
}

/*Check when player reaches the end of the level AND has a key to open*/
void Item_LevelBorder(int xSelf, int ySelf, int levelcount)
{
	int i = 0;
	if(levelGraphicData[levelcount][xSelf][ySelf][0] == 236)
	{
		for (i = 0; i < activeInventoryCount; ++i)
		{
			if (strcmp(activeInventoryList[i].name, "LVLKey") == 0) {
				//Goes to Win screen when player reaches the end of the last level border
				if (levelcount == 3) /*Change the number according to the level we have*/
				{
					gameWin = 1;
					GotoGameOver();
				}

				RemoveInventoryItem(i); /*Remove the lvl key*/
				GotoNextLevel(++levelcount);
			}
			
		}
		
	}
}
/*DOOR function, return 0 = door is now cloase, return 1 = door is now open*/
int Item_OpenCloseDoor(struct ITEM* item)
{
	/*Check if door is open or closed*/
	switch (item->graphicIndex) {
		case 0:/*Vertical Door Closed*/
			item->graphicAdd += 3 * 9 * (2);
			item->graphicIndex += (2);
			strcpy_s(item->itemDescrip, 57, "Door is opened");
			break;
		case 1:/*Vertical Door Closed*/
			item->graphicAdd += 3 * 9 * (2);	
			item->graphicIndex += (2);
			strcpy_s(item->itemDescrip, 57, "Door is opened");
			break;

		case 4:/*Horizontal Door Closed*/
			item->graphicAdd += 3 * 9 * (2);	/*!TakeNote Place holder*/
			item->graphicIndex += 2;
			strcpy_s(item->itemDescrip, 57, "Door is opened");
			break;
		case 5:/*Horizontal Door Closed*/
			item->graphicAdd += 3 * 9 * (2);	/*!TakeNote Place holder*/
			item->graphicIndex += 2;
			strcpy_s(item->itemDescrip, 57, "Door is opened");
			break;

		case 2:/*Vertical Door Opened*/
		case 3:
			item->graphicAdd -= 3 * 9 * (_int64)2;
			item->graphicIndex -= 2;
			strcpy_s(item->itemDescrip, 57, "Use Key or Switch to unlock door");
			return 0;
			break;
		case 6:/*Horizontal Door Opened*/
		case 7:
			item->graphicAdd -= 3 * 9 * (_int64)2;
			item->graphicIndex -= 2;
			strcpy_s(item->itemDescrip, 57, "Use Key or Switch to unlock door");
			return 0;
			break;
	}
	return 1;
}

/*0 = Entry unstep, 1 = Entry step, 2 = Exit unstep, 3 = Exit step*/
/*state 0 = step, state 1 = left step state*/
void Item_MinimartExitEntry(struct ITEM* item, int state) {
	if (state == 0) {
		if (item->numOfUsage == 0) {
			item->numOfUsage = 1;
		}
		else if (item->numOfUsage == 2) {
			item->numOfUsage = 3;
		}
	}
	else if (state == 1) {
		if (item->numOfUsage == 1) {
			Item_TutShopEntry.graphicAdd = &itemGraphicData[7][0][0][0];
			item->numOfUsage = 2;
		}
		else if (item->numOfUsage == 3) {
			Item_TutShopEntry.graphicAdd = &itemGraphicData[6][0][0][0];
			item->numOfUsage = 0;
		}
	}

	
}

int Collision_Projectile(struct ITEM* item, Direction dir) {
	COORD oldPos = { 0 };
	/*Check if hits player*/
	CheckCollisionAgainstPlayer(item->pos.X, item->pos.Y);

	if (Map_CheckCollision(item->pos.X, item->pos.Y, currLevel, 2)) {
		/*If rock hits wall, spawn at old position*/
		if (strcmp(item->name, "Rock") == 0) {
			oldPos = GetFacingPosition_General(item->pos.X, item->pos.Y, dir, -1);
			Spawn_Item(&Item_Rock, oldPos.X, oldPos.Y, oldPos.X, oldPos.Y, oldPos.X, oldPos.Y, LIGHTBLUE);
			Spawn_SoundItem(&Item_Sound, oldPos.X, oldPos.Y, oldPos.X, oldPos.Y, oldPos.X, oldPos.Y, 4, 4);
		}
		return 1;
	}
	return 0;
}

void Spawn_SoundItem(Item* item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int duration, int radius)
{
	item->pos.X = x;
	item->pos.Y = y;
	item->fpos.X = (float)x;
	item->fpos.Y = (float)y;
	item->zoneOrig.X = xOrig;
	item->zoneOrig.Y = yOrig;
	item->zoneEnd.X = xEnd;
	item->zoneEnd.Y = yEnd;
	item->fpos.X = (float)duration;
	item->fpos.Y = (float)duration;
	item->numOfUsage = radius;
	activeSoundList[activeSoundCount] = *item;
	++activeSoundCount;
}

void UpdateSoundLogic(struct ITEM* item) {
	item->fpos.X -= Clock_GetDeltaTime()*0.001f * 2;
	item->fpos.Y = roundf(item->fpos.X);

	if (item->fpos.Y <= 0.01f) {
		DespawnSound(item);
	}
}

void RenderSoundFx(int posX, int posY, int spread, char graphicIndex, int color) {
	Console_SetRenderBuffer_Attribute(color, BLACK, DEFAULTLVB); /*Colour*/
	switch (spread) {
	case 0:
		Console_SetRenderBuffer_Char(posX, posY, graphicIndex);
		break;
	case 1:
		Console_SetRenderBuffer_Char(posX + 1 - localMapOffset.X, posY - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - 1 - localMapOffset.X, posY - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - localMapOffset.X, posY + 1 - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - localMapOffset.X, posY - 1 - localMapOffset.Y, graphicIndex);
		break;
	default:
		Console_SetRenderBuffer_Char(posX + 2 - localMapOffset.X, posY - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - 2 - localMapOffset.X, posY - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - localMapOffset.X, posY + 2 - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - localMapOffset.X, posY - 2 - localMapOffset.Y, graphicIndex);

		Console_SetRenderBuffer_Char(posX + 1 - localMapOffset.X, posY + 1 - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - 1 - localMapOffset.X, posY - 1 - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX - 1 - localMapOffset.X, posY + 1 - localMapOffset.Y, graphicIndex);
		Console_SetRenderBuffer_Char(posX + 1 - localMapOffset.X, posY - 1 - localMapOffset.Y, graphicIndex);
		break;

	}
}

void DespawnSound(struct ITEM* item) {
	int soundIndex = 0;

	if (activeSoundCount >= 1) {
		/*Find the projectile*/
		for (int i = 0; i < activeSoundCount; ++i)
		{
			if (activeSoundList[i].pos.X == item->pos.X && activeSoundList[i].pos.Y == item->pos.Y) {
				soundIndex = i;
				break;
			}
		}
		/*Push the array down*/
		for (; soundIndex < activeSoundCount - 1; ++soundIndex) { /*Make sure does not go out of range*/
			activeSoundList[soundIndex] = activeSoundList[soundIndex + 1];
		}
		activeSoundList[activeSoundCount - 1] = (struct ITEM){ 0 };
		--activeSoundCount;
	}	
}

/*Spawn laser on map and make them active*/
void Spawn_AlarmLaser(Item* item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int usageC, float duration, int tempo)
{
	item->pos.X = x;
	item->pos.Y = y;
	item->fpos.X = (float)x;
	item->fpos.Y = (float)y;
	item->zoneOrig.X = xOrig;
	item->zoneOrig.Y = yOrig;
	item->zoneEnd.X = xEnd;
	item->zoneEnd.Y = yEnd;
	item->usageCode = usageC;
	item->fpos.X = -(tempo * 1.0f); 
	item->fpos.Y = duration;
	item->numOfUsage = tempo;
	activeItemList[activeItemCount] = *item;
	++activeItemCount;
}

void Item_RenderLaserBeam(struct ITEM* item) {
	int dirX = 0, dirY = 0, laserDir = 1;
	int i = 0; /*for loop*/
	float percentage;

	/*Laser Logics*/
	dirX = item->zoneEnd.X - item->zoneOrig.X;
	dirY = item->zoneEnd.Y - item->zoneOrig.Y;

	item->fpos.X += Clock_GetDeltaTime() * 0.001f;
	percentage = item->fpos.X / item->fpos.Y;
	if (percentage < 0.0f) { /*Laser is off*/
		if (percentage >= -0.2f) {
			Console_SetRenderBuffer_Attribute(LIGHTBLUE, BLACK, DEFAULTLVB); /*Laser about to turn on*/
		}
		else {
			return;
		}
		
	}
	else {
		if (percentage >= 1.0f) {
			item->fpos.X = -item->fpos.Y;
		}
		else if (percentage >= 0.8f) {
			Console_SetRenderBuffer_Attribute(LIGHTBLUE, BLACK, DEFAULTLVB); /*Laser about to turn off*/
		}
		else {
			Console_SetRenderBuffer_Attribute(item->usageCode, BLACK, DEFAULTLVB);
		}
	}

	/*Render the laser*/
	if (dirX != 0) {		/*Horizontal laser*/
		if (dirX < 0 ) { /*Left*/
			laserDir = -1;
		}
		for (i = 0; i <= abs(dirX); ++i) {
			if (WithinPlayArea(item->zoneOrig.X + (i * laserDir), item->zoneOrig.Y, 1, -1)) {
				Console_SetRenderBuffer_Char(item->zoneOrig.X + (i * laserDir) - localMapOffset.X, item->zoneOrig.Y - localMapOffset.Y, item->graphicIndex);
			}
		}
	}
	else if (dirY != 0) {	/*Vertical laser*/
		if (dirY < 0) { /*Up*/
			laserDir = -1;
		}
		for (i = 0; i <= abs(dirY); ++i) {
			if (WithinPlayArea(item->zoneOrig.X, item->zoneOrig.Y + (i * laserDir), 1, -1)) {
				Console_SetRenderBuffer_Char(item->zoneOrig.X - localMapOffset.X, item->zoneOrig.Y + (i * laserDir) - localMapOffset.Y, item->graphicIndex);
			}
		}
	}
}