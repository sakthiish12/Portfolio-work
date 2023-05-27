/*********************************************************************************
GameSounds.c
Author: Sakthiish (FMOD integration)
Code Lines: 88 (31 + 3 + 20 + 18 + 2 + 12 + 2)
Brief: Main source code for gameplay music and sound, using FMOD library.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h> 
#include <math.h>
#include "GameSounds.h"
#include "../Settings/Settings.h"

#include "../fmod/inc/fmod.h"
#include "../fmod/inc/fmod_common.h"
#include <stdbool.h>

/* FMOD */
FMOD_SYSTEM* fmod_system = NULL;

FMOD_SOUND* sound_Bgm = NULL;
FMOD_SOUND* sound_ObtainItem = NULL;
FMOD_SOUND* sound_UseKey = NULL;
FMOD_SOUND* sound_UseSwitch = NULL;
FMOD_SOUND* sound_GuardAlert = NULL;
FMOD_SOUND* sound_PrisonAlarm = NULL;

FMOD_CHANNEL* channel1 = NULL;
FMOD_CHANNEL* channel2 = NULL;
FMOD_CHANNEL* channel3 = NULL;
FMOD_CHANNEL* channel4 = NULL;
FMOD_CHANNEL* channel5 = NULL;
FMOD_CHANNEL* channel6 = NULL;

char filepath[250] = { 0 };
char musicpath1[250] = { 0 };
char musicpath2[250] = { 0 };
char musicpath3[250] = { 0 };
char musicpath4[250] = { 0 };
char musicpath5[250] = { 0 };
char musicpath6[250] = { 0 };

//FMUSIC_PlaySong(handle);

/*********************************************************************************
InitFMod()
Author: Sakthiish
Code Lines: 3
Brief: Initalise FMOD SYSTEM (ONCE PER GAME)
**********************************************************************************/
void InitFMod() {
	/*This can only be created once per game*/
	FMOD_System_Create(&fmod_system);
	FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);
}

/*********************************************************************************
FmodCreateSound()
Author: Sakthiish
Code Lines: 20 ++ 
Brief: Create all sound data and channel
**********************************************************************************/
void FmodCreateSound() {
	

	(void)_getcwd(filepath, 250); /*Get the current directory*/
	//strcat_s(filepath, 250, "\\Libraries\\fmod\\GameSounds");
	strcat_s(filepath, 250, "\\Audio"); /*RELEASE MODE*/

	/*Set up sounds*/
	strcpy_s(musicpath1, 250, filepath);
	strcpy_s(musicpath2, 250, filepath);
	strcpy_s(musicpath3, 250, filepath);
	strcpy_s(musicpath4, 250, filepath);
	strcpy_s(musicpath5, 250, filepath);
	strcpy_s(musicpath6, 250, filepath);
	/*Background music*/
	strcat_s(musicpath1, 250, "\\mainbackground.wav");
	/*FMOD_RESULT result1 = */FMOD_System_CreateSound(fmod_system, musicpath1, FMOD_LOOP_NORMAL, FMOD_DEFAULT, &sound_Bgm);

	/*Obtain Item Sound Fx*/
	strcat_s(musicpath2, 250, "\\ItemObtain.wav");
	/*FMOD_RESULT result2 = */FMOD_System_CreateSound(fmod_system, musicpath2, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_ObtainItem);

	/*Use Key Sound Fx*/
	strcat_s(musicpath3, 250, "\\KeyUsed.wav");
	/*FMOD_RESULT result3 = */FMOD_System_CreateSound(fmod_system, musicpath3, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_UseKey);

	/*Use Switch Sound Fx*/
	strcat_s(musicpath4, 250, "\\Switch.wav");
	/*FMOD_RESULT result4 = */FMOD_System_CreateSound(fmod_system, musicpath4, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_UseSwitch);

	/*Guard Alert Sound Fx*/
	strcat_s(musicpath5, 250, "\\GuardAlert.wav");
	/*FMOD_RESULT result5 = */FMOD_System_CreateSound(fmod_system, musicpath5, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_GuardAlert);

	/*Prison Alarm Sound Fx*/
	strcat_s(musicpath6, 250, "\\AlarmTimer_Cut.wav");
	/*FMOD_RESULT result6 = */FMOD_System_CreateSound(fmod_system, musicpath6, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_PrisonAlarm);
}

/*********************************************************************************
FmodPlayGameSound()
Author: Sakthiish
Code Lines: 18 ++
Brief:Play sound when needed, all sound will be released in sound release
**********************************************************************************/
void FmodPlayGameSound(int index ) {
	if (!musicToggleState) {
		return;
	}

	switch (index) {
	case 0:
		FMOD_System_PlaySound(fmod_system, sound_Bgm, 0, false, &channel1); /*Play Game BGM*/
		break;
	case 1:
		FMOD_System_PlaySound(fmod_system, sound_ObtainItem, 0, false, &channel2); /*Play Item Obtain*/
		break;
	case 2:
		FMOD_System_PlaySound(fmod_system, sound_UseKey, 0, false, &channel3); /*Play Use Key*/
		break;
	case 3:
		FMOD_System_PlaySound(fmod_system, sound_UseSwitch, 0, false, &channel4); /*Play Use Switch*/
		break;
	case 4:
		FMOD_System_PlaySound(fmod_system, sound_GuardAlert, 0, false, &channel5); /*Play Guard Alert*/
		break;
	case 5:
		FMOD_System_PlaySound(fmod_system, sound_PrisonAlarm, 0, false, &channel6); /*Play Guard Alert*/
		break;
	}	
}

/*********************************************************************************
FmodUpdate()
Author: Sakthiish
Code Lines: 2
Brief: Update for FMOD engine
**********************************************************************************/
void FmodUpdate() {

	FMOD_System_Update(fmod_system);
}

/*********************************************************************************
FmodSoundRelease()
Author: Sakthiish
Code Lines: 12
Brief: Release memory for sound
**********************************************************************************/
void FmodSoundRelease() {

	FMOD_Channel_Stop(channel1);
	FMOD_Sound_Release(sound_Bgm);

	FMOD_Channel_Stop(channel2);
	FMOD_Sound_Release(sound_ObtainItem);

	FMOD_Channel_Stop(channel3);
	FMOD_Sound_Release(sound_UseKey);

	FMOD_Channel_Stop(channel4); 
	FMOD_Sound_Release(sound_UseSwitch);

	FMOD_Channel_Stop(channel5); 
	FMOD_Sound_Release(sound_GuardAlert);

	FMOD_Channel_Stop(channel6); 
	FMOD_Sound_Release(sound_PrisonAlarm);
}

/*********************************************************************************
FmodClose()
Author: Sakthiish
Code Lines: 2
Brief: Release memory for FMOD SYSTEM
**********************************************************************************/
void FmodClose() {
	FMOD_System_Close(fmod_system);
}