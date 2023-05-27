/*********************************************************************************
SplashScreen.c
Author: Chong Jun Yi
Code Lines: 42 (10 + 3 + 4 + 12 + 11 + 3)
Brief: Handles state machine for splashscreen - digipen logo and studio logo
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <math.h>
#include <stdlib.h> /*Malloc, NULL*/
#include "SplashScreen.h"
#include "../Console/Console.h" /*Rendering*/
#include "../TxtFileReader/TxtFileReader.h" /*Splashscreen Data*/
#include "../Global/Global.h"
#include "../StateMachine/StateMachine.h"
#include "../Clock/Clock.h"
#include "../Random/Random.h"

/*0 = DigipenLogo, 1= Studio Logo*/
int splashScreenStage = 0;
float logoInterval = 2.0f; /*Duration before transition to next logo  DIGIPEN 2s requirment*/
float currSSElapsed = 0.0f; /*Current timer till next transition*/
float animationTime = 0.0f; /*Animation time*/
float fanimationStartTime = 0.0f; /*Animation time*/
int animationStartTime = 0; /*Animation time*/
/*Digipen logo*/
int* randWArray = NULL;
int* randHArray = NULL;
int randDigipenColor[2] = { WHITE, RED };
/*********************************************************************************
								STATE MANAGEMENT
********************************************************************************/
/*********************************************************************************
SplashScreen_EnterState()
Author: Chong Jun Yi
Code Lines: 3
Brief: Actions to take when entering state from a different state
**********************************************************************************/
void SplashScreen_EnterState()
{
	if (!Global_bAssetsLoaded) {	/*Has the textfiles been loaded*/
		ReadFromResource();			/*Start reading textfile*/
		StateMachine_ChangeState(State_Game_Loading);
	}

	RandomiseGraphicArray();
}

/*********************************************************************************
SplashScreen_ExitState()
Author: Chong Jun Yi
Code Lines: 3
Brief: Actions to take when exiting state to a different state
**********************************************************************************/
void SplashScreen_ExitState()
{
	/*Reset values*/
	currSSElapsed = 0.0f;
	animationTime = 0.0f;
	fanimationStartTime = 0.0f;
	animationStartTime = 0;
	FreeRandomiseGraphicArray();
}

/*********************************************************************************
SplashScreen_ProcessInput()
Author: Chong Jun Yi
Code Lines: 4
Brief: Input recieved from keyboard in splashscreen state
**********************************************************************************/
void SplashScreen_ProcessInput()
{
	if (GetAsyncKeyState(VK_SPACE)) {
		if (splashScreenStage == 1) {
			currSSElapsed = 3.1f; /*Skip to main menu*/
		}
	}
}

/*********************************************************************************
SplashScreen_Update()
Author: Chong Jun Yi
Code Lines: 12
Brief: Handles timer and logic for splashscreen 
**********************************************************************************/
void SplashScreen_Update()
{
	animationTime += Clock_GetDeltaTime() * 0.001f * 40;
	if (animationTime > 90) {
		fanimationStartTime += Clock_GetDeltaTime() * 0.001f * 70;
	}



	/*Timer*/
	currSSElapsed += Clock_GetDeltaTime() *0.001f;
	if (currSSElapsed > 3.0f) {
		currSSElapsed = 0.0f;
		++splashScreenStage;
		animationTime = 0.0f;
		fanimationStartTime = 0.0f;

		if (splashScreenStage == 2) {
			StateMachine_ChangeState(State_MainMenu);
		}
	}
}


/*********************************************************************************
SplashScreen_Render()
Author: Chong Jun Yi
Code Lines: 11
Brief: Render graphics for splashscreen logos
**********************************************************************************/
void SplashScreen_Render()
{
	int w = 0, h = 0;
	int randw = 0, randh = 0;
	//int randColor = 0;

	if (splashScreenStage == 0) {
		/*Render Digipen Logo, fixed color*/
		Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
		for (h = 0; h < 31 && h < (int)roundf(animationTime); ++h)
		{
			for (w = 0; w < 49 && w < (int)roundf(animationTime); ++w)
			{
				randh = randHArray[h];
				randw = randWArray[w];
				if (uISplashScreenData[7][randw][randh][0] != 0) {
					///*Random 2 color*/
					//if ((int)roundf(animationTime) < 50) {
					//	randColor = Random_Range(0, 1);
					//	Console_SetRenderBuffer_Attribute(randDigipenColor[randColor], BLACK, DEFAULTLVB);
					//}
					Console_SetRenderBuffer_Char(randw + 5, randh, (char)uISplashScreenData[7][randw][randh][0]);
				}
			}
		}
	}
	else if (splashScreenStage == 1) {
		/*Render Scrubs Logo*/
		/*Performs a Scrolling Right Reveal Animation*/
		for (h = 0; h < 24; ++h)
		{
			animationStartTime = (int)roundf(fanimationStartTime);
			for (w = animationStartTime; w < (int)roundf(animationTime) && w <54; ++w)
			{
				if (uISplashScreenData[8][w][h][0] != 0) {
					/*Render color and ascii*/
					Console_SetRenderBuffer_Attribute(uISplashScreenData[8][w][h][1], uISplashScreenData[8][w][h][2], DEFAULTLVB);
					Console_SetRenderBuffer_Char(w + 3, h + 4, (char)uISplashScreenData[8][w][h][0]);
				}
			}
		}
	}
}

/*Shuffle the array with modern Fisher–Yates algorithm*/
void RandomiseGraphicArray() {
	int i = 0;
	int swapIndex = 0;

	randHArray = (int*)malloc(sizeof(int) * 31);
	if (randHArray != NULL) {
		for (i = 31 - 1; i >= 0; --i) {
			*(randHArray + i) = i; /*initalise value*/
		}
		for (i = 31 - 1; i >= 0; --i) {
			swapIndex = Random_Range(0, i);
			SimpleSwap(randHArray + i, randHArray + swapIndex);
		}
	}
	

	randWArray = (int*)malloc(sizeof(int) * 49);
	if (randWArray != NULL) {
		for (i = 49 - 1; i >= 0; --i) {
			*(randWArray + i) = i; /*initalise value*/
		}
		for (i = 49 - 1; i >= 0; --i) {
			swapIndex = Random_Range(0, i);
			SimpleSwap(randWArray + i, randWArray + swapIndex);
		}
	}
	
}

void SimpleSwap(int *a, int *b) {
	int temp = 0;
	temp = *a;
	*a = *b;
	*b = temp;
}

void FreeRandomiseGraphicArray() {
	free(randHArray);
	free(randWArray);
}

