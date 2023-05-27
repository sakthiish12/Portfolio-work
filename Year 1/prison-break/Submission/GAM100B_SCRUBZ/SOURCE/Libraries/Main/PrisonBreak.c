/*********************************************************************************
PrisonBreak.c
Author: Sakthiish 
Code Lines: 19
Brief: Main game loop.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <stdio.h>
#include <stdbool.h> 
#include "../Global/Global.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../Clock/Clock.h"
#include "../Random/Random.h"
#include "../Sounds/GameSounds.h"

int main()
{
	/*Game Initialization*/
	Console_Init();
	Random_Init();
	InitFMod(); /*Starts Fmod System once*/
	Console_SetTitle("ESCAPE From DIGITAL PENITENTIARY");
	Console_SetSquareFont(Global_fontSizeOrig.X, Global_fontSizeOrig.Y);
	Console_SetWindowedMode(Global_screenSize.X, Global_screenSize.Y, false);
	Global_ConformAccurateScreenSize((short)Console_GetWidth(), (short)Console_GetHeight());
	Console_SetCursorVisibility(0);

	/*Later change this state to splash screen*/
	StateMachine_ChangeState(State_SplashScreen);

	/* Game Loop*/
	while (Global_bIsGameRunning())
	{
		Clock_GameLoopStart(); /*This fixes the delta time thingy*/
		StateMachine_StartFrame();
		StateMachine_ProcessInput();
		StateMachine_Update();
		StateMachine_Render();
	}

	/*Game Program Exit*/
	Console_CleanUp();
	FmodSoundRelease();  /*This one need to take alook again*/
	FmodClose();
	return 0;
}

