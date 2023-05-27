// /******************************************************************************/
// /*!
// \file         SoundManager.h
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the old sound manager
//				 It is deprecated and should not be used
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#if 0
#include <iostream>
#include <fmod.hpp>
#include <vector>
//extern bool bgplaying = true;
enum SoundTestIndex
{
	suction = 0,
	button,
	walk,
	gunshot,
	bitten,
	switchSkills,
	background,
	bossMusic,
	bridge,
	teleport,
	enemyDeath,
	beginningCutscene,
	blenderScene,
	fightingtheboss,
	lever,
	throneRoom,
	winSound,
	airBlast1,
	airBlast2,
	airBlast3,
	airBlast4,
	bridgeExtension,
	lose,
	CaveBgm,
	GrassWalking1,
	GrassWalking2,
	GrassWalking3,
	TeleportAppear,
	MushroomDig,
	ShieldBreak,
	FallingRocks,
	CaveWalking1,
	CaveWalking2,
	CaveWalking3,
	LoseBGM,
	Teleporter,
	bridgeExtension2

};
extern bool bgmuted ;
extern bool Thronemuted ;
extern bool level2muted ;
extern bool sfxmuted;
class SoundClass
{
public:
	//static SoundClass* Instance;
	//static std::shared_ptr<SoundClass> Instance;
	std::vector<FMOD::Sound*> sounds;
	std::vector<FMOD::Channel*> channels;
	
	static SoundClass* getInstance();
	FMOD::System* soundsystem;

	 SoundClass();
	 ~SoundClass();
	 void Update();
	 void LoadCreateSounds();
	 void setArgsAndPlay(int id, const float*);
	 void Release();
	 void HybridCreateSounds(int id, const char* filename, bool loop, int volume);
	 void SoundTest();
     void CreateSound(unsigned int soundID, const char* filename, bool Loop);
	 void PlayAgain(int id);
	 void Pause(int id);
	 void Stop(int id);
	 int CreateChannel(int id, bool boundtocharacter);
	 void setVolume(int id, float value);
	 void setSpeed(int id, float value);
	 void updatesystem();
	 void cleanchannels();
	
};

 //SoundClass;

#endif
#endif // !SOUNDMANAGER_h
