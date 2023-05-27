/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef HIGHSCORE_H
#define HIGHSCORE_H

typedef struct HIGHSCORE
{
	int playerScore;			/*Total score scored by the player*/
	int name[4][4];				/*Player name*/
}Highscore;


int deathCount;
int resetGameCount;
int playerentered;
int position;
int Alphabet;
int highScoreCounter;
int playerattempt;
int randNumCheck;

void Highscore_EnterState();
void Highscore_ExitState();

void Highscore_ProcessInput();
void Highscore_Update();
void Highscore_Render();

#endif // HIGHSCORE_H
