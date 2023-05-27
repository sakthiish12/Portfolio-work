#include "InGameUIScripts.h"
#include "../../BehaviourScripts/Camera/CameraController.h"
#include "../../BehaviourScripts/Level/Checkpoint.h"

namespace TH
{

	void IGResumeGameButtonScript::Awake()
	{
		m_gameUICanvas = BE::UI->FindUICanvasesByName("GameUI", 1).front();
	}

	void IGResumeGameButtonScript::OnButtonPressed()
	{
		BE::UI->GetCanvas(m_gameUICanvas)->SetIsActive(true);
		BE::UI->GetCanvas(uiID.first)->SetIsActive(false);
		BE::Input->SetEnableCursor(false);
		BE::PauseControl::SetPause(false);
	}

	//----------------------------------------------------------------------------------

	void IGRestartButtonScript::OnButtonPressed()
	{
		BE::SceneMgr->RestartScene();
	}

	//----------------------------------------------------------------------------------
	
	void IGControlsButtonScript::Awake()
	{
		m_controlsUICanvas = BE::UI->FindUICanvasesByName("Controls_Page", 1).front();
	}

	void IGControlsButtonScript::OnButtonPressed()
	{
		//Hide the pause menu
		BE::UI->GetCanvas(uiID.first)->SetIsActive(false);
		BE::UI->SaveFontState();

		//Show the controls UI canvas
		BE::UI->GetCanvas(m_controlsUICanvas)->SetIsActive(true);
	}

	//----------------------------------------------------------------------------------

	void IGOptionsButtonScript::Awake()
	{
		m_optionsUICanvas = BE::UI->FindUICanvasesByName("Options_Page", 1).front();
	}

	void IGOptionsButtonScript::OnButtonPressed()
	{
		//Hide the pause menu
		BE::UI->GetCanvas(uiID.first)->SetIsActive(false);
		BE::UI->SaveFontState();

		//Show the controls UI canvas
		BE::UI->GetCanvas(m_optionsUICanvas)->SetIsActive(true);
	}

	//----------------------------------------------------------------------------------

	void IGQuitGameButtonScript::OnButtonPressed()
	{
		BE::SceneMgr->LoadScene("MainMenu");
	}

	//----------------------------------------------------------------------------------
	
	void IGBackButtonScript::Awake()
	{
		m_pauseUICanvas = BE::UI->FindUICanvasesByName("PauseCanvas", 1).front();
	}


	void IGBackButtonScript::OnButtonPressed()
	{
		//Show the pause menu
		BE::UI->GetCanvas(m_pauseUICanvas)->SetIsActive(true);

		//Restore the fonts
		BE::UI->RestoreFontState();

		//Hide the controls UI canvas
		BE::UI->GetCanvas(uiID.first)->SetIsActive(false);
	}



	void IGRetryButtonScript::Awake()
	{
		m_gameMgr = BE::ECS->FindEntitiesByName("GameManager", 1).front();
	}

	void IGRetryButtonScript::OnButtonPressed()
	{
		BE::ECS->GetComponent<BE::GameLogic>(m_gameMgr).GetScriptDerived<CheckpointManager>()->RestoreCheckpoint();
	}


	
	//----------------------------------------------------------------------------------

	void IGAbilityOuterRingAnimation::Init()
	{
		originalSize = BE::UI->GetCanvas(uiID.first)->GetUI<UIImage>(uiID.second)->GetUIScale();
		tScale = 0.0f;
		tRot = 0.0f;

		b = (float)BE::Math::PI_TIMES_2 / oneCycleScaleTime;
		d = (float)BE::Math::PI_TIMES_2 / oneCycleRotTime;
	}
	
	void IGAbilityOuterRingAnimation::Update(double dt)
	{
		tScale += (float)dt;
		tRot += (float)dt;

		std::shared_ptr<BE::UIImage> image = BE::UI->GetCanvas(uiID.first)->GetUI<BE::UIImage>(uiID.second);

		//If t exceeds the time, then subtract by that time
		if (tScale >= oneCycleScaleTime)
			tScale -= oneCycleScaleTime;

		if (tRot >= oneCycleRotTime)
			tRot -= oneCycleRotTime;

		BE::Vec3f newSize = (a * BEMath::CosRad(b * tScale) + c) * sizeIncrease;

		//Set size
		image->SetUIScale(originalSize + newSize);

		//Set rotation
		image->SetUIRotRad(tRot * d);
	}	


	//----------------------------------------------------------------------------------

	void IGGameOverLevelCompleteAnimation::Init()
	{
		currAnimationStartDelay = 0.0f;
		currAnimationCompleteTime = 0.0f;
		currFadeTime = 0.0f;

		isInDelay = false;

		//Get the canvas and go through all the children
		//auto& canvas = BE::UI->GetCanvas(uiID.first);

		//For each children
		for (BE::UIElementID uid : BE::UI->GetCanvas(uiID.first)->GetAllUIElements())
		{
			//If it's the background itself
			if (uid == uiID.second)
			{
				//Reset opacity
				BE::UI->GetUI<BE::UIImage>(uiID.first, uiID.second)->setOpacity(0.0f);
			}
			//If it's the banner
			else if (BE::UI->GetUI(uiID.first, uid)->GetUIType() == BE::UIElementType::image)
			{
				//Reset position
				Vec3f newPos = BE::UI->GetUI(uiID.first, uid)->GetUIPos();
				newPos.y = bannerStartingYPos;
				BE::UI->GetUI(uiID.first, uid)->SetUIPos(newPos);
			}
			//If it's a button
			else if (BE::UI->GetUI(uiID.first, uid)->GetUIType() == BE::UIElementType::button)
			{
				Vec3f newPos = BE::UI->GetUI(uiID.first, uid)->GetUIPos();
				newPos.y = buttonStartingYPos;
				BE::UI->GetUI(uiID.first, uid)->SetUIPos(newPos);
			}
		}
	}

	void IGGameOverLevelCompleteAnimation::Update(double dt)
	{
		//Fades in the black background
		if (currFadeTime < bgFadeTime)
		{
			currFadeTime += (float)dt;
			//Update opacity of the background
			BE::UI->GetUI<BE::UIImage>(uiID.first, uiID.second)
				  ->setOpacity(BEMath::Min(currFadeTime/bgFadeTime * bgOpacityFinal, bgOpacityFinal));	
		}

		//Delay other animations for a while
		if (currAnimationStartDelay < animationStartDelay)
		{
			currAnimationStartDelay += (float)dt;
			return;
		}

			
		if (currAnimationCompleteTime >= animationCompleteTime)
			return;

		//Start doing animation
		currAnimationCompleteTime += (float)dt;

		//For each children
		for (BE::UIElementID uid : BE::UI->GetCanvas(uiID.first)->GetAllUIElements())
		{
			//If it's the background itself
			if (uid == uiID.second)
			{
				//Ignore it
				continue;
			}
			//If it's the banner
			else if (BE::UI->GetUI(uiID.first, uid)->GetUIType() == BE::UIElementType::image)
			{
				//Reset position
				Vec3f newPos = BE::UI->GetUI(uiID.first, uid)->GetUIPos();
				
				if (currAnimationCompleteTime >= animationCompleteTime)
				{
					newPos.y = bannerEndingYPos;
				}
				else
				{
					newPos.y = bannerStartingYPos + (bannerEndingYPos - bannerStartingYPos) 
								* BEMath::SinRad<float>((2.0f * (float)BEMath::PI * currAnimationCompleteTime) / (4.0f * animationCompleteTime));
				}

				BE::UI->GetUI(uiID.first, uid)->SetUIPos(newPos);
			}
			//If it's a button
			else if (BE::UI->GetUI(uiID.first, uid)->GetUIType() == BE::UIElementType::button)
			{
				Vec3f newPos = BE::UI->GetUI(uiID.first, uid)->GetUIPos();

				if (currAnimationCompleteTime >= animationCompleteTime)
				{
					newPos.y = buttonEndingYPos;
				}
				else
				{
					newPos.y = buttonStartingYPos + (buttonEndingYPos - buttonStartingYPos)
						* BEMath::SinRad<float>((2.0f * (float)BEMath::PI * currAnimationCompleteTime) / (4.0f * animationCompleteTime));
				}

				BE::UI->GetUI(uiID.first, uid)->SetUIPos(newPos);
			}
		}

	}



	//void IGBlackBGFadingController::Awake()
	//{
	//	blackscreen = BE::UI->FindUIElementByName("CutsceneBG", 1).front();
	//}

	//void IGBlackBGFadingController::Update(double dt)
	//{
	//	if (!isTransitioning)
	//		return;

	//	float currOpacity = BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->getOpacity();

	//	//Going from 0% to 100%
	//	if (isFadingIn)
	//	{
	//		currOpacity += fadingRatePerSecond * (float)dt;

	//		if (currOpacity >= 1.0f)
	//		{
	//			isTransitioning = false;
	//			currOpacity = 1.0f;
	//		}

	//		BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->setOpacity(currOpacity);
	//	}
	//	//Going from 100% to 0%
	//	else
	//	{
	//		currOpacity -= fadingRatePerSecond * (float)dt;

	//		if (currOpacity <= 0.0f)
	//		{
	//			isTransitioning = false;
	//			currOpacity = 0.0f;
	//			BE::UI->RestoreFontState();
	//		}

	//		BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->setOpacity(currOpacity);
	//	}

	//}

	//void IGBlackBGFadingController::StartFadingIn(float _fadingInRate)
	//{
	//	Assert(_fadingInRate > 0.0f, "Cannot have fading rate <= 0");

	//	isTransitioning = true;
	//	isFadingIn = true;
	//	fadingRatePerSecond = _fadingInRate;
	//	BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->setOpacity(0.0f);
	//	BE::UI->SaveFontState();
	//}

	//void IGBlackBGFadingController::StartFadingOut(float _fadingOutRate)
	//{
	//	Assert(_fadingOutRate > 0.0f, "Cannot have fading rate <= 0");

	//	isTransitioning = true;
	//	isFadingIn = false;
	//	fadingRatePerSecond = _fadingOutRate;
	//	BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->setOpacity(1.0f);
	//}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	
	registration::class_<IGAbilityOuterRingAnimation>(RegistrationNameCreator<IGAbilityOuterRingAnimation>())
		.constructor()
		.property("oneCycleScaleTime", &IGAbilityOuterRingAnimation::oneCycleScaleTime)
		.property("sizeIncrease", &IGAbilityOuterRingAnimation::sizeIncrease)
		.property("oneCycleRotTime", &IGAbilityOuterRingAnimation::oneCycleRotTime);
	
	registration::class_<IGResumeGameButtonScript>(RegistrationNameCreator<IGResumeGameButtonScript>())
		.constructor();

	registration::class_<IGRestartButtonScript>(RegistrationNameCreator<IGRestartButtonScript>())
		.constructor();

	registration::class_<IGControlsButtonScript>(RegistrationNameCreator<IGControlsButtonScript>())
		.constructor();

	registration::class_<IGOptionsButtonScript>(RegistrationNameCreator<IGOptionsButtonScript>())
		.constructor();
	
	registration::class_<IGQuitGameButtonScript>(RegistrationNameCreator<IGQuitGameButtonScript>())
		.constructor();

	registration::class_<IGBackButtonScript>(RegistrationNameCreator<IGBackButtonScript>())
		.constructor();

	registration::class_<IGRetryButtonScript>(RegistrationNameCreator<IGRetryButtonScript>())
		.constructor();

	registration::class_<IGGameOverLevelCompleteAnimation>(RegistrationNameCreator<IGGameOverLevelCompleteAnimation>())
		.constructor();

	//registration::class_<IGBlackBGFadingController>(RegistrationNameCreator<IGBlackBGFadingController>())
	//	.constructor();
}