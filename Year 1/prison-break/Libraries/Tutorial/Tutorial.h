/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#pragma once
#ifndef  TUTORIAL_H
#define TUTORIAL_H
#include <windows.h>/*COORD*/
#include "../Item/Item.h"

int tutorialStages;
int bDisableTutorial;

typedef struct TUTORIALTRIGGER {
	char text[40];
	COORD origZone;
	COORD endZone;
}TutorialTrigger;

struct TUTORIALTRIGGER tutorialTriggerList[7];

void TutorialEnterState();

void TutorialReset();

void TutorialUpdates(COORD playerPos);

void TutorialRender();

void Tutorial_ThiefMovement(struct ITEM* thief, int itemIndex, COORD playerPos);

void OnTutorialStageChange(int newStage);

void EnableTutorialTrigger(int triggerIndex, short zoneOrigX, short zoneOrigY, short zoneEndX, short zoneEndY);

void DisableTutorialTrigger(int triggerIndex);

void Tutorial_CashierEvent();

void LoadTutorialText(int index);

void ToggleMinimartExitEntry(COORD playerPos);

void ShopAdvertisementLogic();

void ShopAdvertisementOverlay();
#endif // ! TUTORIAL_H

