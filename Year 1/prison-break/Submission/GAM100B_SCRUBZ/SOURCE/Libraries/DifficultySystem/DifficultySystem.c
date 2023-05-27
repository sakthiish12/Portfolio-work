/*********************************************************************************
DifficultySystem.c
Author: Benjamin
Code Lines: 105 (12 + 10 + 80 + 3)
Brief: Difficulty system to buff or nerf objects
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include "DifficultySystem.h"
#include "../Human/Human.h"
#include "../Guard/Guard.h"
#include "../Item/Item.h"
#include "../States/Game.h"

struct DIFFICULTYVARIABLE difficultyVarList[4] = {0};/*Store all the active game modifier, max 4 active ones*/
struct DIFFICULTYVARIABLE defaultdiffVar = {  /*Default empty game modifier to reset array*/
	DS_DEFAULT,
	BUFF,
	0,
};

int difficultyVarCount = 0; /*Count of active game modifier*/
int difficultyVarMax = 4; /*Max possible count*/

int bNextMalfunction = 0; /*Debug to set next modifier as a malfunction*/
/*********************************************************************************
AddDifficultyVar()
Author: Benjamin
Code Lines: 10
Brief: Assign the difficulty modifier to gameplay
**********************************************************************************/
void AddDifficultyVar(enum DifficultyModifier dsMod, enum DifficultyType type, int difficultyIntensity) {
	if (difficultyVarCount >= difficultyVarMax-1) 
	{
		difficultyVarCount = 0; /*Overwrite the earliest modifier*/
	}
	if (difficultyVarList[difficultyVarCount].difficultyIntensity != DS_DEFAULT) {
		ResetDifficultVar(difficultyVarList[difficultyVarCount]); /*Reset old modifiers and set game to original states*/
		RemoveDifficultVar(difficultyVarCount);
	}

	/*Set to function parameter*/
	difficultyVarList[difficultyVarCount].dsMod = dsMod;
	difficultyVarList[difficultyVarCount].type = type;
	difficultyVarList[difficultyVarCount].difficultyIntensity = difficultyIntensity;
	++difficultyVarCount;
}

/*********************************************************************************
ResetDifficultVar()
Author: Benjamin
Code Lines: 80
Brief: Restore the game modifier to their original states
**********************************************************************************/
void ResetDifficultVar(struct DIFFICULTYVARIABLE diffVar) {
	switch (diffVar.dsMod)
	{
		//Add BUFF or DEBUFF in Highscore.c
		case DS_GuardsNum:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				
			}
			break;
		case DS_GuardBullet:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_GuardMelee:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_GuardRange:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardRange = 15;
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardRange = 5;
			}
			break;
		case DS_GuardReload:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardShootingReload = 0.5f;
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardShootingReload = 2.0f;
			}
			break;
		case DS_GuardSpeed:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardMoveSpeed_patrol = 6;
				guardMoveSpeed_chase = 15;
				guardMoveSpeed_invest = 10;
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardMoveSpeed_patrol = 1;
				guardMoveSpeed_chase = 5;
				guardMoveSpeed_invest = 3;
			}
			break;
		case DS_GuardReaction:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardDetectDelay = 0.1f;
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				guardDetectDelay = 1.0f;
			}
			break;
		case DS_PlayerSpeed:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				/*Buffing of Player's Speed by increasing speed*/
				PlayerChar.moveSpeed = 15;
			}
			if (diffVar.type == DEBUFF)
			{
				/*Debuffing of Player's Speed by decreasing speed*/
				PlayerChar.moveSpeed = 5;  
			}
			break;
		case DS_PlayerLockDelay:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_ItemMalfunction:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				toggleItemMalfunction = 0;
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				toggleItemMalfunction = 1;
			}
			break;
		case DS_PrisonAlarmTime:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				
			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{
				
			}
			break;
		case DS_PrisonCamera:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_PrisonDarknessNum:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_PrisonDarknessSize:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
		case DS_EasyTutorial:
			if (diffVar.type == BUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			else if (diffVar.type == DEBUFF) /*To check if it is a BUFF or DEBUFF*/
			{

			}
			break;
	}
}

/*********************************************************************************
RemoveDifficultVar()
Author: Benjamin
Code Lines: 3
Brief: Reset Game modifier from the array to a blank state
**********************************************************************************/
void RemoveDifficultVar(int diffVarCount) {
	difficultyVarList[diffVarCount] = defaultdiffVar;
}