/*********************************************************************************
Highscore.c
Author: Benjamin (UI)
Code Lines: 150 (Approx)
Author: Sakthiish (Graphics)
Code Lines: 150 (Approx)
Brief: Highscore functionality in the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/

#include <Windows.h> /*for Asynckeystate*/
#include "Highscore.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../States/Game.h"
#include "../States/MainMenu.h"
#include "../DifficultySystem/DifficultySystem.h"
#include "../States/GameOver.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************
int deathCount = 0;      /*Player's death count*/
int randNumCheckDEBUFF;
int randNumCheckBUFF;
int resetGameCount = 0;  /*Amount of times game was reset*/
int totalGameScore = 0;  /*Score scored by player*/
int Alphabet = 65;		 /*Player's name alpabet*/
int position = 0;		 /*Player's name position*/
int playerentered = 0;   /*Has player pressed the enter key*/
Highscore player = { 0, {65, 65,65,65} }; /*Struct player , used for their naming*/
int playerattempt = 0;    /*Attempt affecting with highscore*/
int randNumCheck;         /**/
int highScoreCounter = 0; /*Counter for highscore "Amount of times highscore is added
						  into highscore list"*/


//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Highscore_EnterState()
{
	int i;

	randNumCheckBUFF = rand() % 5;
	randNumCheckDEBUFF = rand() % 6;

	for (i = 0; i < currLevel; ++i)
	{
		totalGameScore += overallLevelScore[i];    /*Adding all level scores together*/
	}

	if (resetGameCount > 3)
	{
		resetGameCount = 0;
	}

	/* Assigning difficulty modifier once*/
	if (bNextMalfunction) {
		AddDifficultyVar(DS_ItemMalfunction, DEBUFF, 1);
		return;
	}
	
	if (totalGameScore < 5000 * currLevel && gameWin != 1)
	{
		switch (randNumCheckBUFF)
		{
		case 1: /*Increase of Guards Range*/
			AddDifficultyVar(DS_GuardRange, DEBUFF, 1);
			break;
		case 2: /*Increase of Guards Speed*/
			AddDifficultyVar(DS_GuardSpeed, DEBUFF, 1);
			break;
		case 3: /*Guards Reload Speed*/
			AddDifficultyVar(DS_GuardReload, DEBUFF, 0);
		case 4: /*Guards Reaction Speed*/
			AddDifficultyVar(DS_GuardReaction, DEBUFF, 0);
			break;
			//case 5: /*Prison Alarm Time*/
			//	Console_SetRenderBuffer_String(14, y++, "With bad eyesight, the guards thinks");
			//	Console_SetRenderBuffer_String(14, y++, "that you are one of them when spotted.");
			//	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
			//	AddDifficultyVar(DS_GuardReaction, BUFF, 0);
			//	break;
		default: /*Increase of Player's Speed*/
			AddDifficultyVar(DS_PlayerSpeed, BUFF, 0);
			break;
		}
	}
	else if (totalGameScore >= 7000 * currLevel && gameWin != 1) {
		switch (randNumCheckDEBUFF)
		{
		case 1: /*Increase of Guards Range*/
			AddDifficultyVar(DS_GuardRange, BUFF, 0);
			break;
		case 2: /*Increase of Guards Speed*/
			AddDifficultyVar(DS_GuardSpeed, BUFF, 0);
			break;
		case 3: /*Decrease of Player's Speed*/
			AddDifficultyVar(DS_PlayerSpeed, DEBUFF, 1);
			break;
		case 4: /*Guards Reload Speed*/
			AddDifficultyVar(DS_GuardReload, BUFF, 0);
			break;
		case 5: /*Guards Reaction Speed*/
			AddDifficultyVar(DS_GuardReaction, BUFF, 0);
			break;
			//case 6: /*Prison Alarm Time*/

			//	AddDifficultyVar(DS_PrisonAlarmTime, DEBUFF, 1);
			//	break;
		default: /*Item Malfunction*/
			AddDifficultyVar(DS_ItemMalfunction, DEBUFF, 1);
			break;
		}
	}
	
	
 	/*Reset score system when game starts*/
}


//*********************************************************************************
//									INPUT
//*********************************************************************************
void Highscore_ProcessInput()
{
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		StateMachine_ChangeState(State_MainMenu);
		currLevel = 0;
		gameWin = 0;
		bNextMalfunction = 0;
	}

	/*Prevent the player from starting the game if they have not played it yet*/
	/*In case they have not read the instructions*/
	if (totalGameScore!=0 && GetAsyncKeyState('R') &  1)
	{
		timeSinceLvlStarted = 0;
		++resetGameCount;  /*Using this to check how many times the player died and restart*/
		
		StateMachine_ChangeState(State_Game);
	}

	
	if (playerentered == 0) 
	{
		/*Rotate through the alphabets and enter to add into array*/
		if (GetAsyncKeyState(VK_UP) & 1)
		{
			if (Alphabet >= 65 && Alphabet < 90)/* Check if alphabet is between A to Z */
			{
				player.name[position][playerattempt] = ++Alphabet;
			}
			else if (Alphabet >= 90)/* Ensures alphabet stays in the loop */
			{
				Alphabet = 65;
				player.name[position][playerattempt] = 65;
			}

		}
		else if (GetAsyncKeyState(VK_DOWN) & 1)
		{
			if (Alphabet <= 90 && Alphabet > 65)
			{
				player.name[position][playerattempt] = --Alphabet;
			}
			else if (Alphabet <= 65)
			{
				Alphabet = 90;
				player.name[position][playerattempt] = 90;
			}

		}
		if (GetAsyncKeyState(VK_RIGHT) & 1)
		{
			if (position < 3) {
				++position;
			}
			if (player.name[position][playerattempt] == 0) {
				player.name[position][playerattempt] = Alphabet;
			}

		}
		else if (GetAsyncKeyState(VK_LEFT) & 1)
		{
			if (position > 0) {
				--position;
			}
			

			if (player.name[position][playerattempt] == 0) {
				player.name[position][playerattempt] = Alphabet;
			}
		}

	}
	if (GetAsyncKeyState(VK_RETURN) & 1 && playerentered == 0)
	{
		/*Remove arrows and says saved*/
		playerentered = 1;

		/*Total score saved into player struct*/
		player.playerScore = totalGameScore;
		/*Counter increasing by one everything highscore is added*/
	}

}

//*********************************************************************************
//									UPDATE
//*********************************************************************************
void Highscore_Update()
{
}


//*********************************************************************************
//									RENDER
//*********************************************************************************
void Highscore_Render()
{
	int i;
	int y = 1;
	char charGameScore[16];
	/*Calculates the total game score.*/
	for (i = totalGameScore = 0; i <= currLevel; i++)
	{
		totalGameScore += overallLevelScore[i];
	}

	if (totalGameScore > 99999) {
		totalGameScore = 99999;
	}
	/*convert int to char*/
	_itoa_s(totalGameScore, charGameScore, 16, 10);
	

	/*=============================================================================*/
	/*HIGH SCORES ARE HARD CODED,Only for developers							   */
	/*=============================================================================*/
	Console_SetRenderBuffer_String(0, y++, "");
	Console_SetRenderBuffer_String(27, y++, "HIGH SCORE");
	Console_SetRenderBuffer_String(13, y++, "====================================");
	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(18, y, "BENJ");
	Console_SetRenderBuffer_String(35, y++, "100000");
	Console_SetRenderBuffer_Attribute(LIGHTMAGENTA, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(18, y, "SAK");
	Console_SetRenderBuffer_String(36, y++, "97465");
	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(18, y, "JY");
	Console_SetRenderBuffer_String(36, y++, "80808");
	Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(18, y, "HAOZHE");
	Console_SetRenderBuffer_String(36, y++, "77777");

	/*display points of the player */
	if (playerentered != 0)
	{
		Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_Char(18, y, (char)(player.name[0][playerattempt]));
		Console_SetRenderBuffer_Char(19, y, (char)(player.name[1][playerattempt]));
		Console_SetRenderBuffer_Char(20, y, (char)(player.name[2][playerattempt]));
		Console_SetRenderBuffer_Char(21, y, (char)(player.name[3][playerattempt]));
		
		Console_SetRenderBuffer_String(36, y++, charGameScore);
	}


	/*=============================================================================*/
	/*                     HIGH SCORES ARE PRINTED OUT HERE                        */
	/*=============================================================================*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	_itoa_s(totalGameScore, charGameScore, 6, 10);
	Console_SetRenderBuffer_String(15, y, "Your total score is ");
	Console_SetRenderBuffer_String(36, y++, charGameScore);
	Console_SetRenderBuffer_String(13, y++, "====================================");

	if (bNextMalfunction) {
		Console_SetRenderBuffer_String(14, y++, "Items are brittle and have a");
		Console_SetRenderBuffer_String(14, y++, "chance to break.");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	}
	else {
		/*Checking of score from lowest to highest to print out different statements*/
		if (totalGameScore == 0)
		{
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_String(18, y++, "What are you doing here??");
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_String(18, y++, "This game is very fun!!");
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_String(18, y++, "Go and try it out NOW!!");

		}
		else if (totalGameScore < 5000 * currLevel && gameWin != 1)
		{
			Console_SetRenderBuffer_String(15, y++, "The guards felt that you are not");
			Console_SetRenderBuffer_String(23, y++, "much of a threat.");
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_String(14, y++, "Guards will now have reduce speeed");
			Console_SetRenderBuffer_String(26, y++, "and detection");
			Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);

			/*Random modifier for Buff to help player*/
			switch (randNumCheckBUFF)
			{
			case 1: /*Increase of Guards Range*/
				Console_SetRenderBuffer_String(14, y++, "Guards will now have reduced range");
				Console_SetRenderBuffer_String(14, y++, "and detection");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 2: /*Increase of Guards Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guards will now have reduced speed");
				Console_SetRenderBuffer_String(14, y++, "and detection");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 3: /*Guards Reload Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guards have bad eyesight with bad aiming");
				Console_SetRenderBuffer_String(14, y++, "therfore their shooting speed has decreased.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
			case 4: /*Guards Reaction Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guards all wants to have a vacation overseas");
				Console_SetRenderBuffer_String(14, y++, "and their reaction speed has decreased.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
				//case 5: /*Prison Alarm Time*/
				//	Console_SetRenderBuffer_String(14, y++, "With bad eyesight, the guards thinks");
				//	Console_SetRenderBuffer_String(14, y++, "that you are one of them when spotted.");
				//	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				//	break;
			default: /*Increase of Player's Speed*/
				Console_SetRenderBuffer_String(14, y++, "Your speed have been increased due to");
				Console_SetRenderBuffer_String(14, y++, "you finding a pair of pegasus boots");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			}

		}

		else if (totalGameScore >= 5000 * currLevel && totalGameScore < 7000 * currLevel && gameWin != 1)
		{
			Console_SetRenderBuffer_String(16, y++, "You are quick and sly but you");
			Console_SetRenderBuffer_String(15, y++, "still couldn't outrun the guards");
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_String(15, y++, "You are thrown back in your cell");
			Console_SetRenderBuffer_String(18, y++, "to rethink about your life");
			Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		}

		else if (totalGameScore >= 7000 * currLevel && gameWin != 1)
		{
			Console_SetRenderBuffer_String(16, y++, "You did Well. Maybe too well..");
			Console_SetRenderBuffer_String(0, y++, "");
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);

			/*Random modifier for Debuff to player if they are doing too well*/
			switch (randNumCheckDEBUFF)
			{
			case 1: /*Increase of Guards Range*/
				Console_SetRenderBuffer_String(14, y++, "Guard's Range has increased to");
				Console_SetRenderBuffer_String(14, y++, "prevent you from escaping again.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 2: /*Increase of Guards Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guard's speed has increase to");
				Console_SetRenderBuffer_String(14, y++, "prevent you from escaping again.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 3: /*Decrease of Player's Speed*/
				Console_SetRenderBuffer_String(14, y++, "Your speed have been decreased");
				Console_SetRenderBuffer_String(14, y++, "due to added shackles.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 4: /*Guards Reload Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guards have all became cowboys and");
				Console_SetRenderBuffer_String(14, y++, "their shooting speed has increased.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			case 5: /*Guards Reaction Speed*/
				Console_SetRenderBuffer_String(14, y++, "Guards think that they are in The Matrix");
				Console_SetRenderBuffer_String(14, y++, "and their reaction speed has increased.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
				//case 6: /*Prison Alarm Time*/
				//	Console_SetRenderBuffer_String(14, y++, "Prison Alarm will straight away");
				//	Console_SetRenderBuffer_String(14, y++, "sound when you are spotted.");
				//	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				//	break;
			default: /*Item Malfunction*/
				Console_SetRenderBuffer_String(14, y++, "Items are brittle and have a");
				Console_SetRenderBuffer_String(14, y++, "chance to break.");
				Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
				break;
			}
		}
	}

	

	/*=============================================================================*/
	/*                    INSTRUCTIONS TO GO BACK TO MENU                          */
	/*=============================================================================*/
	Console_SetRenderBuffer_String(10, 24, "Press");
	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(16, 24, "<SPACE>");
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(24, 24, "to continue to the Main Menu");
	Console_SetRenderBuffer_String(0, 25, "");

	if(totalGameScore != 0)
	{
		/*If player hits "enter" remove the up and down arrow*/
		if (playerentered == 0)
		{

		Console_SetRenderBuffer_String(15, y += 3, "What is your name?");
		Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL);
		
		for (i = 0; i < 4; ++i) {
			if (position == i) {
				Console_SetRenderBuffer_Attribute(LIGHTGREEN, BLACK, GRID_FULL);
			}
			else {
				Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL);
			}
			Console_SetRenderBuffer_Char(34 + i*2, y, (char)(player.name[i][0]));
		}
		Console_SetRenderBuffer_Attribute(LIGHTGRAY, BLACK, GRID_FULL);
	
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(34, --y, 30);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(36, y, 30);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(38, y, 30);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(40, y, 30);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(34, y + 2, 31);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(36, y + 2, 31);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(38, y + 2, 31);
			Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(40, y + 2, 31);
		}

		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(0, 20, "");
		Console_SetRenderBuffer_String(25, 21, "Remember...");
		Console_SetRenderBuffer_String(15, 22, "THERE IS NO ESCAPE. HAHAHAHAHA");

		Console_SetRenderBuffer_String(17, 26, "Press");
		Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(23, 26, "<R>");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(27, 26, "to Restart the game");
	}
}



//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Highscore_ExitState()
{
}
