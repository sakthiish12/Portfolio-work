// /******************************************************************************/
// /*!
// \file         SoundManagerV2.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         5th Apr, 2022
// \brief        This file contains the definition of the new audio driver code 
// \lines        218
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include <iostream>
#include "../pch.h"
#include "SoundManagerV2.h"
#include "../Debugging/Assert.h"

AudioDriver& AudioDriver::GetInstance()
{
	static AudioDriver instance;
	return instance;
}

//Constructor
AudioDriver::AudioDriver()
{
	if (FMOD::System_Create(&fmodSystem) != FMOD_OK)
	{
		PartyAssert(false, "Unable to create FMOD system");
	}

	if (fmodSystem->init(512, FMOD_INIT_NORMAL, 0) != FMOD_OK)
	{
		PartyAssert(false, "Unable to initialise FMOD system");
	}

	loadSounds();
}

//Destuctor
AudioDriver::~AudioDriver()
{
	unloadSounds();

	fmodSystem->release();
}

//Helper to load sounds, prints out when there are issues loading a sound
void AudioDriver::loadHelper(const char* name, FMOD_MODE mode, FMOD::Sound** sound)
{
	if (fmodSystem->createSound(name, mode, 0, sound) != FMOD_OK)
	{
		std::cout << "<SoundManagerV2.cpp> Trouble loading " << name << std::endl;
	}
}

void AudioDriver::loadSounds()
{
	//SFX
	loadHelper("Resources/Sounds/SFXAirblastNew.ogg", FMOD_DEFAULT, &sfxAirblastNew);
	loadHelper("Resources/Sounds/SFXBlenderDispense.ogg", FMOD_DEFAULT, &sfxBlenderDispense);
	loadHelper("Resources/Sounds/SFXBlenderRun.ogg", FMOD_DEFAULT, &sfxBlenderRun);
	loadHelper("Resources/Sounds/SFXBridgeExtend.ogg", FMOD_DEFAULT, &sfxBridgeExtend);
	loadHelper("Resources/Sounds/SFXCaveWalk1.ogg", FMOD_DEFAULT, &sfxCaveWalk1);
	loadHelper("Resources/Sounds/SFXCaveWalk2.ogg", FMOD_DEFAULT, &sfxCaveWalk2);
	loadHelper("Resources/Sounds/SFXCaveWalk3.ogg", FMOD_DEFAULT, &sfxCaveWalk3);
	loadHelper("Resources/Sounds/SFXClick1.ogg", FMOD_DEFAULT, &sfxClick1);
	loadHelper("Resources/Sounds/SFXClick2.ogg", FMOD_DEFAULT, &sfxClick2);
	loadHelper("Resources/Sounds/SFXDing.ogg", FMOD_DEFAULT, &sfxDing);
	loadHelper("Resources/Sounds/SFXEnemyBite.ogg", FMOD_DEFAULT, &sfxEnemyBite);
	loadHelper("Resources/Sounds/SFXEnemyDeath.ogg", FMOD_DEFAULT, &sfxEnemyDeath);
	loadHelper("Resources/Sounds/SFXEnemyShieldBreak.ogg", FMOD_DEFAULT, &sfxEnemyShieldBreak);
	loadHelper("Resources/Sounds/SFXEnemySquish.ogg", FMOD_DEFAULT, &sfxEnemySquish);
	loadHelper("Resources/Sounds/SFXExplosion.ogg", FMOD_DEFAULT, &sfxExplosion);
	loadHelper("Resources/Sounds/SFXFallingRocks.ogg", FMOD_DEFAULT, &sfxFallingRocks);
	loadHelper("Resources/Sounds/SFXGrassWalk1.ogg", FMOD_DEFAULT, &sfxGrassWalk1);
	loadHelper("Resources/Sounds/SFXGrassWalk2.ogg", FMOD_DEFAULT, &sfxGrassWalk2);
	loadHelper("Resources/Sounds/SFXGrassWalk3.ogg", FMOD_DEFAULT, &sfxGrassWalk3);
	loadHelper("Resources/Sounds/SFXGunClick.ogg", FMOD_DEFAULT, &sfxGunClick);
	loadHelper("Resources/Sounds/SFXLaserBoom.ogg", FMOD_DEFAULT, &sfxLaserBoom);
	loadHelper("Resources/Sounds/SFXLose.ogg", FMOD_DEFAULT, &sfxLose);
	loadHelper("Resources/Sounds/SFXMetalCrunch.ogg", FMOD_DEFAULT, &sfxMetalCrunch);
	loadHelper("Resources/Sounds/SFXMushroomBossEmerge.ogg", FMOD_DEFAULT, &sfxMushroomBossEmerge);
	loadHelper("Resources/Sounds/SFXMushroomBossGrowl.ogg", FMOD_DEFAULT, &sfxMushroomBossGrowl);
	loadHelper("Resources/Sounds/SFXMushroomEmerge.ogg", FMOD_DEFAULT, &sfxMushroomEmerge);
	loadHelper("Resources/Sounds/SFXOperateLever.ogg", FMOD_DEFAULT, &sfxOperateLever);
	loadHelper("Resources/Sounds/SFXPaperCrumple.ogg", FMOD_DEFAULT, &sfxPaperCrumple);
	loadHelper("Resources/Sounds/SFXPaperCrumpleSoft.ogg", FMOD_DEFAULT, &sfxPaperCrumpleSoft);
	loadHelper("Resources/Sounds/SFXPew.ogg", FMOD_DEFAULT, &sfxPew);
	loadHelper("Resources/Sounds/SFXPlayerFootstep1.ogg", FMOD_DEFAULT, &sfxPlayerFootstep1);
	loadHelper("Resources/Sounds/SFXPlayerFootstep2.ogg", FMOD_DEFAULT, &sfxPlayerFootstep2);
	loadHelper("Resources/Sounds/SFXPlayerMelee.ogg", FMOD_DEFAULT, &sfxPlayerMelee);
	loadHelper("Resources/Sounds/SFXPlayerMeleeWhoosh.ogg", FMOD_DEFAULT, &sfxPlayerMeleeWhoosh);
	loadHelper("Resources/Sounds/SFXPlayerVacuumEnd.ogg", FMOD_DEFAULT, &sfxPlayerVacuumEnd);
	loadHelper("Resources/Sounds/SFXPlayerVacuumLoop.ogg", FMOD_DEFAULT, &sfxPlayerVacuumLoop);
	loadHelper("Resources/Sounds/SFXPlayerVacuumStart.ogg", FMOD_DEFAULT, &sfxPlayerVacuumStart);
	loadHelper("Resources/Sounds/SFXPop.ogg", FMOD_DEFAULT, &sfxPop);
	loadHelper("Resources/Sounds/SFXSquish2.ogg", FMOD_DEFAULT, &sfxSquish2);
	loadHelper("Resources/Sounds/SFXSuction1.ogg", FMOD_DEFAULT, &sfxSuction1);
	loadHelper("Resources/Sounds/SFXSuction2.ogg", FMOD_DEFAULT, &sfxSuction2);
	loadHelper("Resources/Sounds/SFXSwitch.ogg", FMOD_DEFAULT, &sfxSwitch);
	loadHelper("Resources/Sounds/SfxTeleporterAppear.ogg", FMOD_DEFAULT, &sfxTeleporterAppear);
	loadHelper("Resources/Sounds/SFXTeleporterDrone.ogg", FMOD_DEFAULT, &sfxTeleporterDrone);
	loadHelper("Resources/Sounds/SFXTomatoBossGrowl.ogg", FMOD_DEFAULT, &sfxTomatoBossGrowl);
	loadHelper("Resources/Sounds/SFXUpgradeSuccess.ogg", FMOD_DEFAULT, &sfxUpgradeSuccess);
	loadHelper("Resources/Sounds/SFXWhoosh1.ogg", FMOD_DEFAULT, &sfxWhoosh1);
	loadHelper("Resources/Sounds/SFXWhoosh2.ogg", FMOD_DEFAULT, &sfxWhoosh2);
	loadHelper("Resources/Sounds/SFXWhoosh3.ogg", FMOD_DEFAULT, &sfxWhoosh3);
	loadHelper("Resources/Sounds/SFXWhoosh4.ogg", FMOD_DEFAULT, &sfxWhoosh4);


	//BGM
	loadHelper("Resources/Sounds/BGMBeginningCutscene.ogg", FMOD_LOOP_NORMAL, &bgmBeginningCutscene);
	loadHelper("Resources/Sounds/BGMBlenderScene.ogg", FMOD_LOOP_NORMAL, &bgmBlenderScene);
	loadHelper("Resources/Sounds/BGMCredits.ogg", FMOD_LOOP_NORMAL, &bgmCredits);
	loadHelper("Resources/Sounds/BGMLose.ogg", FMOD_LOOP_NORMAL, &bgmLose);
	loadHelper("Resources/Sounds/BGMMainMenu.ogg", FMOD_LOOP_NORMAL, &bgmMainMenu);
	loadHelper("Resources/Sounds/BGMMelancholic.ogg", FMOD_LOOP_NORMAL, &bgmMelancholic);
	loadHelper("Resources/Sounds/BGMNewLevel1.ogg", FMOD_LOOP_NORMAL, &bgmNewLevel1);
	loadHelper("Resources/Sounds/BGMNewLevel2.ogg", FMOD_LOOP_NORMAL, &bgmNewLevel2);
	loadHelper("Resources/Sounds/BGMNewMainMenu.ogg", FMOD_LOOP_NORMAL, &bgmNewMainMenu);
	loadHelper("Resources/Sounds/BGMOldBossFight.ogg", FMOD_LOOP_NORMAL, &bgmOldBossFight);
	loadHelper("Resources/Sounds/BGMOldLevel1.ogg", FMOD_LOOP_NORMAL, &bgmOldLevel1);
	loadHelper("Resources/Sounds/BGMOldMainMenu.ogg", FMOD_LOOP_NORMAL, &bgmOldMainMenu);
	loadHelper("Resources/Sounds/BGMThroneRoom.ogg", FMOD_LOOP_NORMAL, &bgmThroneRoom);
	loadHelper("Resources/Sounds/BGMWin1.ogg", FMOD_LOOP_NORMAL, &bgmWin1);
	loadHelper("Resources/Sounds/BGMWin2.ogg", FMOD_LOOP_NORMAL, &bgmWin2);

	//VO
	loadHelper("Resources/Sounds/VO/VO_MAMA_Angry.ogg", FMOD_DEFAULT, &voMamaAngry);
	loadHelper("Resources/Sounds/VO/VO_MAMA_Disappointed1.ogg", FMOD_DEFAULT, &voMamaDisappointed1);
	loadHelper("Resources/Sounds/VO/VO_MAMA_Disappointed2.ogg", FMOD_DEFAULT, &voMamaDisappointed2);
	loadHelper("Resources/Sounds/VO/VO_MAMA_Happy.ogg", FMOD_DEFAULT, &voMamaHappy);
	loadHelper("Resources/Sounds/VO/VO_MUSHROOM_growl.ogg", FMOD_DEFAULT, &voMushroomGrowl);
	loadHelper("Resources/Sounds/VO/VO_MUSHROOM_Talking_Audible.ogg", FMOD_DEFAULT, &voMushroomTalkingAudible);
	loadHelper("Resources/Sounds/VO/VO_MUSHROOM_Talking_Long.ogg", FMOD_DEFAULT, &voMushroomTalkingLong);
	loadHelper("Resources/Sounds/VO/VO_MUSHROOM_Talking_Variation1.ogg", FMOD_DEFAULT, &voMushroomTalkingVariation1);
	loadHelper("Resources/Sounds/VO/VO_MUSHROOM_Talking_Variation2.ogg", FMOD_DEFAULT, &voMushroomTalkingVariation2);
	loadHelper("Resources/Sounds/VO/VO_TOMOTO_Angry.ogg", FMOD_DEFAULT, &voTomotoAngry);
	loadHelper("Resources/Sounds/VO/VO_TOMOTO_Talking.ogg", FMOD_DEFAULT, &voTomotoTalking);
}

void AudioDriver::unloadSounds()
{
	//Free SFX
	sfxAirblastNew->release();
	sfxBlenderDispense->release();
	sfxBlenderRun->release();
	sfxBridgeExtend->release();
	sfxCaveWalk1->release();
	sfxCaveWalk2->release();
	sfxCaveWalk3->release();
	sfxClick1->release();
	sfxClick2->release();
	sfxDing->release();
	sfxEnemyBite->release();
	sfxEnemyDeath->release();
	sfxEnemyShieldBreak->release();
	sfxEnemySquish->release();
	sfxExplosion->release();
	sfxFallingRocks->release();
	sfxGrassWalk1->release();
	sfxGrassWalk2->release();
	sfxGrassWalk3->release();
	sfxGunClick->release();
	sfxLaserBoom->release();
	sfxLose->release();
	sfxMetalCrunch->release();
	sfxMushroomBossEmerge->release();
	sfxMushroomBossGrowl->release();
	sfxMushroomEmerge->release();
	sfxOperateLever->release();
	sfxPaperCrumple->release();
	sfxPaperCrumpleSoft->release();
	sfxPew->release();
	sfxPlayerFootstep1->release();
	sfxPlayerFootstep2->release();
	sfxPlayerMelee->release();
	sfxPlayerMeleeWhoosh->release();
	sfxPlayerVacuumEnd->release();
	sfxPlayerVacuumLoop->release();
	sfxPlayerVacuumStart->release();
	sfxPop->release();
	sfxSquish2->release();
	sfxSuction1->release();
	sfxSuction2->release();
	sfxSwitch->release();
	sfxTeleporterAppear->release();
	sfxTeleporterDrone->release();
	sfxTomatoBossGrowl->release();
	sfxUpgradeSuccess->release();
	sfxWhoosh1->release();
	sfxWhoosh2->release();
	sfxWhoosh3->release();
	sfxWhoosh4->release();

	//Free BGM
	bgmBeginningCutscene->release();
	bgmBlenderScene->release();
	bgmCredits->release();
	bgmLose->release();
	bgmMainMenu->release();
	bgmMelancholic->release();
	bgmNewLevel1->release();
	bgmNewLevel2->release();
	bgmNewMainMenu->release();
	bgmOldBossFight->release();
	bgmOldLevel1->release();
	bgmOldMainMenu->release();
	bgmThroneRoom->release();
	bgmWin1->release();
	bgmWin2->release();

	//Free VO
	voMamaAngry->release();
	voMamaDisappointed1->release();
	voMamaDisappointed2->release();
	voMamaHappy->release();
	voMushroomGrowl->release();
	voMushroomTalkingAudible->release();
	voMushroomTalkingLong->release();
	voMushroomTalkingVariation1->release();
	voMushroomTalkingVariation2->release();
	voTomotoAngry->release();
	voTomotoTalking->release();
}

void AudioDriver::playPannedSoundEffect(FMOD::Sound* toPlay, FMOD::Channel** ch, float pan, float vol, float pitch)
{
	if (unmuteSFX)
	{
		if (ch)
		{
			fmodSystem->playSound(toPlay, 0, false, ch);
			(*ch)->setVolume(vol * volumeSFX * (float)unmuteSFX);
			(*ch)->setPan(pan);
			(*ch)->setPitch(pitch);
		}
		else
		{
			fmodSystem->playSound(toPlay, 0, false, 0);
		}
	}
}

void AudioDriver::killChannel(FMOD::Channel* target)
{
	target->stop();
}

void AudioDriver::changeBGM(FMOD::Sound* newBGM)
{
	chBGM->stop();
	if (unmuteBGM) fmodSystem->playSound(newBGM, 0, false, &chBGM);
	chBGM->setLoopCount(-1); //Infinite loop
}


//#include <fmod_errors.h>
//#include "SoundManagerV2.h"
//#include <iostream>
//
//
//bool ErrorLog(const char* _function, const char* _file, int _line)
//{
//	//if (AudioError == true)
//	{
//		std::cout << "Error at " << _function << " " << _file << ",line " << _line << std::endl;
//		return false;
//	}
//
//	return true;
//}
//
///* FMOD */
//FMOD_SYSTEM* fmod_system = NULL;
//
//FMOD_SOUND* sound_Bgm = NULL;
//FMOD_SOUND* sound_Bite = NULL;
//FMOD_SOUND* sound_UseKey = NULL;
//FMOD_SOUND* sound_UseSwitch = NULL;
//FMOD_SOUND* sound_GuardAlert = NULL;
//FMOD_SOUND* sound_PrisonAlarm = NULL;
//
//FMOD_CHANNEL* channel1 = NULL;
//FMOD_CHANNEL* channel2 = NULL;
//FMOD_CHANNEL* channel3 = NULL;
//FMOD_CHANNEL* channel4 = NULL;
//FMOD_CHANNEL* channel5 = NULL;
//FMOD_CHANNEL* channel6 = NULL;
//
//char filepath[250] = { 0 };
//char musicpath1[250] = { 0 };
//char musicpath2[250] = { 0 };
//char musicpath3[250] = { 0 };
//char musicpath4[250] = { 0 };
//char musicpath5[250] = { 0 };
//char musicpath6[250] = { 0 };
//
////FMUSIC_PlaySong(handle);
//
//
//void InitFMod() {
//	/*This can only be created once per game*/
//	FMOD_System_Create(&fmod_system);
//	FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);
//}
//
//
//void FmodCreateSound() {
//
//	//strcat_s(filepath, 250, "\\Libraries\\fmod\\GameSounds");
//
//	/*Set up sounds*/
//	strcpy_s(musicpath1, 250, filepath);
//	strcpy_s(musicpath2, 250, filepath);
//	strcpy_s(musicpath3, 250, filepath);
//	strcpy_s(musicpath4, 250, filepath);
//	strcpy_s(musicpath5, 250, filepath);
//	strcpy_s(musicpath6, 250, filepath);
//	///*Background music*/
//	//strcat_s(musicpath1, 250, "Enemy Bite.wav");
//	///*FMOD_RESULT result1 = */FMOD_System_CreateSound(fmod_system, musicpath1, FMOD_LOOP_NORMAL, FMOD_DEFAULT, &sound_Bgm);
//
//	/*Obtain Item Sound Fx*/
//	//strcat_s(musicpath2, 250, "Resources/Sounds/Enemy Bite.wav");
//	/*FMOD_RESULT result2 = */FMOD_System_CreateSound(fmod_system, musicpath2, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_Bite);
//
//	///*Use Key Sound Fx*/
//	//strcat_s(musicpath3, 250, "\\KeyUsed.wav");
//	///*FMOD_RESULT result3 = */FMOD_System_CreateSound(fmod_system, musicpath3, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_UseKey);
//
//	///*Use Switch Sound Fx*/
//	//strcat_s(musicpath4, 250, "\\Switch.wav");
//	///*FMOD_RESULT result4 = */FMOD_System_CreateSound(fmod_system, musicpath4, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_UseSwitch);
//
//	///*Guard Alert Sound Fx*/
//	//strcat_s(musicpath5, 250, "\\GuardAlert.wav");
//	///*FMOD_RESULT result5 = */FMOD_System_CreateSound(fmod_system, musicpath5, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_GuardAlert);
//
//	///*Prison Alarm Sound Fx*/
//	//strcat_s(musicpath6, 250, "\\AlarmTimer_Cut.wav");
//	///*FMOD_RESULT result6 = */FMOD_System_CreateSound(fmod_system, musicpath6, FMOD_LOOP_OFF, FMOD_DEFAULT, &sound_PrisonAlarm);
//}
//int musicToggleState = 1;
//void FmodPlayGameSound(int index) {
//	if (!musicToggleState) {
//		return;
//	}
//
//	switch (index) {
//	case 0:
//		FMOD_System_PlaySound(fmod_system, sound_Bgm, 0, false, &channel1); /*Play Game BGM*/
//		break;
//	case 1:
//		FMOD_System_PlaySound(fmod_system, sound_Bite, 0, false, &channel2); /*Play Item Obtain*/
//		break;
//	case 2:
//		FMOD_System_PlaySound(fmod_system, sound_UseKey, 0, false, &channel3); /*Play Use Key*/
//		break;
//	case 3:
//		FMOD_System_PlaySound(fmod_system, sound_UseSwitch, 0, false, &channel4); /*Play Use Switch*/
//		break;
//	case 4:
//		FMOD_System_PlaySound(fmod_system, sound_GuardAlert, 0, false, &channel5); /*Play Guard Alert*/
//		break;
//	case 5:
//		FMOD_System_PlaySound(fmod_system, sound_PrisonAlarm, 0, false, &channel6); /*Play Guard Alert*/
//		break;
//	}
//}
//
//
//void FmodUpdate() {
//
//	FMOD_System_Update(fmod_system);
//}
//
//
//void FmodSoundRelease() {
//
//	FMOD_Channel_Stop(channel1);
//	FMOD_Sound_Release(sound_Bgm);
//
//	FMOD_Channel_Stop(channel2);
//	FMOD_Sound_Release(sound_Bite);
//
//	FMOD_Channel_Stop(channel3);
//	FMOD_Sound_Release(sound_UseKey);
//
//	FMOD_Channel_Stop(channel4);
//	FMOD_Sound_Release(sound_UseSwitch);
//
//	FMOD_Channel_Stop(channel5);
//	FMOD_Sound_Release(sound_GuardAlert);
//
//	FMOD_Channel_Stop(channel6);
//	FMOD_Sound_Release(sound_PrisonAlarm);
//}
//
//
//void FmodClose() {
//	FMOD_System_Close(fmod_system);
//}