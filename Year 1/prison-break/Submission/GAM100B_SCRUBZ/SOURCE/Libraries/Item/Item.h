/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef ITEM_H
#define ITEM_H
#include "../Global/Global.h"

/**
*\Members: "char[16] name", "COORD pos"
*/
typedef enum INTERACTTYPE {
	interact_passive = 0,
	interact_active = 1
}InteractType;

typedef struct ITEM {
	char name[18];		/*Name of Item: */
	char itemDescrip[57];
	char graphicIndex;	/*The index from graphics array*/
	COORD graphicSize;	/*Size of graphic w x h*/
	int* graphicAdd;	/*Location of graphic*/
	COORD zoneOrig;		/*Start and End of Zone*/
	COORD zoneEnd;
	COORD pos;			/*Current DISPLAY position of item*/
	fCOORD fpos;		/*current float position*/
	InteractType interactType; /*Whether need to press button to pick up, or passive like doors*/
	int numOfUsage;		/*1 for keys, 99 for permanent items*/
	int usageCode;		
}Item;
/*List of item*/

Item Item_Key;
Item Item_DoorStatic;
Item Item_DoorUnlock;
Item Item_Bullet;
Item Item_LevelKey;
Item Item_Switch;
Item Item_Bullet;
Item Item_TutShopEntry;
Item Item_Peanut;
Item Item_Cashier;
Item Item_Thief;
Item Item_Rock;
Item Item_Sound;
Item Item_AlarmLaser;
Item Item_EasterEgg;

/*All active items on the current level*/
Item activeItemList[50];
int activeItemCount;

/*Remove soon*/
Item activeInventoryList[6];
int activeInventoryCount;

Item activeProjectileList[10];
int activeProjectileCount;

Item activeSoundList[6];
int activeSoundCount;

/*Toggle this in Difficulty System to make items Malfunction*/
bool toggleItemMalfunction;

/*Spawn item and place them in to active item array for game use*/
void Spawn_Item(Item* item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int usageC);

/*Mainly to change item within same family - Door and static door*/
void Change_Item(Item* oldItem, Item* newItem);

/*Checking it item is hit*/
int Item_CheckCollision();

/*Making item Passive*/
void Item_InteractPassive(int xSelf, int ySelf, char buffer[11], int *itemIndex);

void FurtherPassiveInteraction(struct ITEM* item);

/*Making item interactable*/
void Item_InteractActive(int xSelf, int ySelf);

/*Despawn Item*/
void Item_Despawn(int itemIndex);

void Clear_Map_Item();

/*Holds all function to perform when an item is recieved*/
void Item_ObtainItem(struct ITEM item, int itemIndex);

/*Item added into inventory*/
void AddInventoryItems(struct ITEM* item);

/*Item removed from inventory*/
void RemoveInventoryItem(int itemIndex);

void ItemMalfunction(int itemIndex);

void ClearInventory();

/*From our level data, identify which are item spots and intialise those items into the activeItemList*/
void Item_ScanMapForItems(int currLevel);

/*Spawn a projectile into activeProjectileList*/
void SpawnProjectile(Item* item, short x, short y, enum DIRECTION dir, short range, int speed, int color);

/*Update the projectile movement*/
void UpdateProjectile(struct ITEM *item);


void SpawnRock(struct ITEM* item, short x, short y, enum DIRECTION dir, int speed, int color);

/*Remove projectile from the game*/
void  DespawnProjectile(struct ITEM* item);

/*Removes all projectiles from the game*/
void DespawnAllProjectile();

int Item_OpenCloseDoor(struct ITEM* item);

void Item_MinimartExitEntry(struct ITEM* item, int state);

/*When player reaches the end of the level*/
void Item_LevelBorder(int xSelf, int ySelf, int levelcount);

int Collision_Projectile(struct ITEM* item, Direction dir);

void Spawn_SoundItem(Item* item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int duration, int radius);

void UpdateSoundLogic(struct ITEM* item);

void RenderSoundFx(int posX, int posY, int spread, char graphicIndex, int color);

void DespawnSound(struct ITEM* item);

void Spawn_AlarmLaser(Item* item, short x, short y, short xOrig, short yOrig, short xEnd, short yEnd, int usageC, float duration, int tempo);

void Item_RenderLaserBeam(struct ITEM* item);
#endif // !ITEM_H
