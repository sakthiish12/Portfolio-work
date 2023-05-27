#include "DialogueController.h"
#include "../MusicPlayer/MusicPlayer.h"
namespace TH
{
	DialogueController* DialogueController::inst;
	BE::AssetID currentDialougeAudio = BE::NULL_ENTITY;
	inline const std::unordered_map<BE::BEID::IDType, BE::AssetID> dialogueToAudioFile
	{
		{10490901327220546571u,BE::BEID(5989399499413804103)},
		{6127718305137174847u,BE::BEID(1674820362315021364)},
		{8777257673336226675u,BE::BEID(12072854050109443631)},
		{12346298112454029651u,BE::BEID(5728910493575496982)},
		{8562414301453828395u,BE::BEID(6642762375399989463)},
	};
	inline float GetShowTime(BE::AssetID id)
	{
		using namespace BE;
		switch (*id)
		{
		//Dialogue Sequence 1 start
		case 10490901327220546571u:
			return 1.75f;
		case 10177613899769329736u:
			return 2.5f;
		case 15005932803736110985u:
			return 3.44f;
		case 12014657957254358643u:
			return 1.6f;
		case 5549178850497316513:
			return 3.63f;
		case 11339706108779959322:
			return 2.77f;
		//Dialogue Sequence 1 end,Dialogue Sequence 2 start
		case 6127718305137174847u:
			return 2.82f;
		case 17547657000168751225:
			return 2.62f;
		//Dialogue Sequence 2 end,Dialogue Sequence 3 start
		case 8777257673336226675u:
			return 2.75f;
		case 2185973688413639239u:
			return 2.0f;
		//Dialogue Sequence 3 end,Dialogue Sequence 4 start
		case 12346298112454029651u:
			return 1.87f;
		case 14065333755724280157u:
			return	2.42f;
		case 2755140670931502438u:
			return	4.252f;
		//Dialogue Sequence 4 end,Dialogue Sequence 5 start
		case 8562414301453828395u:
			return 1.74f;
		case 2719714462473500273u:
			return 5.56f;

		default:
			return 4.0f;
			break;
		}
	}
	void DialogueController::Awake()
	{
		inst = this;
	}
	void DialogueController::Init()
	{
		using namespace BE;
		dialogueUIID = BE::UI->FindUIElementByName("Dialogue", 1).front();
		dialogueShowTimeRemaining = 0.0f;
		AudioSource& as = ECS->GetComponent<AudioSource>(IGMusicPlayer::inst->GetID());
		for (const auto& itr : dialogueToAudioFile)
		{
			as.AddAudio(itr.second);
			as.SetVolume(0.3f, itr.second);
		}
#ifdef USE_DEBUGGER
		//Assert that the UI Elements exist
		for (size_t i = 0; i < m_dialogues.size(); ++i)
		{
			for (size_t j = 0; j < m_dialogues[i].size(); ++j)
			{
				Assert(BE::AssetMgr->IsAssetDataLoaded(m_dialogues[i][j]), "Asset %llu not loaded", m_dialogues[i][j]);
			}
		}

		//Assert the UI exists
		Assert(BE::UI->DoesUICanvasExist(dialogueUIID.first), 
			"UI Canvas %llu does not exist", dialogueUIID.first);
		Assert(BE::UI->DoesUIElementExist(dialogueUIID.first, dialogueUIID.second), 
			"UI Element %llu does not exist in Canvas %llu", dialogueUIID.second, dialogueUIID.first);

#endif
	}

	void DialogueController::Update(double dt)
	{
		//If no dialogue is running then don't do anything
		if (!IsDialogueShowing() || BE::PauseControl::GetPause())
			return;

		dialogueShowTimeRemaining += (float)dt * BE::EngineSpeed->GetEngineSpeedMultiplier();//FIXME:Bad stuff

		//Showed the dialogue for long enough, then show the next dialogue
		if (dialogueShowTimeRemaining > GetShowTime(CurrentDialogueToDisplay()))
		{
			dialogueShowTimeRemaining = 0.0f;

			//change to next dialogue
			if(autoEnd || m_dialogueToShow < m_dialogues[m_dialogueSetToShow].size() - 1)
				++m_dialogueToShow;

			//If there still is dialogue left to show, then show
			if (IsDialogueShowing())
			{
				BE::UI->GetCanvas(dialogueUIID.first)->GetUI<BE::UIImage>(dialogueUIID.second)->SetTexture(CurrentDialogueToDisplay());
				if (dialogueToAudioFile.contains(*CurrentDialogueToDisplay()))
				{
					using namespace BE;
					AudioSource& as = ECS->GetComponent<AudioSource>(IGMusicPlayer::inst->GetID());
					if (as.IsAudioPlaying(currentDialougeAudio))
						as.Stop(currentDialougeAudio);
					currentDialougeAudio = dialogueToAudioFile.at(*CurrentDialogueToDisplay());
					as.Play(currentDialougeAudio);
				}
			}
				
			else if(autoEnd)
				BE::UI->GetCanvas(dialogueUIID.first)->GetUI<BE::UIImage>(dialogueUIID.second)->SetTexture(BE::BEID(0));
		}
	}

	//Starting the dialogue at the given index
	void DialogueController::StartDialogue(size_t _dialogueSet, bool _autoEnd)
	{
		Assert(_dialogueSet < m_dialogues.size(), "Dialogue Set %llu does not exist", _dialogueSet);
		Assert(!m_dialogues[_dialogueSet].empty(), "The dialogue set %llu is empty", _dialogueSet);

		//Set to that dialogue
		m_dialogueSetToShow = _dialogueSet;
		m_dialogueToShow = 0;
		dialogueShowTimeRemaining = 0.0f;

		autoEnd = _autoEnd;

		//Get the Dialogue UI and set the first dialogue
		BE::UI->GetCanvas(dialogueUIID.first)->GetUI<BE::UIImage>(dialogueUIID.second)->SetTexture(CurrentDialogueToDisplay());
		if (dialogueToAudioFile.contains(*CurrentDialogueToDisplay()))
		{
			using namespace BE;
			AudioSource& as = ECS->GetComponent<AudioSource>(IGMusicPlayer::inst->GetID());
			if (currentDialougeAudio != NULL_ASSET && as.IsAudioPlaying(currentDialougeAudio))
				as.Stop(currentDialougeAudio);
			currentDialougeAudio = dialogueToAudioFile.at(*CurrentDialogueToDisplay());
			as.Play(currentDialougeAudio);
		}
	}

	//Automatically ends the current dialogue
	void DialogueController::EndDialogue()
	{
		BE::UI->GetCanvas(dialogueUIID.first)->GetUI<BE::UIImage>(dialogueUIID.second)->SetTexture(BE::BEID(0));
		m_dialogueToShow = std::numeric_limits<size_t>::max();
		m_dialogueSetToShow = std::numeric_limits<size_t>::max();
	}

	//Whether any dialogue to run
	bool DialogueController::IsDialogueShowing() const
	{
		return m_dialogueSetToShow < m_dialogues.size() && m_dialogueToShow < m_dialogues[m_dialogueSetToShow].size();
	}

	//The current dialogue to show
	BE::AssetID DialogueController::CurrentDialogueToDisplay() const
	{
		Assert(m_dialogueSetToShow < m_dialogues.size(), "No dialogue set chosen, please use StartDialogue()");
		Assert(m_dialogueToShow < m_dialogues[m_dialogueSetToShow].size(), "Dialogue out of range");

		return m_dialogues[m_dialogueSetToShow][m_dialogueToShow];
	}
}



RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<DialogueController>(RegistrationNameCreator<DialogueController>())
		.constructor()
		.property("DialogueSets", &DialogueController::m_dialogues);
}