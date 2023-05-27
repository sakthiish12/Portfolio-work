/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#pragma once
#ifndef DIFFICULTYSYSTEM_H
#define DIFFICULTYSYSTEM_H

/*Different gameplay modifiers to challenge players*/
typedef enum DIFFICULTYMODIFIER {
	DS_GuardsNum = 0,
	DS_GuardBullet = 1,
	DS_GuardMelee = 2,
	DS_GuardRange = 3,
	DS_GuardReload = 4, 
	DS_GuardSpeed = 5,
	DS_GuardReaction = 6,
	DS_PlayerSpeed = 7,
	DS_PlayerLockDelay = 8,
	DS_ItemMalfunction = 9,
	DS_PrisonAlarmTime = 10,
	DS_PrisonCamera = 11,
	DS_PrisonDarknessNum = 12,
	DS_PrisonDarknessSize = 13,
	DS_EasyTutorial = 14,
	DS_DEFAULT = 15,
} DifficultyModifier;

/*Buff or debuff*/
typedef enum DIFFICULTYTYPE {
	BUFF = 0,
	DEBUFF = 1,
	DEVHACK = 2,
} DifficultyType;

/*Struct to store currently active gameplay modifiers*/
typedef struct DIFFICULTYVARIABLE {
	enum DifficultyModifier dsMod;	/*How will the difficulty be changed*/
	enum DifficultyType type;		/*BUFF OR DEBUFF*/
	int difficultyIntensity;		/*How easy or difficult*/

}DifficultyVariable;

struct DIFFICULTYVARIABLE difficultyVarList[4]; /*Store all the active game modifier, max 4 active ones*/
struct DIFFICULTYVARIABLE defaultdiffVar; /*Default empty game modifier to reset array*/
int difficultyVarCount; /*Count of active game modifier*/
int difficultyVarMax; /*Max possible count*/
int bNextMalfunction; /*Debug to set next modifier as a malfunction*/

/*Give the gameplay a new gameplay modifier*/
void AddDifficultyVar(enum DifficultyModifier dsMod, enum DifficultyType type, int difficultyIntensity);

/*Reset previously modified gameplay global values to original state*/
void ResetDifficultVar(struct DIFFICULTYVARIABLE diffVar);

/*Reset the gameplay modifier array*/
void RemoveDifficultVar(int diffVarCount);
#endif //DIFFICULTYSYSTEM_H