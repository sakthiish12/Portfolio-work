/*********************************************************************************
Credits.c
Author: Benjamin (Credits)
Code Lines: 90 (6 + 4 + 13 + 6 + 61)
Brief: Display the credits page, according to digipen requirements
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include "Credits.h"
#include "../Clock/Clock.h"					/*Auto switch credits slides*/
#include "../Console/Console.h"				/*Rendering*/
#include "../StateMachine/StateMachine.h"	/*Navigating to main menu*/

/*********************************************************************************
								LOCAL VARIABLES
/*********************************************************************************/

unsigned int toggleCreditsPage = 1;			/*Which credits page to show*/
float checkTime = 0;						/*Timer to move on to next credit page*/

/*********************************************************************************
Credits_EnterState() / Credits_ExitState()
Author: Benjamin
Code Lines: 4
Brief: Handles moving in and out of credits statemachine
**********************************************************************************/
void Credits_EnterState()
{

}

void Credits_ExitState()
{
	/*Reset values before going back to main menu*/
	checkTime = 0.0f;
	toggleCreditsPage = 1;
}

/*********************************************************************************
Credits_ProcessInput()
Author: Benjamin
Code Lines: 13
Brief: Handles credits statemachine keyboard input
**********************************************************************************/
void Credits_ProcessInput()
{
	/*Manual movement across all credits slides*/
	if (GetAsyncKeyState(VK_RIGHT) & 1 || GetAsyncKeyState('D') & 1)
	{
		if (toggleCreditsPage < 5) { /*Move onwards to next credit page*/
			toggleCreditsPage += 1;
			checkTime = 0.0f;
		}
	}
	else if (GetAsyncKeyState(VK_LEFT) & 1 || GetAsyncKeyState('A') & 1)
	{
		if (toggleCreditsPage > 1) {	/*Move to previous credit page*/
			toggleCreditsPage -= 1;
			checkTime = 0.0f;
		}
		
	}
	/*Skip Credits and goback to MainMenu*/
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		StateMachine_ChangeState(State_MainMenu);		
	}
	if (GetAsyncKeyState(VK_ESCAPE) &  1)
	{
		StateMachine_ChangeState(State_MainMenu);
	}
}

/*********************************************************************************
Credits_Update()
Author: Benjamin
Code Lines: 6
Brief: Handles credits statemachine GAME LOGIC
**********************************************************************************/
void Credits_Update()
{
	checkTime += Clock_GetDeltaTime() * 0.001f;
	if (checkTime >= 2.4f) /*Approximately 3 seconds for each screen*/
	{
		toggleCreditsPage += 1; /*Increase and AUTO move to the next Credits page*/
		checkTime = 0.0f;
	}

	/*END OF AUTO CREDITS*/
	if (toggleCreditsPage == 5)
	{
		StateMachine_ChangeState(State_MainMenu);
	}
}


/*********************************************************************************
Credits_Render()
Author: Benjamin
Code Lines: 61
Brief: Render the graphics for credits screen
**********************************************************************************/
void Credits_Render()
{
	int i = 0; /*Credits auto move indicator*/

	if (toggleCreditsPage == 1) //1st Credit Page
	{
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(15, 5, "DIGIPEN INSTITUTE OF TECHNOLOGY");
		Console_SetRenderBuffer_String(25, 6, "presents");
		Console_SetRenderBuffer_String(22, 8, "A        PRODUCTION");
		Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(24, 8, "SCRUBS");
		Console_SetRenderBuffer_String(15, 10, "ESCAPE FROM DIGITAL PENITANTARY");
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(22, 12, "WWW.DIGIPEN.EDU");
		Console_SetRenderBuffer_String(13, 15, "COPYRIGHT  2019 BY DIGIPEN CORP,USA.");
		Console_SetRenderBuffer_String(21, 16, "ALL RIGHTS RESERVED.");
		Console_SetRenderBuffer_String(22, 15, "c"); /*Copyright Symbol*/
	}
	else if (toggleCreditsPage == 2) //2nd Credit Page
	{
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(23, 8, "DEVELOPED BY");

		Console_SetRenderBuffer_String(15, 15, "Jun Yi ---> Physics/ Tech Lead");
		Console_SetRenderBuffer_String(15, 16, "Hao Zhe ---> AI");
		Console_SetRenderBuffer_String(15, 17, "Sakthiish ---> Graphics");
		Console_SetRenderBuffer_String(15, 18, "Benjamin ---> UI Systems");
		Console_SetRenderBuffer_String(15, 19, "");
	}
	else if (toggleCreditsPage == 3) //3rd Credit Page
	{
		Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(24, 12, "INSTRUCTORS");
		Console_SetRenderBuffer_Attribute(MAGENTA, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(23, 15, "Andy Logam Tan");
		Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(24, 16, "Yannick Gerber");
		Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(25, 17, "Raymond Teo");

	}
	else if (toggleCreditsPage == 4) //4th Credit Page
	{
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		Console_SetRenderBuffer_String(26, 8, "PRESIDENT");
		Console_SetRenderBuffer_String(29, 9, "");
		Console_SetRenderBuffer_String(24, 10, "CLAUDE COMAIR");
		Console_SetRenderBuffer_String(29, 11, "");
		Console_SetRenderBuffer_String(25, 12, "EXECUTIVES");
		Console_SetRenderBuffer_String(19, 14, "JASON CHU  JOHN BAUER");
		Console_SetRenderBuffer_String(14, 15, "SAMIR ABOU SAMRA  RAYMOND YAN");
		Console_SetRenderBuffer_String(15, 16, "PRASANNA GHALI  MICHELE COMAIR");
		Console_SetRenderBuffer_String(17, 17, "XIN LI  ANGELA KUGLER");
		Console_SetRenderBuffer_String(14, 18, "MELVIN GONSALVEZ  MEIGHAN MCKELVEY");
		Console_SetRenderBuffer_String(15, 19, "");
		Console_SetRenderBuffer_String(12, 20, "");

	}

	/*Auto Move Timer Indicator*/
	Console_SetRenderBuffer_Attribute(MAGENTA, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_Char(30, 22, '.');
	for (i = 0; i < 6 && i < 6 - (checkTime/0.4f); ++i) {
		Console_SetRenderBuffer_Char(30 + i, 22, '.');
		Console_SetRenderBuffer_Char(30 - i, 22, '.');
	}

	/*Credits Controls instructions*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(10, 24, "Press");
	Console_SetRenderBuffer_String(24, 24, "to continue to the Main Menu");
	Console_SetRenderBuffer_String(10, 26, "Press     to manually navigate CREDITS slides");
	Console_SetRenderBuffer_Attribute(LIGHTCYAN, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(16, 24, "<SPACE>");
	Console_SetRenderBuffer_String(16, 26, "< >");
}



