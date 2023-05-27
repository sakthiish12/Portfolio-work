#pragma once
#include <BananaEngine.h>

namespace TH
{
	/*
		------------------------- All the button scripts for the main menu ----------------------
	*/

	//Main menu to play
	class MMPlayButtonScript : public BE::IScript
	{
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Main menu to quit
	class MMQuitButtonScript : public BE::IScript
	{
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Main menu to credits
	class MMCreditsButtonScript : public BE::IScript
	{
		BE::CanvasID mainMenuCanvas;
		BE::CanvasID creditsCanvas;

		void Init() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class MMOptionsButtonScript : public BE::IScript
	{
		BE::CanvasID mainMenuCanvas;
		BE::CanvasID optionsCanvas;

		void Init() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class MMControlsButtonScript : public BE::IScript 
	{
		BE::CanvasID mainMenuCanvas;
		BE::CanvasID controlsCanvas;

		void Init() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Credits/Options/Controls to main menu
	class BackButtonScript : public BE::IScript
	{
		BE::CanvasID mainMenuCanvas;
		BE::CanvasID creditsCanvas;
		BE::CanvasID optionsCanvas;
		BE::CanvasID controlsCanvas;

		void Init() override;
		void Update(double dt) override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Controls the music slider
	class MusicSliderController : public BE::IScript
	{
		void Init() override;
		void OnSliderChanged(float _val) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Controls the sound slider
	class SoundSliderController : public BE::IScript
	{
		float nextPlaySoundTime = 0.25f;
		float currDt = 0.0f;
		

		void Init() override;
		void Update(double dt) override;
		void OnSliderChanged(float _val) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};



	class MMTeeheeAnimationScript : public BE::IScript
	{
		enum class Side
		{
			BotLeft = 0,
			BotRight,
			TopRight,
			TopLeft,

			last
		};

		BE::AssetID monkeyPortrait;
		BE::AssetID monkeyPortraitFlipped;

		//Which side the portrait will appear
		Side currSide = Side::BotLeft;

		//The starting position of the portrait
		std::array<BE::Vec3f, (size_t)Side::last> startPos;

		//Size properties will model [ a * cos([2pi/b] * t) + c ]
		const float oneCycleTime = 2.0f;									//For 1 cycle takes 1.5s
		const float a = -0.5f;
		const float b = (float)BE::Math::PI_TIMES_2 / oneCycleTime;
		const float c = 0.5f;

		//How much to translate
		const float translate = 0.34f;

		//Current time elapsed
		float t = 0.0f;

		void Init() override;
		void Update(double d) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}