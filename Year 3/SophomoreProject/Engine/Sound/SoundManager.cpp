// /******************************************************************************/
// /*!
// \file         SoundManager.cpp
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

#if 0
#include <iostream>
#include "SoundManager.h"
#include "../Debugging/Log.h"//logger
#include <fmod_errors.h>
#include <functional>
#include <thread>
bool bgmuted = false;
bool sfxmuted = false;
int counter = 0;
SoundClass::SoundClass()
{
	unsigned int  Size = 1024;
	int  Buffer= 1024;

	//Create the main system
	FMOD::System_Create(&soundsystem);

	//Init FMOD
	soundsystem->init(64, NULL, NULL);
	soundsystem->getDSPBufferSize(&Size, &Buffer);
	Log log;
	log.Info(true,"Creating Sound system\n");
	sounds.resize(64);
	channels.resize(64);
	
	//Load sounds
	SoundTest();
}

void SoundClass::Update() 
{
	soundsystem->update();
}

SoundClass::~SoundClass()
{
	
}

SoundClass* SoundClass::getInstance()
{
	static SoundClass Instance;
	return &Instance;
}

void SoundClass::CreateSound(unsigned int soundID, const char* filename, bool Loop)
{
	Log log;
	log.Info(true, "Created Sound\n");
	log.Info(true, filename);
	std::string newFile(filename);
	newFile = "Resources/Sounds/" + newFile;

	FMOD::Sound* sound;
	if (!Loop)
		soundsystem->createSound(newFile.c_str(), FMOD_LOOP_OFF | FMOD_3D, NULL, &sound);
	else
		soundsystem->createSound(newFile.c_str(), FMOD_LOOP_NORMAL | FMOD_3D, NULL, &sound);
	//sounds.push_back(sound);
	sounds[soundID] = sound;
}

void SoundClass::PlayAgain(int id)
{
	//FMOD::Channel* channel;
	channels[id]->setPaused(true);
	channels[id]->stop();
	channels[id]->setPosition(0, FMOD_TIMEUNIT_MS);
	soundsystem->playSound(sounds[id], NULL, false, &channels[id]);
}


void SoundClass::SoundTest()
{
	CreateSound(suction, "NewVacuumSound.wav", true);//0
	CreateChannel(suction, true);

	CreateSound(button, "ButtonClick.wav", false);//1
	CreateChannel(button, false);

	CreateSound(walk, "WalkingPlayer1.wav", true);//2
	CreateChannel(walk, true);

	CreateSound(gunshot, "gunshot.wav", false);//3
	CreateChannel(gunshot, false);

	CreateSound(bitten, "Enemy Bite.wav", false);//4
	CreateChannel(bitten, true);

	CreateSound(switchSkills, "skills.mp3", false);//4
	CreateChannel(switchSkills, true);

	CreateSound(background, "GameplayBGM_1.wav", true);//4
	CreateChannel(background, true);
	setSpeed(background, 0.8f);
	setVolume(background, 0.2f);

	CreateSound(bossMusic, "Main Menu Music.wav", false);//4
	CreateChannel(bossMusic, true);
	
	CreateSound(enemyDeath, "EnemyDeath.wav", false);//4
	CreateChannel(enemyDeath, true);

	CreateSound(beginningCutscene, "BeginningCutscene.wav", false);//4
	CreateChannel(beginningCutscene, true);

	CreateSound(blenderScene, "BlenderScene.wav", false);//4
	CreateChannel(blenderScene, true);	
	
	CreateSound(fightingtheboss, "Fighting the boss.wav", false);//4
	CreateChannel(fightingtheboss, true);	

	CreateSound(lever, "Lever.wav", false);//4
	CreateChannel(lever, true);	

	CreateSound(throneRoom, "ThroneRoom (Kettle Keep).wav", true);//4
	CreateChannel(throneRoom, true);	

	CreateSound(winSound, "Win Screen.wav", false);//4
	CreateChannel(winSound, true);	

	CreateSound(airBlast1, "AirBlast 1.wav", false);//4
	CreateChannel(airBlast1, true);

	CreateSound(airBlast2, "AirBlast 2.wav", false);//4
	CreateChannel(airBlast2, true);

	CreateSound(airBlast3, "AirBlast 3.wav", false);//4
	CreateChannel(airBlast3, true);

	CreateSound(airBlast4, "AirBlast 4.wav", false);//4
	CreateChannel(airBlast4, true);

	CreateSound(bridgeExtension, "BridgeExtension.flac", false);//4
	CreateChannel(bridgeExtension, true);

	CreateSound(lose, "Lose.wav", false);//4
	CreateChannel(lose, true);

	CreateSound(CaveBgm, "CaveBackground.wav", true);//4
	CreateChannel(CaveBgm, true);
	
	CreateSound(GrassWalking1, "GrassWalking.wav", false);//4
	CreateChannel(GrassWalking1, true);
	
	CreateSound(GrassWalking2, "GrassWalking2.wav", false);//4
	CreateChannel(GrassWalking2, true);
	
	CreateSound(GrassWalking3, "GrassWalking3.wav", false);//4
	CreateChannel(GrassWalking3, true);
	
	CreateSound(TeleportAppear, "TeleportAppear.wav", false);//4
	CreateChannel(TeleportAppear, true);
	
	CreateSound(MushroomDig, "MushroomAppearFromGround.wav", false);//4
	CreateChannel(MushroomDig, true);
		
	CreateSound(ShieldBreak, "ShieldBreaking.wav", false);//4
	CreateChannel(ShieldBreak, true);
		
	CreateSound(FallingRocks, "FallingRocks.wav", true);//4
	CreateChannel(FallingRocks, true);
		
	CreateSound(CaveWalking1, "CaveWalking.wav", false);//4
	CreateChannel(CaveWalking1, true);
		
	CreateSound(CaveWalking2, "CaveWalking2.wav", false);//4
	CreateChannel(CaveWalking2, true);
			
	CreateSound(CaveWalking3, "CaveWalking3.wav", false);//4
	CreateChannel(CaveWalking3, true);
				
	CreateSound(LoseBGM, "LoseBGM.wav", false);//4
	CreateChannel(LoseBGM, true);

	CreateSound(Teleporter, "Teleporter.wav", false);//4
	CreateChannel(Teleporter, true);

	CreateSound(bridgeExtension2, "BridgeExtension.flac", false);//4
	CreateChannel(bridgeExtension2, true);

	//channels[walk]->setPaused(false);
	const float pos[] = { 0.f,0.f,0.f };
	//setArgsAndPlay(walk, pos);
	
	
}

int SoundClass::CreateChannel(int id, bool boundtocharacter)
{
	//cleanchannels();
	FMOD::Channel* channel;
	soundsystem->playSound(sounds[id], NULL, true, &channel);
	if (boundtocharacter)
		channel->set3DLevel(0);
	//channels.push_back(channel);
	channels[id] = channel;
	return static_cast<int>(channels.size()) - 1;
}

void SoundClass::setVolume(int id, float value)
{
	channels[id]->setVolume(value);
}

void SoundClass::setSpeed(int id, float value)
{
	channels[id]->setPitch(value);
}

void SoundClass::updatesystem()
{
	//getInstance()->soundsystem->update();
	//soundsystem->update();
	soundsystem->update();

	//static int TestVar = 0;
	//if (++TestVar > 120)
	//{
	//	const float pos[] = { 0.f,0.f,0.f };
	//	PlayAgain(gunshot);
	//	TestVar -= 120;
	//}
}

void SoundClass::cleanchannels()
{
	int i = 0;
	while (i < channels.size())
	{
		bool state;
		channels[i]->getPaused(&state);
		if (state)
		{
			channels[i]->stop();
			channels.erase(channels.begin() + i);
			i--;
		}
		i++;
	}
	
}

void SoundClass::HybridCreateSounds(int id, const char* filename, bool loop, int volume)
{
	static_cast<void>(loop);
	int SoundChannelNo;
	const char* file;
	int vol = 0;
	vol = volume;
	SoundChannelNo = id;
	file = filename;
	//SoundClass::getInstance()->CreateSound(file, true);
	SoundClass::getInstance()->CreateChannel(SoundChannelNo, false);
	SoundClass::getInstance()->setVolume(SoundChannelNo, (float)vol);

}

void SoundClass::LoadCreateSounds()
{


}

void SoundClass::setArgsAndPlay(int id, const float* Position)
{
	FMOD_VECTOR pos = { Position[0],Position[1],Position[2] };
	channels[id]->set3DAttributes(&pos, NULL);
	channels[id]->setPaused(false);
}

void SoundClass::Pause(int id) 
{
	channels[id]->setPaused(true);
}
void SoundClass::Stop(int id)
{
	channels[id]->stop();
}
void SoundClass::Release() 
{
	 for (int i = 0; i < sounds.size(); i++)
	 {
		 this->sounds[i]->release();
		 sounds[i]->release();
	 }
	 {
		 cleanchannels();
		 //this->channels[i]->release();
	 }
	 soundsystem->close();
	 soundsystem->release();
	 if (&soundsystem)
	 {
		 soundsystem->release();
		 soundsystem = nullptr;
		 delete soundsystem;
	 }

}

#endif //#if 0