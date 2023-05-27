#include "MainMenuScripts.h"

namespace TH
{

	//Press the play button to start playing
	void MMPlayButtonScript::OnButtonPressed()
	{
		BE::SceneMgr->LoadScene("Level1");
	}

	//-------------------------------------------------------------------------------------

	//Press the quit button to quit
	void MMQuitButtonScript::OnButtonPressed()
	{
		BE::SceneMgr->QuitScene();
	}

	//-------------------------------------------------------------------------------------

	//Go to credits
	void MMCreditsButtonScript::Init()
	{
		mainMenuCanvas = BE::UI->FindUICanvasesByName("Main_Page", 1).front();
		creditsCanvas = BE::UI->FindUICanvasesByName("Credits_Page", 1).front();
	}

	void MMCreditsButtonScript::OnButtonPressed()
	{
		BE::UI->GetCanvas(mainMenuCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(creditsCanvas)->SetIsActive(true);
	}


	//-------------------------------------------------------------------------------------

	//Go to options
	void MMOptionsButtonScript::Init()
	{
		mainMenuCanvas = BE::UI->FindUICanvasesByName("Main_Page", 1).front();
		optionsCanvas = BE::UI->FindUICanvasesByName("Options_Page", 1).front();
	}

	void MMOptionsButtonScript::OnButtonPressed()
	{
		BE::UI->GetCanvas(mainMenuCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(optionsCanvas)->SetIsActive(true);
	}

	//-------------------------------------------------------------------------------------
	
	//Go to controls
	void MMControlsButtonScript::Init()
	{
		mainMenuCanvas = BE::UI->FindUICanvasesByName("Main_Page", 1).front();
		controlsCanvas = BE::UI->FindUICanvasesByName("Controls_Page", 1).front();
	}

	void MMControlsButtonScript::OnButtonPressed()
	{
		BE::UI->GetCanvas(mainMenuCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(controlsCanvas)->SetIsActive(true);
	}
	
	//-------------------------------------------------------------------------------------
	
	//Back button from controls/credits/options
	void BackButtonScript::Init()
	{
		mainMenuCanvas = BE::UI->FindUICanvasesByName("Main_Page", 1).front();
		creditsCanvas = BE::UI->FindUICanvasesByName("Credits_Page", 1).front();
		controlsCanvas = BE::UI->FindUICanvasesByName("Controls_Page", 1).front();
		optionsCanvas = BE::UI->FindUICanvasesByName("Options_Page", 1).front();
	}

	void BackButtonScript::Update(double dt)
	{
		if(BE::Input->IsKeyTriggered(BE::Key::ESCAPE))
			OnButtonPressed();
	}

	void BackButtonScript::OnButtonPressed()
	{
		BE::UI->GetCanvas(mainMenuCanvas)->SetIsActive(true);
		BE::UI->GetCanvas(controlsCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(creditsCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(optionsCanvas)->SetIsActive(false);
	}
	
	//-------------------------------------------------------------------------------------
	
	void MusicSliderController::Init()
	{
		BE::UI->GetCanvas(uiID.first)->GetUI<BE::UISlider>(uiID.second)->SetSliderPercentage(BE::AudioMgr->GetGlobalMusicVol());
	}
	
	void MusicSliderController::OnSliderChanged(float _val)
	{
		BE::AudioMgr->SetGlobalMusicVol(_val);
	}

	void SoundSliderController::Init()
	{
		BE::UI->GetCanvas(uiID.first)->GetUI<BE::UISlider>(uiID.second)->SetSliderPercentage(BE::AudioMgr->GetGlobalSoundVol());
	}

	void SoundSliderController::Update(double dt)
	{
		currDt = (float)dt;
	}

	void SoundSliderController::OnSliderChanged(float _val)
	{
		BE::AudioMgr->SetGlobalSoundVol(_val);

		nextPlaySoundTime -= currDt;

		if (nextPlaySoundTime < 0)
		{
			nextPlaySoundTime = 0.25f;
			auto slider = BE::UI->GetCanvas(uiID.first)->GetUI<BE::UISlider>(uiID.second);
			BE::AudioEvent ae(uiID.first, uiID.second, slider->GetAudioQue(BE::UIAudioQueType::pressed), BE::UIAudioQueType::pressed);
			BE::AudioProperties ap(slider->GetPitch(), slider->GetVolume(), slider->GetPriority(), false, false, MIN_AUDIO_DISTANCE, MAX_AUDIO_DISTANCE);
			BE::AudioMgr->PlayAudio(ae, ap);
		}
	}

	//-------------------------------------------------------------------------------------


	//Teehee Portrait animation
	void MMTeeheeAnimationScript::Init()
	{
		startPos[(size_t)Side::BotLeft] =  BE::Vec3f(0.17f,  1.2f, 0.0f);
		startPos[(size_t)Side::BotRight] = BE::Vec3f(0.83f,  1.2f, 0.0f);
		startPos[(size_t)Side::TopRight] = BE::Vec3f(0.83f, -0.2f, 0.0f);
		startPos[(size_t)Side::TopLeft] =  BE::Vec3f(0.17f, -0.2f, 0.0f);

		monkeyPortrait = BE::AssetMgr->GetAssetID("../Assets/Textures/UI_Assets/MainMenu/TeeheePortrait.dds");
		monkeyPortraitFlipped = BE::AssetMgr->GetAssetID("../Assets/Textures/UI_Assets/MainMenu/TeeheePortrait_Flipped.dds");
	}

	void MMTeeheeAnimationScript::Update(double dt)
	{
		t += (float)dt;
		std::shared_ptr<BE::UIImage> image = BE::UI->GetCanvas(uiID.first)->GetUI<BE::UIImage>(uiID.second);

		//If t exceeds the time, then subtract by that time
		if (t >= oneCycleTime)
		{
			t -= oneCycleTime;

			//change to another position
			switch (currSide)
			{
				case Side::BotLeft:		
				{
					currSide = Side::BotRight;
					image->SetTexture(monkeyPortraitFlipped);
					break;
				}

				case Side::BotRight:
				{
					currSide = Side::TopRight;
					BE::Vec3f scale = image->GetUIScale();
					scale *= -1.0f;
					image->SetUIScale(scale);
					image->SetTexture(monkeyPortrait);
					break;
				}

				case Side::TopRight:
				{
					currSide = Side::TopLeft;
					image->SetTexture(monkeyPortraitFlipped);
					break;
				}

				case Side::TopLeft:
				{
					currSide = Side::BotLeft;
					BE::Vec3f scale = image->GetUIScale();
					scale *= -1.0f;
					image->SetUIScale(scale);
					image->SetTexture(monkeyPortrait);
					break;
				}
			}
		}

		float addedPos = (a * BEMath::CosRad(b * t) + c) * translate;
		
		//Set position
		//Top side
		if (currSide == Side::TopLeft || currSide == Side::TopRight)
			image->SetUIPos(startPos[(size_t)currSide] + BE::Vec3f(0.0f, addedPos, 0.0f));
		//Bottom side
		else
			image->SetUIPos(startPos[(size_t)currSide] - BE::Vec3f(0.0f, addedPos, 0.0f));
	}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;

	registration::class_<MMPlayButtonScript>(RegistrationNameCreator<MMPlayButtonScript>())
		.constructor();

	registration::class_<MMQuitButtonScript>(RegistrationNameCreator<MMQuitButtonScript>())
		.constructor();

	registration::class_<MMCreditsButtonScript>(RegistrationNameCreator<MMCreditsButtonScript>())
		.constructor();

	registration::class_<MMOptionsButtonScript>(RegistrationNameCreator<MMOptionsButtonScript>())
		.constructor();

	registration::class_<MMControlsButtonScript>(RegistrationNameCreator<MMControlsButtonScript>())
		.constructor();

	registration::class_<BackButtonScript>(RegistrationNameCreator<BackButtonScript>())
		.constructor();

	registration::class_<MusicSliderController>(RegistrationNameCreator<MusicSliderController>())
		.constructor();

	registration::class_<SoundSliderController>(RegistrationNameCreator<SoundSliderController>())
		.constructor();

	registration::class_<MMTeeheeAnimationScript>(RegistrationNameCreator<MMTeeheeAnimationScript>())
		.constructor();
}