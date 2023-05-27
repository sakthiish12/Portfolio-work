/*********************************************************************************
Settings.h
Author:
Code Lines: 10
Brief: Toggling settings for the game

* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdbool.h>/*bool var type*/

/*Sound on or sound off*/
int musicToggleState;

bool toggleDeveloperCheat;

void Settings_EnterState();
void Settings_ExitState();

void Settings_ProcessInput();
void Settings_Update();
void Settings_Render();

#endif // SETTINGS_H
