/*********************************************************************************
GameOver.h
Author: Benjamin (UI)
Code Lines: 9 (Approx)
Brief: Statemachine for victory and game over.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef GAMEOVER_H
#define GAMEOVER_H
#include <stdbool.h>/*bool var type*/

bool gameWin;
void GameOver_EnterState();
void GameOver_ExitState();

void GameOver_ProcessInput();
void GameOver_Update();
void GameOver_Render();

#endif // GAME_H