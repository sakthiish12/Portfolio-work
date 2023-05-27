#pragma once
#include <BananaEngine.h>

namespace TH
{
	class CutsceneController : public BE::IScript
	{
		//CanvasID of the cutscenes, number of panels for the cutscene depends on the number of UIElements in that canvas
		std::vector<BE::CanvasID> m_cutsceneSets{};

		//Panel fade delay, how much time since the cutscene started will the panel shows up?
		//Outher vector is the idx to the canvas
		std::vector<std::vector<float>> m_cutscenePanelFadeInDelay{};

		//The set of panels for the currently running cutscene
		std::vector<BE::UIElementID> m_cutscenePanels{};

		//The cutscene background and the "press space to continue"
		std::pair<BE::CanvasID, BE::UIElementID> m_cutsceneBGUI{};
		BE::CanvasID m_cutscenePressSpaceUI{};

		//How much time passed since the cutscene started
		float m_timeElapsed = 0.0f;

		//The current idx of the cutscene panel fading
		size_t m_currentPanel{ std::numeric_limits<size_t>::max() };

		//The idx of the cutscene set to run
		size_t m_cutsceneSet{ std::numeric_limits<size_t>::max() };

		BE::EntityID BGMPlayer = BE::NULL_ENTITY;
		/*
			CONSTANTS
		*/
		//Cutscene scrolling speed
		const float m_verticalScrollSpeed = -0.1f;

		//Fast scroll speed
		const float m_fastScrollMultiplier = 3.0f;
		
		//Amount of time to fade in the panel
		const float fadeTime = 0.65f;

	public:
		void Awake() override;
		void Init() override;
		void Update(double dt) override;

		//Start running the cutscene
		void StartCutscene(size_t _cutsceneSet);

		//Whether any panels remaining need to show
		bool HasPanelsRemaining() const;

		//Whether there is still cutscenes running
		bool HasCutsceneRunning() const;

	public:

		RTTR_ENABLE(IScript);
		RTTR_REGISTRATION_FRIEND;
	};
}