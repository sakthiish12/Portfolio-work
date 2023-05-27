/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#pragma once
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

void SplashScreen_EnterState();

void SplashScreen_ExitState();

void SplashScreen_ProcessInput();

void SplashScreen_Update();

void SplashScreen_Render();

void RandomiseGraphicArray();

void SimpleSwap(int* a, int* b);

void FreeRandomiseGraphicArray();
#endif // !SPLASHSCREEN_H
