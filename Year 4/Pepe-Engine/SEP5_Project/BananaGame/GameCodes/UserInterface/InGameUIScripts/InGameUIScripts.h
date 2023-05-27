#pragma once
#include <BananaEngine.h>

namespace TH
{
	/*
		For pause screen
	*/
	//Pause Menu's resume button
	class IGResumeGameButtonScript : public BE::IScript
	{
		BE::CanvasID m_gameUICanvas;

		void Awake() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Pause menu's restart button
	class IGRestartButtonScript : public BE::IScript
	{
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Pause menu's control button
	class IGControlsButtonScript : public BE::IScript
	{
		BE::CanvasID m_controlsUICanvas;

		void Awake() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Pause menu's options button
	class IGOptionsButtonScript : public BE::IScript
	{
		BE::CanvasID m_optionsUICanvas;

		void Awake() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Pause menu quit button
	class IGQuitGameButtonScript : public BE::IScript
	{
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Control Page's back button
	class IGBackButtonScript : public BE::IScript
	{
		BE::CanvasID m_pauseUICanvas;

	public:
		void Awake() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//Controls the go back to last checkpoint "Retry button" in game over screen
	class IGRetryButtonScript : public BE::IScript
	{
		BE::EntityID m_gameMgr = BE::NULL_ENTITY;

	public:
		void Awake() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};






	/*
		Ability Icons and Menu
	*/
	class IGAbilityOuterRingAnimation : public BE::IScript
	{
		//Size properties will model [ a * cos([2pi/b] * t) + c ]
		float oneCycleScaleTime = 1.5f;									//For 1 cycle takes 1.5s
		BE::Vec3f sizeIncrease = BE::Vec3f{};							//Increase size by 0.05f
		const float a = -0.5f;
		float b = 0.0f;
		const float c = 0.5f;
		BE::Vec3f originalSize{};
		float tScale = 0.0f;

		//Rotation properties
		float oneCycleRotTime = 4.0f;									//For 1 rotation takes 4s
		float d = 0.0f;
		float tRot = 0.0f;

		void Init() override;
		void Update(double dt) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	
	/*
	 	Controls the level complete UI and gameover Ui, which is attached to the black screen object
	*/
	class IGGameOverLevelCompleteAnimation : public BE::IScript
	{
		bool isInDelay = false;
		
		//Will model  sin([2 * pi * t] / [4a] ) where a is animation completion time
		static constexpr float animationStartDelay = 1.0f;		
		static constexpr float animationCompleteTime = 0.8f;	
		
		//button animations
		static constexpr float buttonStartingYPos = 1.15f;
		static constexpr float buttonEndingYPos = 0.8f;

		//game over / level complete UI
		static constexpr float bannerStartingYPos = -0.2f;
		static constexpr float bannerEndingYPos = 0.5f;

		//Black background constants
		static constexpr float bgFadeTime = 1.5f;				
		static constexpr float bgOpacityFinal = 0.85f;			

		//Current times
		float currAnimationStartDelay = 0.0f;					
		float currAnimationCompleteTime = 0.0f;					
		float currFadeTime = 0.0f;								

	public:
		void Init() override;
		void Update(double dt) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	/*
		Controls camera fading in and out
	*/
	//class IGBlackBGFadingController : public BE::IScript
	//{
	//	bool isTransitioning = false;
	//	bool isFadingIn = false;
	//	std::pair<BE::CanvasID, BE::UIElementID> blackscreen;

	//	float fadingRatePerSecond = 1.0f;

	//public:
	//	void Awake() override;
	//	void Update(double dt) override;

	//	//Call once, opacity goes to 100%
	//	void StartFadingIn(float _fadingInRate);

	//	//Call once, opacity goes to 0%
	//	void StartFadingOut(float _fadingOutRate);

	//	//Disable pause
	//	inline bool ShouldDisablePause() const { return BE::UI->GetUI<BE::UIImage>(blackscreen.first, blackscreen.second)->getOpacity() > 0.0f; }

	//	RTTR_ENABLE(IScript)
	//	RTTR_REGISTRATION_FRIEND
	//};

}

