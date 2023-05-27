/*********************************************************************************
Game.h
Author: Chong Jun Yi (Physics, Map)
Code Lines: 20 (Approx)
Author: Xie Hao Zhe (AI)
Code Lines: 10 (Approx)
Author: Sakthiish (Game items, Graphics)
Code Lines: 20 (Approx)
Author: Benjamin (UI)
Code Lines: 10 (Approx)
Brief: Header file for Game.c
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#pragma once
#ifndef GAME_H
#define GAME_H
#include<Windows.h> /*COORD*/

int bGameInitalised;
int currLevel;
int overallLevelScore[6];
float timeSinceLvlStarted;
int bInventoryRock;
COORD localMapOffset;

typedef enum GAMEPLAYSTATE {
	GAMEPLAY_RUN,
	GAMEPLAY_PAUSE,
	GAMEPLAY_LOSE,
	GAMEPLAY_TRANSITION,
} GameplayState;

enum GameplayState currGameplayState;

//typedef struct SCORE {
//	int currentlevel[7];         /*The current level the player is in*/
//	int spotted;                 /*The amount of times player is spotted by guards*/
//	int stealth;                 /*If player is not spotted this will be initiated*/
//	int timeSinceLevelStarted;   /*Time since the level has started*/
//	int levelTimeLimit;          /*Time limit given in each level*/
//}SCORE;

/**
 * \brief	When enter game state from another state
 */
void Game_EnterState();

/**
 * \brief	Handle Game input from keyboard
 */
void Game_ProcessInput();

/**
 * \brief	Handle Loading assets before game is playable
 */
void Game_Update_Loading();

/**
 * \brief	Handle Game logic each frame
 */
void Game_Update();

int EvaluateGameState(int sourceIndex);

/**
 * \brief	Handle Loading graphics
 */
void Game_Render_Loading();

/**
 * \brief	Handle Game graphics each frame
 */
void Game_Render();

void RenderGameOverAnimation();

/**
 * \brief	When exit game state 
 */
void Game_ExitState();

///**
// * \brief	Zoom In by increaseing font size and retain screensize
// */
//void ZoomIn();
///**
// * \brief	Zoom In by reduce font size and retain screensize
// */
//void ZoomOut();

/**
 * \brief	Reset game values on start game and game over
 */
void ResetGame(int lvlNum);

/*Check for any nearby item to display in UI*/
void CheckNearbyItem();

void LoadItemTextToDisplay(int itemIndex);

void CreateNotification(int templateIndex, const int *format, int formatC);

void UpdateNotification();

void RemoveNotification(int index);

void GotoNextLevel(int nextLevel);

/*Score System tabulation*/
void Game_scoreSystem(float time, float lvlTimeLimit);

void PlayerCaptured();


void GotoGameOver();

void ResetLevelScores();
#endif // GAME_H
