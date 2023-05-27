/*********************************************************************************
Settings.c
Author: Benjamin
Code Lines: 53
Brief: Toggling settings for the game
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <Windows.h> /*For Asynckeystate, */
#include "Settings.h"
#include "../Console/Console.h"
#include "../StateMachine/StateMachine.h"
#include "../States/Game.h"
#include "../States/MainMenu.h"
#include "../Global/Global.h"
#include "../Sounds/GameSounds.h"

//*********************************************************************************
//								LOCAL VARIABLES
//*********************************************************************************

int musicToggleState = 1;
bool toggleDeveloperCheat = 1; /*Change this to 0 to disable developer cheats*/

//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Settings_EnterState()
{
}

//*********************************************************************************
//									INPUT
//*********************************************************************************
void Settings_ProcessInput()
{
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		StateMachine_ChangeState(State_MainMenu);
	}

	if (GetAsyncKeyState('M') & 1)
	{
		if (musicToggleState == 0)
		{
			musicToggleState = 1;
			FmodSoundRelease();
		}

		else if (musicToggleState == 1)
		{
			musicToggleState = 0;
		}
	}

	//Toggle for Developer cheats
	if (GetAsyncKeyState('C') & 1)
	{
		if (toggleDeveloperCheat == 0)
		{
			toggleDeveloperCheat = 1;
		}

		else if (toggleDeveloperCheat == 1)
		{
			toggleDeveloperCheat = 0;
		}
	}
}

//*********************************************************************************
//									UPDATE
//*********************************************************************************

void Settings_Update()
{
	
}


//*********************************************************************************
//									RENDER
//*********************************************************************************
void Settings_Render()
{
	Console_SetRenderBuffer_String(10, 10, "[ ]usic:");
	Console_SetRenderBuffer_String(10, 12, "[ ]heat:");

	/*Music State Toggle*/
	if (musicToggleState == 0) //Music OFF
	{
		Console_SetRenderBuffer_String(19, 10, "OFF");
	}

	else if (musicToggleState == 1)	//Music ON
	{
		Console_SetRenderBuffer_String(19, 10, "ON");
	}

	/*Developer Hack Toggle*/
	if (toggleDeveloperCheat == 0) //Developer Hack OFF
	{
		Console_SetRenderBuffer_String(19, 12, "OFF");
	}

	else if (toggleDeveloperCheat == 1)	//Developer Hack ON
	{
		Console_SetRenderBuffer_String(19, 12, "ON");
		Console_SetRenderBuffer_String(25, 8, "F1: Next Level");
		Console_SetRenderBuffer_String(25, 10, "F2: Pass through wall");
		Console_SetRenderBuffer_String(25, 12, "F4: Trigger the Alarm");
		Console_SetRenderBuffer_String(25, 14, "F6: Game Over");
		Console_SetRenderBuffer_String(25, 16, "F7: Instant Win");

	}

	Console_SetRenderBuffer_String(10, 20, "Press the first alphabet to ON/OFF option");
	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(11, 10, "M");
	Console_SetRenderBuffer_String(11, 12, "C");
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(10, 22, "Press");
	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(16, 22, "<SPACE>");
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(24, 22, "to continue to the Main Menu");
}



//*********************************************************************************
//								STATE MANAGEMENT
//*********************************************************************************
void Settings_ExitState()
{
}
