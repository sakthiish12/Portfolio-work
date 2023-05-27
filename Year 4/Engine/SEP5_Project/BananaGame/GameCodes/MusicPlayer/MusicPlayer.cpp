#include "MusicPlayer.h"

namespace TH
{
	IGMusicPlayer* IGMusicPlayer::inst;
	void IGMusicPlayer::Awake()
	{
		inst = this;
		gameBGM = BE::AssetMgr->GetAssetID("../Assets/AudioClips/BGM/BattleMusicBGM.wav");
		bossBGM = BE::AssetMgr->GetAssetID("../Assets/AudioClips/BGM/FireWallTheme.wav");
		cutsceneBGM = BE::AssetMgr->GetAssetID("../Assets/AudioClips/BGM/CutsceneBGM.wav");

		originalGameBGMVol = BE::ECS->GetComponent<BE::AudioSource>(id).GetVolume(gameBGM);
		originalCutsceneBGMVol = BE::ECS->GetComponent<BE::AudioSource>(id).GetVolume(cutsceneBGM);
		originalBossBGMVol = BE::ECS->GetComponent<BE::AudioSource>(id).GetVolume(bossBGM);
	}
	void IGMusicPlayer::Init()
	{
		//Start playing music
		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.Play(gameBGM);					//BattleMusicBGM.wav
		as.SetVolume(0, gameBGM);
		as.Play(cutsceneBGM);				//BattleMusicBGM.wav
	}

	void IGMusicPlayer::Update(double dt)
	{
		//If no fading is happening then just return
		if (!isFading)
			return;

		currFadeTime += (float)dt;
		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		
		float fadeRatio = BEMath::Clamp<float>(0, 1, currFadeTime / fadingTime);

		//If game BGM is getting louder, i.e. exiting cutscene
		if (isGameBGMFadingIn)
		{
			as.SetVolume(fadeRatio * originalGameBGMVol, gameBGM);
			as.SetVolume((1.0f - fadeRatio) * originalCutsceneBGMVol, cutsceneBGM);
		}
		//If game BGM is getting softer, i.e. entering cutscene
		else
		{
			as.SetVolume((1.0f - fadeRatio) * originalGameBGMVol, gameBGM);
			as.SetVolume(fadeRatio * originalCutsceneBGMVol, cutsceneBGM);
		}

		//After reaching the fading time
		if (currFadeTime > fadingTime)
		{
			isFading = false;

			//If Game BGM is getting louder, i.e. exiting the cutscene, then stop the cutscene
			if(isGameBGMFadingIn)
				as.Stop(cutsceneBGM);

			if (!firstCutsceneRan)
				firstCutsceneRan = true;
		}
	}
	 
	void IGMusicPlayer::Cleanup()
	{
		//Stop playing music
		BE::ECS->GetComponent<BE::AudioSource>(id).StopAllAudio();
	}


	void IGMusicPlayer::StartBossMusic()
	{
		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.Stop(gameBGM);		//BattleMusicBGM.wav
		as.Play(bossBGM);		//FireWallTheme.wav
		as.SetVolume(originalBossBGMVol, bossBGM);
	}

	void IGMusicPlayer::StartGameMusic()		//FireWallTheme.wav
	{
		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.Stop(bossBGM);		//FireWallTheme.wav
		as.Play(gameBGM);		//BattleMusicBGM.wav
		as.SetVolume(originalGameBGMVol, gameBGM);
	}


	void IGMusicPlayer::StartFadingInCutscene()
	{
		//If first cutscene has not ran, then just skip the rest of the stuff
		if (!firstCutsceneRan)
			return;

		isFading = true;
		isGameBGMFadingIn = false;
		currFadeTime = 0.0f;

		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.SetVolume(originalGameBGMVol, gameBGM);
		as.SetVolume(0.0f, cutsceneBGM);

		as.Play(cutsceneBGM);
	}

	void IGMusicPlayer::StartFadingOutCutscene()
	{
		isFading = true;
		isGameBGMFadingIn = true;
		currFadeTime = 0.0f;

		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.SetVolume(0.0f, gameBGM);
		as.SetVolume(originalCutsceneBGMVol, cutsceneBGM);

	}

	//======================================================================================

	void MMMusicPlayer::Init()
	{
		//Start playing music

		BE::AudioSource& as = BE::ECS->GetComponent<BE::AudioSource>(id);
		as.Play(as.GetAllAudio().begin()->first);
	}

	void MMMusicPlayer::Cleanup()
	{
		//Stop playing music
		BE::ECS->GetComponent<BE::AudioSource>(id).StopAllAudio();
	}


}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<IGMusicPlayer>(RegistrationNameCreator<IGMusicPlayer>())
		.constructor();

	registration::class_<MMMusicPlayer>(RegistrationNameCreator<MMMusicPlayer>())
		.constructor();
}
