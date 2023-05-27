#pragma once
#include <BananaEngine.h>

namespace TH
{
	//Plays in game music
	class IGMusicPlayer : public BE::IScript
	{
		//The asset ID of all the BGM
		BE::AssetID gameBGM = BE::NULL_ASSET;
		BE::AssetID bossBGM = BE::NULL_ASSET;
		BE::AssetID cutsceneBGM = BE::NULL_ASSET;

		//The original volume of the game and cutscene music, and the fading time
		float originalGameBGMVol = 0;
		float originalCutsceneBGMVol = 0;
		float originalBossBGMVol = 0;

		//Has the first cutscene ran?
		bool firstCutsceneRan = false;
		
		//Fading times
		static constexpr float fadingTime = 2.0f;		//Takes 1.5s to fade from 0 to original vol, and vice versa
		float currFadeTime = 0;
		bool isFading = false;
		bool isGameBGMFadingIn = false;					//True means BGM music is getting louder, False means getting softer

		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		void Cleanup() override;

	public:

		void StartBossMusic();		//BattleMusicBGM.wav
		void StartGameMusic();		//FireWallTheme.wav

		void StartFadingInCutscene();		//Transition from game music to cutscene music
		void StartFadingOutCutscene();		//Transition from cutscene music to game music


		static IGMusicPlayer* inst;
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	//Plays main menu music
	class MMMusicPlayer : public BE::IScript
	{
		void Init() override;
		void Cleanup() override;
	public:
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}