#pragma once
#include <BananaEngine.h>

namespace TH
{
	class ObjectivesController : public BE::IScript
	{
		//the objective texture to display, the middle and top left ones
		std::vector<BE::AssetID> m_objectivesTopLeftAssets{};
		std::vector<BE::AssetID> m_objectivesMiddleAssets{};

		//Get instance to the top left obj UI and middle obj UI
		std::pair<BE::CanvasID, BE::UIElementID> m_objTopLeft_UIID{};
		std::pair<BE::CanvasID, BE::UIElementID> m_objMiddle_UIID{};

		//The current obj to show
		size_t m_currObj{ std::numeric_limits<size_t>::max() };

		//time control - how much time to show the middle obj
		const float bigObjShowTime = 5.0f;
		float		bigObjShowTimeRemaining = 0.0f;


	public:
		void Init() override;
		void Update(double dt) override;

		//Starting the dialogue at the given index
		void StartObjectives(size_t _objSet);

		//Whether any dialogue to run
		bool IsObjectivesShowing() const;

		//The current dialogue to show
		BE::AssetID CurrentDialogueToDisplay() const;

		//Clear the current objectives
		void ClearObjectives();

		RTTR_ENABLE(IScript);
		RTTR_REGISTRATION_FRIEND;
	};
}