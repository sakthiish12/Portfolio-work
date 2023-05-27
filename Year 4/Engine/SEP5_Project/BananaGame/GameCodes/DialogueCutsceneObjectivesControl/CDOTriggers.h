#pragma once
#include <BananaEngine.h>
#include "CutsceneController.h"
#include "DialogueController.h"
#include "ObjectivesController.h"
//CDO = Cutscene, Dialogue, Objectives

namespace TH
{
	enum class TriggerType
	{
		cutscene = 0,
		dialogue,
		objective,

		last
	};

	struct Trigger
	{
		TriggerType m_type = TriggerType::last;			//Is it trigger C/D/O
		size_t m_Index = 0;								//Index of the C/D/O to trigger
	};


	class CDOTriggers : public BE::IScript 
	{
	private:
		std::vector<Trigger> m_triggers;
		bool m_isActivated = false;						//If the trigger has been activated before

		BE::EntityID m_cdoController{ BE::BEID(0) };	//Reference to the CDO controller
		//BE::EntityID m_player{ BE::BEID(0) };			//Reference to the player

	public:

		void Init() override;
		void OnCollision(BE::BEID collidingID) override;

		inline bool IsActivated() const						{ return m_isActivated; }

		RTTR_ENABLE(IScript);
		RTTR_REGISTRATION_FRIEND;
	};

	class ObjectivesUIDisabler : public BE::IScript
	{
		std::pair<BE::CanvasID, BE::UIElementID> m_objTopLeft_UIID{};
		bool m_isActivated = false;						//If the trigger has been activated before

	public:
		void Init() override;
		void OnCollision(BE::BEID collidingID) override;
	};
}