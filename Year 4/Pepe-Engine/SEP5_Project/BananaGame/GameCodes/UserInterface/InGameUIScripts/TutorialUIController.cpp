#include "TutorialUIController.h"
#include "../../BehaviourScripts/Character/CharacterScript.h"
#include "../../DialogueCutsceneObjectivesControl/DialogueController.h"
bool turnOnVent = false;

namespace TH
{
	float ventTimer = 0.0f;
	TutorialUIController* TutorialUIController::inst;
	void TutorialUIController::Awake()
	{
		inst = this;
		turnOnVent = false;
		//Set reference to the Tutorial canvas
		m_tutorialCanvas = BE::UI->FindUICanvasesByName("TutorialCanvas", 1).front();

		//Set reference to the checkbox
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark1", 1).front());
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark2", 1).front());
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark3", 1).front());
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark4", 1).front());
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark5", 1).front());
		m_tutorialUICheckbox.push_back(BE::UI->FindUIElementByName("TutCheckmark6", 1).front());

		//Set reference to the tutorial text
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText1", 1).front());
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText2", 1).front());
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText3", 1).front());
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText4", 1).front());
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText5", 1).front());
		m_tutorialUIText.push_back(BE::UI->FindUIElementByName("TutText6", 1).front());

		completedCount = 0;
	}

	void TutorialUIController::Init()
	{
		//Turn on the canvas
		//BE::UI->GetCanvas(m_tutorialCanvas)->SetIsActive(true);

		//Turn on all the checkbox and tutorial text
		for (size_t i = 0; i < m_tutorialUICheckbox.size(); ++i)
		{
			//Turn off the toggle
			BE::UI->GetCanvas(m_tutorialUICheckbox[i].first)->GetUI<BE::UIToggle>(m_tutorialUICheckbox[i].second)->SetToggle(false);

			//Set opacity of the text
			BE::UI->GetCanvas(m_tutorialUIText[i].first)->GetUI<BE::UIImage>(m_tutorialUIText[i].second)->setOpacity(1.0f);
		}

		completedCount = 0;
	}
	inline void TutorialCompleteEvent()
	{
		using namespace BE;
		const auto tp = ECS->FindEntitiesByName("TPTrigger");
		ECS->GetComponent<EntityInfo>(tp.front()).SetFlag(EntityFlag::active, true);
		const EntityID target = ECS->FindEntitiesByName("VentPivot").front();
		EntityID pivot = ECS->FindEntitiesByName("VentPivot").front();
		AudioSource& as = ECS->GetComponent<AudioSource>(pivot);
		auto cameraCallback = [target,&as,pivot](float dt)
		{
			if(!as.IsAudioPlaying(AssetID(6652379327096777522)))
				ECS->GetComponent<AudioSource>(pivot).Play(AssetID(6652379327096777522));//VentOpen.wav
			ECS->GetComponent<Transform>(target).SetEulerRotation(Vec3f(0, 90, 0) + Vec3f(-90, 0, 0) * dt);
		};
		const EntityID targetting = ECS->FindEntitiesByName("VentTargetingPosition").front();
		CameraController::inst->SetTarget(ECS->GetComponent<Transform>(target).GetPosition(),
			ECS->GetComponent<Transform>(targetting).GetPosition(),
			ECS->GetComponent<Transform>(targetting).GetEulerRotation()
			, 0.5f,cameraCallback
		);
		DialogueController::inst->StartDialogue(1);
	}
	void TutorialUIController::Update(double dt)
	{
		UNUSED(dt);
		if (!turnOnVent && completedCount == 6)
		{
			TutorialCompleteEvent();
			turnOnVent = true;
		}

#if 0
		if (BE::Input->IsKeyTriggered(BE::Key::_Z) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(0, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_Z) && BE::Input->IsShiftPressed())
			SetTutorialComplete(0, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_X) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(1, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_X) && BE::Input->IsShiftPressed())
			SetTutorialComplete(1, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_C) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(2, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_C) && BE::Input->IsShiftPressed())
			SetTutorialComplete(2, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_V) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(3, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_V) && BE::Input->IsShiftPressed())
			SetTutorialComplete(3, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_B) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(4, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_B) && BE::Input->IsShiftPressed())
			SetTutorialComplete(4, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_N) && !BE::Input->IsShiftPressed())
			SetTutorialComplete(5, true);
		if (BE::Input->IsKeyTriggered(BE::Key::_N) && BE::Input->IsShiftPressed())
			SetTutorialComplete(5, false);

		if (BE::Input->IsKeyTriggered(BE::Key::_M) && !BE::Input->IsShiftPressed())
			SetTutorialCanvasEnabled(true);
		if (BE::Input->IsKeyTriggered(BE::Key::_M) && BE::Input->IsShiftPressed())
			SetTutorialCanvasEnabled(false);

		if (BE::Input->IsKeyTriggered(BE::Key::_L))
			std::cout << GetTotalCompletedCount() << std::endl;
#endif

	}


	void TutorialUIController::SetTutorialComplete(size_t _index, bool _b)
	{
		Assert(_index < m_tutorialUICheckbox.size(), "Chosen tutorial index %llu is out of range", _index);

		//If there is no change in state then no need to do anything
		bool originalIsActive = BE::UI->GetCanvas(m_tutorialUICheckbox[_index].first)
			->GetUI<BE::UIToggle>(m_tutorialUICheckbox[_index].second)
			->GetToggle();

		if (_b == originalIsActive)
			return;

		//Update the toggle value
		BE::UI->GetCanvas(m_tutorialUICheckbox[_index].first)->GetUI<BE::UIToggle>(m_tutorialUICheckbox[_index].second)->SetToggle(_b);

		//Update the text opacity and completeCount values

		//IF marking as complete
		if (_b)
		{
			++completedCount;
			BE::UI->GetCanvas(m_tutorialUIText[_index].first)->GetUI<BE::UIImage>(m_tutorialUIText[_index].second)->setOpacity(0.2f);
		}
		//Marking as incomplete
		else
		{
			--completedCount;
			BE::UI->GetCanvas(m_tutorialUIText[_index].first)->GetUI<BE::UIImage>(m_tutorialUIText[_index].second)->setOpacity(1.0f);
		}
	}

	void TutorialUIController::SetTutorialCanvasEnabled(bool _b) const
	{
		BE::UI->GetCanvas(m_tutorialCanvas)->SetIsActive(_b);
	}
}