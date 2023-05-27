// /******************************************************************************/
// /*!
// \file         SoundManagerV2.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         5th Apr, 2022
// \brief        This file contains the declaration of the new audio driver
//				 code
// \lines		 113
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#pragma once
#ifndef SOUNDMANAGERV2_H
#define SOUNDMANAGERV2_H

#include "fmod.hpp"
#include <unordered_map>
#include <stdbool.h>/*bool var type*/
//Assertion for Audio
//#define ASSERT(x) if(!(x)) __debugbreak();
//#define AudioCall(x) x;\
//    ASSERT(ErrorLog(#x, __FILE__, __LINE__))

class AudioDriver
{
private:
	//SINGLETON

	//Single. Constructors should not be accessible
	//Though, there actually needs to exist a constructor for the code to
	//compile
	//To call loadSounds()
	AudioDriver();

	~AudioDriver(); //To call unloadSounds()

	//Delete copy/move ctors so extra instances cannot be created
	//Singletons should not be clonable
	AudioDriver(const AudioDriver&) = delete;
	AudioDriver(AudioDriver&&) = delete;

	//Singletons should not be assignable to
	AudioDriver& operator=(const AudioDriver&) = delete;
	AudioDriver& operator=(AudioDriver&&) = delete;

	//HELPER FUNCTIONS/////////////////////////////////////////////////////////
	//To help load sounds and print out if there are issues loading
	void loadHelper(const char* name, FMOD_MODE mode, FMOD::Sound** sound);

	//TODO Probably have a vector of strings, sounds and channels in private section
	//     Then in the public section, have an enum class of numbers corresponding to the sounds

	//	   This is such that releasing can probably be done easier
	//	   By iterating through the vector? Takes time to reimplement

public:
	//SINGLETON GET INSTANCE
	static AudioDriver& GetInstance();

	//VITALS///////////////////////////////////////////////////////////////////
	FMOD::System* fmodSystem;

	//CHANNELS/////////////////////////////////////////////////////////////////
	//Add to the list as needed
	FMOD::Channel
		* chBGM,		//For Background Music, best to be one track at a time
		* chSFX;		//Main handler for sound effects

	//SOUND EFFECTS////////////////////////////////////////////////////////////
	//Add to the list as needed
	//BEST TO BE IN ALPHABETICAL ORDER
	FMOD::Sound
		* sfxAirblastNew,				//SFXAirblastNew.ogg
		* sfxBlenderDispense,			//SFXBlenderDispense.ogg
		* sfxBlenderRun,				//SFXBlenderRun.ogg
		* sfxBridgeExtend,				//SFXBridgeExtend.ogg
		* sfxCaveWalk1,					//SFXCaveWalk1.ogg
		* sfxCaveWalk2,					//SFXCaveWalk2.ogg
		* sfxCaveWalk3,					//SFXCaveWalk3.ogg
		* sfxClick1,					//SFXClick1.ogg
		* sfxClick2,					//SFXClick2.ogg
		* sfxDing,						//SFXDing.ogg
		* sfxEnemyBite,					//SFXEnemyBite.ogg
		* sfxEnemyDeath,				//SFXEnemyDeath.ogg
		* sfxEnemyShieldBreak,			//SFXEnemyShieldBreak.ogg
		* sfxEnemySquish,				//SFXEnemySquish.ogg
		* sfxExplosion,					//SFXExplosion.ogg
		* sfxFallingRocks,				//SFXFallingRocks.ogg
		* sfxGrassWalk1,				//SFXGrassWalk1.ogg
		* sfxGrassWalk2,				//SFXGrassWalk2.ogg
		* sfxGrassWalk3,				//SFXGrassWalk3.ogg
		* sfxGunClick,					//SFXGunClick.ogg
		* sfxLaserBoom,					//SFXLaserBoom.ogg
		* sfxLose,						//SFXLose.ogg
		* sfxMetalCrunch,				//SFXMetalCrunch.ogg
		* sfxMushroomBossEmerge,		//SFXMushroomBossEmerge.ogg
		* sfxMushroomBossGrowl,			//SFXMushroomBossGrowl.ogg
		* sfxMushroomEmerge,			//SFXMushroomEmerge.ogg
		* sfxOperateLever,				//SFXOperateLever.ogg
		* sfxPaperCrumple,				//SFXPaperCrumple.ogg
		* sfxPaperCrumpleSoft,			//SFXPaperCrumpleSoft.ogg
		* sfxPew,						//SFXPew.ogg
		* sfxPlayerFootstep1,			//SFXPlayerFootstep1.ogg
		* sfxPlayerFootstep2,			//SFXPlayerFootstep2.ogg
		* sfxPlayerMelee,				//SFXPlayerMelee.ogg
		* sfxPlayerMeleeWhoosh,			//SFXPlayerMeleeWhoosh.ogg
		* sfxPlayerVacuumEnd,			//SFXPlayerVacuumEnd.ogg
		* sfxPlayerVacuumLoop,			//SFXPlayerVacuumLoop.ogg
		* sfxPlayerVacuumStart,			//SFXPlayerVacuumStart.ogg
		* sfxPop,						//SFXPop.ogg
		* sfxSquish2,					//SFXSquish2.ogg
		* sfxSuction1,					//SFXSuction1.ogg
		* sfxSuction2,					//SFXSuction2.ogg
		* sfxSwitch,					//SFXSwitch.ogg
		* sfxTeleporterAppear,			//SFXTeleporterAppear.ogg
		* sfxTeleporterDrone,			//SFXTeleporterDrone.ogg
		* sfxTomatoBossGrowl,			//SFXTomatoBossGrowl.ogg
		* sfxUpgradeSuccess,			//SFXUpgradeSuccess.ogg
		* sfxWhoosh1,					//SFXWhoosh1.ogg
		* sfxWhoosh2,					//SFXWhoosh2.ogg
		* sfxWhoosh3,					//SFXWhoosh3.ogg
		* sfxWhoosh4;					//SFXWhoosh4.ogg

	//BACKGROUND MUSIC/////////////////////////////////////////////////////////
	//Add to the list as needed
	//BEST TO KEEP ALPHABETICAL ORDER
	FMOD::Sound
		* bgmBeginningCutscene,				//BGMBeginningCutscene.ogg
		* bgmBlenderScene,					//BGMBlenderScene.ogg
		* bgmCredits,						//BGMCredits.ogg
		* bgmLose,							//BGMLose.ogg
		* bgmMainMenu,						//BGMMainMenu.ogg
		* bgmMelancholic,					//BGMMelancholic.ogg
		* bgmNewLevel1,						//BGMNewLevel1.ogg
		* bgmNewLevel2,						//BGMNewLevel2.ogg
		* bgmNewMainMenu,					//BGMNewMainMenu.ogg
		* bgmOldBossFight,					//BGMOldBossFight.ogg
		* bgmOldLevel1,						//BGMOldLevel1.ogg
		* bgmOldMainMenu,					//BGMOldMainMenu.ogg
		* bgmThroneRoom,					//BGMThroneRoom.ogg
		* bgmWin1,							//BGMWin1.ogg
		* bgmWin2;							//BGMWin2.ogg

	//VOICEOVERS///////////////////////////////////////////////////////////////
	//Add to the list as needed
	//BEST TO KEEP ALPHABETICAL ORDER
	FMOD::Sound
		* voMamaAngry,						//VO/VO_MAMA_Angry.ogg
		* voMamaDisappointed1,				//VO/VO_MAMA_Disappointed1.ogg
		* voMamaDisappointed2,				//VO/VO_MAMA_Disappointed2.ogg
		* voMamaHappy,						//VO/VO_MAMA_Happy.ogg
		* voMushroomGrowl,					//VO/VO_MUSHROOM_growl.ogg
		* voMushroomTalkingAudible,			//VO/VO_MUSHROOM_Talking_Audible.ogg
		* voMushroomTalkingLong,			//VO/VO_MUSHROOM_Talking_Long.ogg
		* voMushroomTalkingVariation1,		//VO/VO_MUSHROOM_Talking_Variation1.ogg
		* voMushroomTalkingVariation2,		//VO/VO_MUSHROOM_Talking_Variation2.ogg
		* voTomotoAngry,					//VO/VO_TOMOTO_Angry.ogg
		* voTomotoTalking;					//VO/VO_TOMOTO_Talking.ogg


	//SETTINGS/////////////////////////////////////////////////////////////////
	float volumeBGM = 1.0f; //Between 0 and 1. The current BGM volume multiplier
	bool unmuteBGM = true; //Whether the BGM is unmuted or not
	
	float volumeSFX = 1.0f; //Between 0 and 1. The current SFX volume multiplier.
	bool unmuteSFX = true; //Whether the SFX is unmuted or not

	//FUNCTIONS////////////////////////////////////////////////////////////////
	void loadSounds(); //Mass call to fmodSystem->createSound()
	void unloadSounds(); //Mass call to sound->release()

	//toPlay  = the sound to play
	//ch      = The channel to play into. Default to main SFX channel
	//pan     = panning between -1 (left only) and 1 (right only). For no panning, input 0
	//vol     = volume. Positive. Is multiplied by volume settings
	//pitch   = Less than 1 results in it being slower and lower pitched. More than 1 results in otherwise. Set at 1 for unaltered.
	void playPannedSoundEffect(FMOD::Sound* toPlay, FMOD::Channel** ch = &GetInstance().chSFX, float pan = 0.0f, float vol = 1.0f, float pitch = 1.0f);

	//Kill a particular channel
	void killChannel(FMOD::Channel* target);

	//Easy change of BGM
	void changeBGM(FMOD::Sound* toChangeTo);
};

/*
//Error Logging for Audio
bool ErrorLog(const char* _function, const char* _file, int _line);
*/


/*
//Initalise FMOD SYSTEM (ONCE PER GAME)
void InitFMod();

//Create all sound data and channel
void FmodCreateSound();

//Play sound when needed, all sound will be released in sound release
void FmodPlayGameSound(int index);

//Update for FMOD engine
void FmodUpdate();

//Release memory for sound
void FmodSoundRelease();

//Release memory for FMOD SYSTEM
void FmodClose();
*/

//Macro directive to get the audio driver faster
#define audio AudioDriver::GetInstance()

#endif