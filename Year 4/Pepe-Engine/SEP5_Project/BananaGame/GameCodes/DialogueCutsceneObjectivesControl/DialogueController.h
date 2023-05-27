#pragma once
#include <BananaEngine.h>

namespace TH
{
	class DialogueController : public BE::IScript
	{
		//the dialogue texture to display
		std::vector<std::vector<BE::AssetID>> m_dialogues{};

		//Dialogue to show
		size_t m_dialogueSetToShow{ std::numeric_limits<size_t>::max() };	//The set of dialogue to run through, 
		size_t m_dialogueToShow{ std::numeric_limits<size_t>::max() };		//The current dialogue in the given set to play

		//Get instance to the dialogue UI
		std::pair<BE::CanvasID, BE::UIElementID> dialogueUIID{};

		//time control - how much time to show the middle obj
		const float dialogueShowTime = 4.0f;
		float		dialogueShowTimeRemaining = 0.0f;

		//Whether the dialogue that was starting is going to auto end
		bool		autoEnd = true;

	public:
		static DialogueController* inst;
		void Awake() override;
		void Init() override;
		void Update(double dt) override;

		//Starting the dialogue at the given index
		void StartDialogue(size_t _dialogueSet, bool _autoEnd = true);

		//Ends the dialogue immediately
		void EndDialogue();

		//Whether any dialogue to run
		bool IsDialogueShowing() const;

		//The current dialogue to show
		BE::AssetID CurrentDialogueToDisplay() const;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}