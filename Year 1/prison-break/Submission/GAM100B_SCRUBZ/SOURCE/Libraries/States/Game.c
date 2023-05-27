/*********************************************************************************
Game.c
Author: Chong Jun Yi (Physics, Map)
Code Lines: 310 (Approx)
Author: Xie Hao Zhe (AI)
Code Lines: 150 (Approx)
Author: Sakthiish (Game items, Graphics)
Code Lines: 150 (Approx)
Author: Benjamin (UI)
Code Lines: 90 (Approx)
Brief: Main source code for gameplay levels. 
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <Windows.h>
#include <stdlib.h> /*Malloc, free*/
#include <math.h> /*Round*/
#include "Game.h"
#include "../Global/Global.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../TxtFileReader/TxtFileReader.h"
#include "../Map/Map.h"
#include "../Human/Human.h"
#include "../Guard/Guard.h"
#include "../Clock/Clock.h"
#include "../GameMenu/GameMenu.h"
#include "../Item/Item.h"
#include "GameOver.h"
#include "../Sounds/GameSounds.h"
#include "../Tutorial/Tutorial.h"
#include "../Settings/Settings.h"
#include "../DifficultySystem/DifficultySystem.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************

enum GameplayState currGameplayState = GAMEPLAY_RUN;

/*Game Initalisation*/
int bGameInitalised = 0; 

/*Overall Game Data*/
int currLevel = 0;
float levelTimeLimit = 0.0f; /*Time left before prisoner escape alarm is automatically sounded*/

/*Game Timers*/
float timeSinceLvlStarted = 0; /*Each level not each run*/

/*Game Hacks*/
int bPlayerCollisionOff = 0; /*Turn off collision*/
int bDisplayPlayerPos = 0; /*Display player position*/

/*UI vars*/
/*Items interaction*/
char textNearbyItem[11] = "Not Active"; /*Display any items near player*/
char itemText1[20] = "";			/*Store Item descriptions words*/
char itemText2[20] = "";
char itemText3[20] = "";

/*Player facing direction*/
float blinkCounter = 0.0f;			/*timer to perform blink effect for ingame UI*/
float const blinkInterval = 1.0f;	/*how long the blink should be*/
int bInventoryRock = 0;

/*Rendering Var*/
char renderNumString[4] = { 0 };	/*Char array to store numbers converted to char*/
float gameOverAnimationStage = 0.0f;
int playerPlayAreaX = 0, playerPlayAreaY = 0; 

/*Score*/
int overallLevelScore[6] = { 0 };	/*Game score system, 1 array for 1 level*/

/*Prison Alarm (Score)*/
int bPrisonAlarmActive = 0;
//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Game_EnterState()
{
	if (!bGameInitalised)			/*If textfile is read but game not started*/
	{
		ResetGame(currLevel);		/*Reset level before starting game*/
		bGameInitalised = 1;
		ResetLevelScores();
		
	}
	if (musicToggleState) {
		FmodCreateSound();
		FmodPlayGameSound(0);
	}
}

/*********************************************************************************
Game_ProcessInput()
Author: Chong Jun Yi (Player Movement)
Code Lines: ?? (?? + ?? Breakdown)
Author: Sakthiish (Player pick up items)
Code Lines:
Author: Benjamin (UI Testing)
Code Lines:
Brief: Main source code for gameplay levels.
**********************************************************************************/
void Game_ProcessInput()
{
	if (!bGameInitalised)
		return;

	/*Tutorial Phase*/
	if (currLevel == 0) {
		if (tutorialStages >= 3 && tutorialStages < 5) { 
			return; /*Player is wrongly arrested, unable to move*/
		}
	}

	/*Every input below this block should not be activated if game is not running or tutorial transition*/
	if (currGameplayState != GAMEPLAY_RUN && currGameplayState != GAMEPLAY_TRANSITION) {
		/*Check for any additional action, will usually terminate the update loop early*/
		if(EvaluateGameState(1)){
			return;
		}
	}
	if (toggleDeveloperCheat == 1)
	{
		/*Developers Hack*/
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			/*Level Hack*/
			currLevel = (currLevel == 1) ? 2 : 1;
			GotoNextLevel(currLevel);

		}
		else if (GetAsyncKeyState(VK_F2) & 1)
		{
			/*Collision Hack*/
			bPlayerCollisionOff = (bPlayerCollisionOff == 1) ? 0 : 1;
		}
		else if (GetAsyncKeyState(VK_F3) & 1)
		{
			/*Position Display Hack*/
			bDisplayPlayerPos = (bDisplayPlayerPos == 1) ? 0 : 1;
		}
		else if (GetAsyncKeyState(VK_F4) & 1)
		{
			/*Trigger the alarm*/
			levelTimeLimit = 0.0f;
		}
		else if (GetAsyncKeyState(VK_F5) & 1)
		{
			/*Test Shooting feature*/
			Guard_Shooting(&activeGuardList[0]);
		}
		else if (GetAsyncKeyState(VK_F6) & 1)
		{
			/*Test Dying feature*/
			GotoGameOver();
		}
		else if (GetAsyncKeyState(VK_F7) & 1)
		{
			/*Test Winning feature*/
			gameWin = 1;
			GotoGameOver();
		}
		else if (GetAsyncKeyState(VK_F8) & 1) {
			if (musicToggleState) {
				FmodSoundRelease();
			}
		}
		else if (GetAsyncKeyState(VK_F9) & 1) {
			ItemMalfunction(0);
		}
		else if (GetAsyncKeyState(VK_F10) & 1) {
			bNextMalfunction = 1;
			toggleItemMalfunction = 1;
		}
	}

	/*GAMEPLAY OPTIONS KEYBOARD INPUT*/
	/*Pause Game*/
	if (GetAsyncKeyState('P') & 1)
	{
		if (currGameplayState == GAMEPLAY_PAUSE) {
			currGameplayState = GAMEPLAY_RUN;
		}
		else {
			currGameplayState = GAMEPLAY_PAUSE;
		}
	}

	if (GetAsyncKeyState(VK_ESCAPE))
	{

	}

	if (GetAsyncKeyState(VK_SPACE) & 1) {
		if (currLevel == 0) {
			if (tutorialStages < 5) { /*Skip tutorial*/
				bDisableTutorial = 1;
				TutorialReset();
				GotoNextLevel(1); /*Goto Level 1*/
			}
			else if (tutorialStages == 5) { /*End Screen of tutorial*/
				TutorialReset();
				GotoNextLevel(1); /*Goto Level 1*/
			}
		}
		else {
			/*Flush input*/
		}
	}

	/*Every input below this block should not be activated if game is paused, gameover*/
	if (currGameplayState != GAMEPLAY_RUN) {
		/*Check for any additional action, will usually terminate the update loop early*/
		if (EvaluateGameState(1)) {
			return;
		}
	}

	/*CORE GAMEPLAY KEYBOARD INPUT*/

	/*Player Item throw */
	if (GetAsyncKeyState('Q') & 1) 
	{
		for (int i = 0; i < activeInventoryCount; ++i) {
			if (strcmp(activeInventoryList[i].name, "Rock")== 0) {
				SpawnProjectile(&Item_Rock, GetFacingLocation(&PlayerChar, 1).X, GetFacingLocation(&PlayerChar, 1).Y, PlayerChar.direction, 5, 4, 9);
				RemoveInventoryItem(i);
				bInventoryRock = 0;
			}
		}
		
	}

	/*Player Item pickup or interact*/
	if (GetAsyncKeyState('E') & 1)
	{
		Item_InteractActive(PlayerChar.pos.X, PlayerChar.pos.Y);

	}

	/*Player movement - Can PRESS ONCE to move or HOLD to move*/
	if (!bIsPlayerWalking) 
	{
		if (GetAsyncKeyState('W') & 1) {
			PlayerChar.direction = DIR_UP; /*We will set player facing dir regardless of collision*/
			if (!Map_CheckCollision(PlayerChar.pos.X, PlayerChar.pos.Y - 1, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_UP, &PlayerChar, 200.0f); /*200 means instantly move one space*/
				bIsPlayerWalking = 1;
			}
		}
		else if (GetAsyncKeyState('D') & 1)
		{
			PlayerChar.direction = DIR_RIGHT;
			if (!Map_CheckCollision(PlayerChar.pos.X + 1, PlayerChar.pos.Y, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_RIGHT, &PlayerChar, 200.0f);
				bIsPlayerWalking = 1;
			}
		}
		else if (GetAsyncKeyState('S') & 1)
		{
			PlayerChar.direction = DIR_DOWN;
			if (!Map_CheckCollision(PlayerChar.pos.X, PlayerChar.pos.Y + 1, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_DOWN, &PlayerChar, 200.0f);
				bIsPlayerWalking = 1;
			}
		}
		else if (GetAsyncKeyState('A') & 1)
		{
			PlayerChar.direction = DIR_LEFT;
			
			if (!Map_CheckCollision(PlayerChar.pos.X - 1, PlayerChar.pos.Y, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_LEFT, &PlayerChar, 200.0f); 
				bIsPlayerWalking = 1;
			}
		}
	}
	else /*HOLD TO MOVE*/
	{
		if (GetAsyncKeyState('W'))
		{	
			PlayerChar.direction = DIR_UP; /*We will set player facing dir regardless of collision*/
			if (!Map_CheckCollision(PlayerChar.pos.X, PlayerChar.pos.Y - 1, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_UP, &PlayerChar, Clock_GetDeltaTime()); /*Movement by euler*/
			}				
		}
		else if (GetAsyncKeyState('D'))
		{
			PlayerChar.direction = DIR_RIGHT;
			if (!Map_CheckCollision(PlayerChar.pos.X + 1, PlayerChar.pos.Y, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_RIGHT, &PlayerChar, Clock_GetDeltaTime());
			}
		}
		else if (GetAsyncKeyState('S'))
		{
			PlayerChar.direction = DIR_DOWN;
			if (!Map_CheckCollision(PlayerChar.pos.X, PlayerChar.pos.Y + 1, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_DOWN, &PlayerChar, Clock_GetDeltaTime());
			}
		}
		else if (GetAsyncKeyState('A'))
		{
			PlayerChar.direction = DIR_LEFT;
			if (!Map_CheckCollision(PlayerChar.pos.X - 1, PlayerChar.pos.Y, currLevel, 0) || bPlayerCollisionOff) {
				Human_Movement(DIR_LEFT, &PlayerChar, Clock_GetDeltaTime());
			}
		}
		else {
			if (bIsPlayerWalking) {
				bIsPlayerWalking = 0;
			}
		}
	}
}

//*********************************************************************************
//									UPDATE
//*********************************************************************************
void Game_Update_Loading()
{
	/*Display a loading screen until all textassets are loaded*/
	if (Global_bAssetsLoaded) {

		StateMachine_ChangeState(State_SplashScreen);
	}
}

void Game_Update()
{
	
	int i = 0; /*For loop index*/

	if (!bGameInitalised)
		return;

	if (currGameplayState != GAMEPLAY_RUN) {
		/*Check for any additional action, will usually terminate the update loop early*/
		if (EvaluateGameState(2)) {
			return;
		}
	}
	/**************************TUTORIAL updates**************************/
	if (currLevel == 0) {
		TutorialUpdates(PlayerChar.pos);
	}
	

	/**************************SOUND updates**************************/
	FmodUpdate();

	/**************************Graphics updates**************************/
	/*Time since level started*/
	timeSinceLvlStarted += 0.001f * Clock_GetDeltaTime();

	if (timeSinceLvlStarted > levelTimeLimit && bPrisonAlarmActive != 1) {
		PrisonAlarmEvent(-1, 1);
		CreateNotification(11, 0, -1);
		bPrisonAlarmActive = 1;
	}

	/*Player facing direction blinking timer*/
	blinkCounter += 0.001f * Clock_GetDeltaTime();
	if (blinkCounter > blinkInterval)
	{
		blinkCounter = -blinkInterval;
	}

	UpdateNotification();

	/**************************LEVEL updates**************************/
	/*Offset the map so that only a small section can be seen*/
	ScrollingMap();

	/**************************Guards updates**************************/
	/*AI updates*/
	for (i = 0; i < activeGuardCount; ++i) {
		Guard_Loop_Start(&activeGuardList[i]);
		/*if Guard Catch Player, gameover*/
		if (CheckCollisionWithPlayer(&activeGuardList[i], &PlayerChar)) {
			PlayerCaptured();
		}
	}

	
	/**************************PLAYER updates**************************/
	/*Check for any items nearby player*/
	CheckNearbyItem();
	//Item_CheckProjectile();

	/*Check if player step on level border AND go to next level if have key*/
	Item_LevelBorder(PlayerChar.pos.X - 1, PlayerChar.pos.Y-1, currLevel);
	/*if Player is hit by projectile, gameover*/
	if (IsPlayerProjectileHit(0)){
		PlayerCaptured();
	}

	/**************************ITEM updates**************************/
	/*Projectile Update*/
	for (i = 0; i < activeProjectileCount; ++i)
	{
		UpdateProjectile(&activeProjectileList[i]);
	}

	/*Sound Update*/
	for (i = 0; i < activeSoundCount; ++i)
	{
		UpdateSoundLogic(&activeSoundList[i]);
	}
	
}

/*1 = input, 2 = update, 3 = render*/
/*Return 1 means terminate, return 0 means continue*/
int EvaluateGameState(int sourceIndex) {
	switch (currGameplayState) {
		case GAMEPLAY_PAUSE:
			if (sourceIndex == 1 || sourceIndex == 2) {
				return 1;
			}
			break;
		case GAMEPLAY_LOSE:
			if (sourceIndex == 1 || sourceIndex == 2) {
				return 1;
			}
			break;
		case GAMEPLAY_TRANSITION:
			if (sourceIndex == 1 || sourceIndex == 2) {
				return 1;
			}
			break;
		default:
			break;
	}
	return 0;
}
 
//*********************************************************************************
//									RENDER
//*********************************************************************************
void Game_Render_Loading()
{
	/*Display a loading screen while players wait for textasset to load*/
	Console_SetRenderBuffer_String(0, 0, "Loading Text Assets");
	if (bErrorDetected[2])
	{
		Console_SetRenderBuffer_String(0, 1, "TextResource Folder is missing");
		Console_SetRenderBuffer_String(0, 2, "Please check readme.txt on  how to restore folder");
	}
	
}


void Game_Render()
{
	int i = 0;				/*for loops*/
	int w = 0, h = 0;		/*Local GameScreen*/
	int offW = 0, offH = 0; /*Offset compared to Actual GameMap*/

	if (!bGameInitalised)
		return;

	if (currGameplayState != GAMEPLAY_RUN) {
		/*Check for any additional action, will usually terminate the update loop early*/
		if (EvaluateGameState(3)) {
			return;
		}
	}

	/*Game UI Border*/
	GameMenu_PlayAreaBorder();
	GameMenu_InventoryBorder(24, 24);/*y and x*/
	/*Display the items in player inventory together with inventory border*/
	GameMenu_InventoryDisplayBox(3, 4, 2, 3);

	/*Game UI - Informative stats*/
	GameMenu_TimeUI(timeSinceLvlStarted, levelTimeLimit);
	GameMenu_Textbox(textNearbyItem, itemText1, itemText2, itemText3);
	GameMenu_NotificationBox();

	/*Game UI - Load any Game Hacks UI*/
	if (bDisplayPlayerPos)
	{
		Console_SetRenderBuffer_String(Global_screenSize.X - 14, Global_screenSize.Y - 4, "Pos: ");
		_itoa_s(PlayerChar.pos.X, renderNumString, 4, 10);
		Console_SetRenderBuffer_String(Global_screenSize.X - 9, Global_screenSize.Y - 4, renderNumString);
		_itoa_s(PlayerChar.pos.Y, renderNumString, 4, 10);
		Console_SetRenderBuffer_String(Global_screenSize.X - 6, Global_screenSize.Y - 4, renderNumString);
	}

	/*Load Game Map onto PlayArea*/
	for (h = 1, offH = localMapOffset.Y; h < Global_playArea.Y; ++h, ++offH)
	{
		for (w = 1, offW = localMapOffset.X; w < Global_playArea.X; ++w, ++offW)
		{
			if (levelGraphicData[currLevel][offW][offH][2] == WHITE) {
				/*Item zone detected*/
				continue;
			}
			Console_SetRenderBuffer_Attribute(levelGraphicData[currLevel][offW][offH][1], levelGraphicData[currLevel][offW][offH][2], DEFAULTLVB);
			Console_SetRenderBuffer_Char(w, h, (char)levelGraphicData[currLevel][offW][offH][0]);
		}
	}

	if (currLevel == 0) {
		TutorialRender();
		if (tutorialStages == 5) {
			return; /*End of tutorial screen*/
		}
	}

	/*Load Items Onto Map*/
	for (i = 0; i < activeItemCount; ++i)
	{
		/*One pixel Size items*/
		if (activeItemList[i].graphicSize.X == 1 && activeItemList[i].graphicSize.Y == 1)
		{
			if (strcmp(activeItemList[i].name, "Alarm Laser") == 0) {
				Item_RenderLaserBeam(&activeItemList[i]);
				continue;
			}
			if (WithinPlayArea(activeItemList[i].zoneOrig.X, activeItemList[i].zoneOrig.Y, 1, -1))
			{
				
				/*One pixel Size items*/
				Console_SetRenderBuffer_Attribute(activeItemList[i].usageCode, BLACK, DEFAULTLVB); /*Color*/
				Console_SetRenderBuffer_Char(activeItemList[i].pos.X - localMapOffset.X, activeItemList[i].pos.Y - localMapOffset.Y, activeItemList[i].graphicIndex); /*Ascii*/
			}
		}
		else /*Items larger than one pixel with animation zone*/
		{
			for (w = 0; w < activeItemList[i].graphicSize.X; ++w) {
				for (h = 0; h < activeItemList[i].graphicSize.Y; ++h) {
					if (activeItemList[i].zoneOrig.X + w - localMapOffset.X < Global_playArea.X - 1 && activeItemList[i].zoneOrig.Y + h - localMapOffset.Y < Global_playArea.Y - 1
						&& activeItemList[i].zoneOrig.X + w - localMapOffset.X > -1 && activeItemList[i].zoneOrig.Y + h - localMapOffset.Y  > -1)
					{
						/*Color*/
						Console_SetRenderBuffer_Attribute(activeItemList[i].usageCode, BLACK, DEFAULTLVB);
						if (*(activeItemList[i].graphicAdd + ((__int64)w * 3 * 3) + ((__int64)h * 3)) != 0) {
							/*Ascii*/
							Console_SetRenderBuffer_Char(1 + activeItemList[i].zoneOrig.X + w - localMapOffset.X,
								1 + activeItemList[i].zoneOrig.Y + h - localMapOffset.Y, (char) (*(activeItemList[i].graphicAdd + ((__int64)w * 3 * 3) + ((__int64)h * 3))));
						}
					}
				}
			}
		}
	}

	if (currGameplayState != GAMEPLAY_LOSE) {
		for (i = 0; i < activeGuardCount; ++i) {
			/*Render Prison Guards Detection - FOV*/
			if (strcmp(activeGuardList[i].name, "GuardTom") == 0 || strcmp(activeGuardList[i].name, "GuardHenry") == 0) {
				RenderGuardFOV(&activeGuardList[i], localMapOffset);
			}
			else if (strcmp(activeGuardList[i].name, "GuardRicky") == 0) {
				RenderGuardFOV2(&activeGuardList[i], localMapOffset);
			}

			/*Render Prison Guards Detection - Player Last Seen*/
			if (activeGuardList[i].detectedLoc.X != -1 || activeGuardList[i].detectedLoc.Y != -1) {
				if (WithinPlayArea(activeGuardList[i].detectedLoc.X, activeGuardList[i].detectedLoc.Y, 1, -1)) {
					Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
					Console_SetRenderBuffer_Char(activeGuardList[i].detectedLoc.X - localMapOffset.X, activeGuardList[i].detectedLoc.Y - localMapOffset.Y, 33);
				}

			}
		}

		for (i = 0; i < activeGuardCount; ++i) {
			/*Render Prison Guards*/
			if (activeGuardList[i].pos.X - localMapOffset.X < Global_playArea.X && activeGuardList[i].pos.Y - localMapOffset.Y < Global_playArea.Y
				&& activeGuardList[i].pos.X - localMapOffset.X > 0 && activeGuardList[i].pos.Y - localMapOffset.Y > 0) {
				Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
				Console_SetRenderBuffer_Char(activeGuardList[i].pos.X - localMapOffset.X, activeGuardList[i].pos.Y - localMapOffset.Y, activeGuardList[i].graphicIndex);
			}
		}
	}

	/*Load Player Character*/
	if (currGameplayState != GAMEPLAY_LOSE) {
		playerPlayAreaX = Global_playArea.X / 2 + playerOffSet.X;	/*Player normally at center of screen until offset*/
		playerPlayAreaY = Global_playArea.Y / 2 + playerOffSet.Y;
		Console_SetRenderBuffer_Attribute(charGraphicData[0][0][0][1], charGraphicData[0][0][0][2], DEFAULTLVB);
		Console_SetRenderBuffer_Char(playerPlayAreaX, playerPlayAreaY, (char)charGraphicData[0][0][0][0]);
	}


	/*Player Facing Direction - blinking effect*/
	if (bInventoryRock) {
		/*if (blinkCounter < blinkInterval && blinkCounter > 0.0f)*/
	/*{*/
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		switch (PlayerChar.direction) {
		case DIR_LEFT:
			Console_SetRenderBuffer_Char(playerPlayAreaX + GetFacingLocation(&PlayerChar, 1).X - PlayerChar.pos.X,
				playerPlayAreaY + GetFacingLocation(&PlayerChar, 1).Y - PlayerChar.pos.Y, 17);
			break;
		case DIR_RIGHT:
			Console_SetRenderBuffer_Char(playerPlayAreaX + GetFacingLocation(&PlayerChar, 1).X - PlayerChar.pos.X,
				playerPlayAreaY + GetFacingLocation(&PlayerChar, 1).Y - PlayerChar.pos.Y, 16);
			break;
		case DIR_UP:
			Console_SetRenderBuffer_Char(playerPlayAreaX + GetFacingLocation(&PlayerChar, 1).X - PlayerChar.pos.X,
				playerPlayAreaY + GetFacingLocation(&PlayerChar, 1).Y - PlayerChar.pos.Y, 30);
			break;
		case DIR_DOWN:
			Console_SetRenderBuffer_Char(playerPlayAreaX + GetFacingLocation(&PlayerChar, 1).X - PlayerChar.pos.X,
				playerPlayAreaY + GetFacingLocation(&PlayerChar, 1).Y - PlayerChar.pos.Y, 31);
			break;
		}

		if (levelGraphicData[currLevel][GetFacingLocation(&PlayerChar, 1).X][GetFacingLocation(&PlayerChar, 1).Y][0] == 0) {

		}
		/*}*/
	}
	

	/*Render Projectile items*/
	for (i = 0; i < activeProjectileCount; ++i)
	{
		if (WithinPlayArea(activeProjectileList[i].pos.X, activeProjectileList[i].pos.Y, -1, 1))
		{
			/*One pixel Size items*/
			if (activeProjectileList[i].graphicSize.X == 1 && activeProjectileList[i].graphicSize.Y == 1)
			{
				if (WithinPlayArea(activeProjectileList[i].pos.X, activeProjectileList[i].pos.Y, 1, -1)) {
					Console_SetRenderBuffer_Attribute(activeProjectileList[i].usageCode, BLACK, DEFAULTLVB); /*Color*/
					Console_SetRenderBuffer_Char(activeProjectileList[i].pos.X - localMapOffset.X, activeProjectileList[i].pos.Y - localMapOffset.Y, activeProjectileList[i].graphicIndex); /*Ascii*/
				}
				
			}
		}
	}

	/*Render Sound Waves from rock*/
	for (i = 0; i < activeSoundCount; ++i)
	{
		int spread = activeSoundList[i].numOfUsage - (int)activeSoundList[i].fpos.Y;

		RenderSoundFx(activeSoundList[i].pos.X, activeSoundList[i].pos.Y, spread, activeSoundList[i].graphicIndex, activeSoundList[i].usageCode);

	}

	/*Render GameOver Animation*/
	if (currGameplayState == GAMEPLAY_LOSE || currGameplayState == GAMEPLAY_TRANSITION) {
		RenderGameOverAnimation();
	}
	
}

void RenderGameOverAnimation() {
	int w = 0, h = 0;		/*Local GameScreen*/
	int radius = 0;
	int disFromTarget = 0;

	radius = Global_screenSize.Y > Global_screenSize.X ? Global_screenSize.X : Global_screenSize.Y;
	gameOverAnimationStage += Clock_GetDeltaTime()*0.001f *10;
	radius -= (int)roundf(gameOverAnimationStage);

	/*Zoom in onto the player death location*/
	for (h = 0; h < Global_screenSize.Y; ++h)
	{
		for (w = 0; w < Global_screenSize.X; ++w)
		{			
			disFromTarget = ((w - playerPlayAreaX) * (w - playerPlayAreaX)) + (h - playerPlayAreaY) * (h - playerPlayAreaY);
			if (disFromTarget > radius * radius) {
				Console_SetRenderBuffer_Attribute(DARKGRAY, BLACK, DEFAULTLVB);
				Console_SetRenderBuffer_Char(w, h, 0);
			}
			
		}
	}

	if (radius < 1) {
		if (currLevel == 0) {
			OnTutorialStageChange(5);
		}
		else {
			GotoGameOver();
		}
		
	}
}

/*********************************************************************************
								STATE MANAGEMENT
/********************************************************************************/

void Game_ExitState()
{
	///*Stop playing the sound*/
	//if (musicToggleState) {
	//	FmodSoundRelease();
	//}
}


//*********************************************************************************
//								GAME INITALISATION
//*********************************************************************************
/*Spawn all player, guard, item*/
void ResetGame(int lvlNum)
{
	/*Clears items from previous map*/
	Clear_Map_Item();
	/*COORD tempwayPt0, tempwayPt1, tempwayPt2, tempwayPt3, tempwayPt4, tempwayPt5;*/
	
	/*Check if need to skip tutorial*/
	if (lvlNum == 0 && bDisableTutorial) {
		lvlNum = 1;
		currLevel = 1;
	}
	/*Remove all notification*/
	RemoveNotification(99);
	switch (lvlNum)
	{
		case 0:
			/*Spawn Player x,y */
			Spawn_PlayerChar(16, 36);
			Item_ScanMapForItems(currLevel);
			
			Spawn_Item(&Item_Peanut, 20, 24, 20, 24, 20, 24, LIGHTMAGENTA);
			Spawn_Item(&Item_Cashier, 5, 14, 5, 14, 7, 22, BLACK);
			Spawn_Item(&Item_Thief, 23, 24, 23, 24, 23, 24, YELLOW);	
			timeSinceLvlStarted = 0;
			levelTimeLimit = 400.0f;
			bPrisonAlarmActive = 0;
			TutorialEnterState();
			currGameplayState = GAMEPLAY_RUN;
			break;

		case 1:
			/*Spawn Player x, y */
			Spawn_PlayerChar(54, 29); /*54, 29*/
			/*Spawn Item "Item name ",exact x,exact y ,affected area x(only change if item is door etc),affected area y */
			Spawn_Item(&Item_Key, 36, 32, 36, 32, 36, 32, MAGENTA);
			Spawn_Item(&Item_LevelKey, 29, 24, 29, 24, 29, 24, GREEN);
			Spawn_Item(&Item_Switch, 51, 26, 51, 26, 51, 26, YELLOW);
			//Spawn_Item(&Item_Rock, 28, 23, 28, 23, 28, 23, LIGHTBLUE);
			Spawn_Guard1(44, 13, 47, 13, 35, 13, -1, -1 ,-1 ,-1 ,-1, -1, -1 ,-1, 2);
			Spawn_AlarmLaser(&Item_AlarmLaser, 33, 42, 33, 42, 50, 42, LIGHTRED, 1.4f, 0);
			Item_ScanMapForItems(currLevel);
			timeSinceLvlStarted = 0;
			levelTimeLimit = 60.0f;
			bPrisonAlarmActive = 0;
			CreateNotification(1, 0, -1);
			currGameplayState = GAMEPLAY_RUN;
			break;
		case 2:
			/*Spawn Player x, y */
			Spawn_PlayerChar(41, 47);
			Spawn_Item(&Item_Key, 42, 37, 42, 37, 42, 37, WHITE);
			Spawn_Item(&Item_Switch, 68, 45, 68, 45, 68, 45, MAGENTA);
			Spawn_Item(&Item_Switch, 15, 45, 15, 45, 15, 45, BLUE);
			Spawn_Item(&Item_LevelKey, 42, 30, 42, 30, 42, 30, GREEN);
			Spawn_Item(&Item_Rock, 35, 30, 35, 30, 35, 30, LIGHTBLUE);
			Spawn_AlarmLaser(&Item_AlarmLaser, 16, 15, 16, 15, 67, 15, LIGHTRED, 1.4f, 0);
			Spawn_AlarmLaser(&Item_AlarmLaser, 16, 13, 16, 13, 67, 13, LIGHTRED, 1.4f, 1);
			Spawn_Guard2(60, 44, 60, 44, 60, 37, 23, 37, 23, 44, -1, -1, -1, -1, 4);
			Spawn_Guard3(23, 37, 23, 37, 23, 44, 60, 44, 60, 37, -1, -1, -1, -1, 4);
			Spawn_Guard1(29, 22, 29, 22, 29, 23, -1, -1, -1, -1, -1, -1, -1, -1, 2);
			Spawn_Guard1(45, 22, 45, 22, 45, 23, -1, -1, -1, -1, -1, -1, -1, -1, 2);
			Item_ScanMapForItems(currLevel);
			timeSinceLvlStarted = 0;
			levelTimeLimit = 30.0f;
			bPrisonAlarmActive = 0;
			CreateNotification(12, 0, -1);
			CreateNotification(13, 0, -1);
			currGameplayState = GAMEPLAY_RUN;
			break;
		case 3:
			/*Spawn Player x, y */
			Spawn_PlayerChar(24, 46);
			Spawn_Guard1(40, 5, 40, 5, 40, 21, -1, -1, -1, -1, -1, -1, -1, -1, 2);
			Spawn_Guard2(4, 26, 4, 26, 20, 26, -1, -1, -1, -1, -1, -1, -1, -1, 2);
			Spawn_Guard3(15, 30, 15, 30, 7, 30, 7, 44, 14, 44, 7, 44, 7, 30, 6);
			Spawn_Guard2(5, 4, 5, 4, 18, 4, 18, 8, 18, 4, 5, 4, -1, -1, 5);
			Spawn_Item(&Item_Key, 27, 14, 27, 14, 27, 14, YELLOW);
			Spawn_Item(&Item_Key, 20, 33, 20, 33, 20, 33, LIGHTRED);
			Spawn_Item(&Item_Switch, 40, 2, 40, 2, 40, 2, LIGHTBLUE);
			Spawn_Item(&Item_LevelKey, 21, 46, 21, 46, 21, 46, GREEN);
			Spawn_AlarmLaser(&Item_AlarmLaser, 16, 16, 16, 16, 17, 16, LIGHTRED, 1.4f, 0);
			Spawn_AlarmLaser(&Item_AlarmLaser, 16, 14, 16, 14, 17, 14, LIGHTRED, 1.4f, 1);
			Spawn_AlarmLaser(&Item_AlarmLaser, 24, 37, 24, 37, 24, 40, LIGHTRED, 1.4f, 2);
			Spawn_AlarmLaser(&Item_AlarmLaser, 10, 42, 10, 42, 10, 46, LIGHTRED, 1.4f,0);
			Spawn_Item(&Item_Rock, 37, 42, 37, 42, 37, 42, LIGHTBLUE);
			Spawn_Item(&Item_EasterEgg, 13, 18, 13, 18, 13, 18, WHITE);
			Item_ScanMapForItems(currLevel);
			timeSinceLvlStarted = 0; 
			levelTimeLimit = 30.0f;
			bPrisonAlarmActive = 0;
			currGameplayState = GAMEPLAY_RUN;
			break;

		default:
			break;
	}
}


void CheckNearbyItem() 
{
	int itemIndex = 0;
	Item_InteractPassive(PlayerChar.pos.X, PlayerChar.pos.Y, textNearbyItem, &itemIndex);

	strncpy_s(itemText1, 20, "", 0); /*Reset the strings*/
	strncpy_s(itemText2, 20, "", 0);
	strncpy_s(itemText3, 20, "", 0);
	if (itemIndex!=-1) { /*if there is a detected item*/
		/*DisplayItem ToolTip*/
		LoadItemTextToDisplay(itemIndex);
	}
	
}

void LoadItemTextToDisplay(int itemIndex) {
	int j = 0, k = 0; /*for loop*/
	char* midTxtPtr = NULL; char* midTxtPtr2 = NULL;
	int length = (int)strlen(activeItemList[itemIndex].itemDescrip);

	
	
	if (length > 19) {
		for (j = 19; j > 0; --j) {
			if (activeItemList[itemIndex].itemDescrip[j] == ' ') {
				break;
			}
		}
		midTxtPtr = &activeItemList[itemIndex].itemDescrip[j + 1];
	}
	if (length > j + 19) {
		for (k = j + 19; k > j; --k) {
			if (activeItemList[itemIndex].itemDescrip[k] == ' ') {
				break;
			}
		}
		midTxtPtr2 = &activeItemList[itemIndex].itemDescrip[k + 1];
	}
	
	if (j == 0) {
		strncpy_s(itemText1, 20, activeItemList[itemIndex].itemDescrip, 19);
	}
	else {
		strncpy_s(itemText1, 20, activeItemList[itemIndex].itemDescrip, j);
	}
	
	if (midTxtPtr != NULL) {
		if (k == 0) {
			strncpy_s(itemText2, 20, midTxtPtr, length - (__int64)j);
		}
		else {
			strncpy_s(itemText2, 20, midTxtPtr, (__int64)k - (__int64)j);
		}
		
	}
	if (midTxtPtr2 != NULL) {
		strncpy_s(itemText3, 20, midTxtPtr2, length - (__int64)k - (__int64)j);
	}
}

void CreateNotification(int templateIndex, const int *format, int formatC) {
	int i = 0, j = 0; /*for loop*/

	/*Prevent notification spam*/
	if (lastNotification == templateIndex) {
		return;
	}

	/*All the notification is full, push out the first one and insert the new one last*/
	if ((int)strlen(notificationIndex[numOfNotification-1].text) != 0){
		RemoveNotification(0);
	}

	/*Place the notification on bottom on existing ones*/
	for (i = 0; i < numOfNotification; ++i) {
		if ((int)strlen(notificationIndex[i].text) == 0){
			strcpy_s(notificationIndex[i].text, 54, gameNotificationList[templateIndex].text);
			for (j = 0; j < 8; ++j) {
				if (format == NULL) {
					notificationIndex[i].format1[j] = gameNotificationList[templateIndex].format1[j];
				}
				else {
					notificationIndex[i].format1[j] = format[j];
				}
				
			}
			if (formatC != -1) {
				notificationIndex[i].format1_Color = formatC;
			}
			else {
				notificationIndex[i].format1_Color = gameNotificationList[templateIndex].format1_Color;
			}
			notificationIndex[i].format1_Pos = gameNotificationList[templateIndex].format1_Pos;
			notificationTime[i] = 0.0f;
			break;
		}
	}
	lastNotification = templateIndex;
	
}

void UpdateNotification() {
	int i = 0;
	for (i = 0; i < numOfNotification; ++i) {
		notificationTime[i] += Clock_GetDeltaTime() * 0.001f * 30;
		/*Remove after a short time*/
		if (notificationTime[i] > 150.0f) { /*approx 5 sec*/
			RemoveNotification(i);
		}
	}
}

void RemoveNotification(int index) {
	int i= 0, j = 0;

	if (index == 99) {/*remove all lines*/
		for (i = 0; i < numOfNotification; ++i) {
			strcpy_s(notificationIndex[i].text, 54, "");
			for (j = 0; j < 8; ++j) {
				notificationIndex[i].format1[j] = 0;
			}
			notificationIndex[i].format1_Color = 0;
			notificationIndex[i].format1_Pos = 0;
			notificationTime[i] = 0.0f;
		}
		lastNotification = -1;
	}
	else { /*Remove only one line*/
		/*Move up*/
		for (; index < numOfNotification - 1; ++index) {
			notificationIndex[index] = notificationIndex[index + 1];
			notificationTime[index] = notificationTime[index + 1];
		}
		/*Clear last one*/
		strcpy_s(notificationIndex[numOfNotification - 1].text, 54, "");
		for (j = 0; j < 8; ++j) {
			notificationIndex[numOfNotification - 1].format1[j] = 0;
		}
		notificationIndex[numOfNotification - 1].format1_Color = 0;
		notificationIndex[numOfNotification - 1].format1_Pos = 0;
		notificationTime[numOfNotification - 1] = 0.0f;

		/*If no more notification*/
		if ((int)strlen(notificationIndex[0].text) == 0) {
			lastNotification = -1;
		}
	}

	
}

void GotoNextLevel(int nextLevel) {
	ClearInventory();
	ClearAllGuard();
	Game_scoreSystem(timeSinceLvlStarted, levelTimeLimit);
	currLevel = nextLevel;
	ResetGame(currLevel);
}

//*********************************************************************************
//								GAME SCORE SYSTEM
//*********************************************************************************
/*TABULATE THE SCORE OF THE PLAYER WITH TIME*/
void Game_scoreSystem(float time, float lvlTimeLimit)
{
	
	//Saving each level score depending on time left and other factors
	if (lvlTimeLimit > time)    /*If level run time is lesser than the level time*/ 
	{
		overallLevelScore[currLevel] = 5000 + (int)((lvlTimeLimit - time))*120;
	}

	else
	{
		overallLevelScore[currLevel] = 5000 + (int)((lvlTimeLimit - time))*40;
	}
}

void PlayerCaptured() {
	if (currLevel == 0) {
		currGameplayState = GAMEPLAY_TRANSITION;
	}
	else {
		currGameplayState = GAMEPLAY_LOSE;
	}
}

void GotoGameOver() {
	ClearInventory();
	ClearAllGuard();
	DespawnAllProjectile();
	if (musicToggleState) {
		FmodSoundRelease();
	}
	StateMachine_ChangeState(State_GameOver);
	bGameInitalised = 0;
	Game_scoreSystem(timeSinceLvlStarted, levelTimeLimit);
	ResetPlayerProjectileHit();
}

void ResetLevelScores() {
	int i = 0;

	for (i = 0; i < 6; ++i) {
		overallLevelScore[i] = 0;	/*Reset score to 0*/
	}
}