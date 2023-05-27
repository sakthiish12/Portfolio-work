#include "CDOTriggers.h"



namespace TH
{

	void CDOTriggers::Init() 
	{
		m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
		//m_player = BE::ECS->FindEntitiesByTag("Player", 1).front();

		//Assert that it triggers something type
#ifdef USE_DEBUGGER
		for (const Trigger& t : m_triggers)
		{
			Assert(t.m_type < TriggerType::last, "CDO Trigger is triggering invalid types");
		}
#endif
	}


	void CDOTriggers::OnCollision(BE::BEID collidingID)
	{
		//If collided with the player and not yet activated
		if (m_isActivated || collidingID != BE::GLS->GetPlayerID())
			return;

		m_isActivated = true;
		BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);

		for (const Trigger& t : m_triggers)
		{
			switch (t.m_type)
			{
				case TriggerType::cutscene:
				{
					std::shared_ptr<CutsceneController> ptr = gl.GetScriptDerived<CutsceneController>();
					ptr->StartCutscene(t.m_Index);

					// for spawning boss decoy when showing cutscene
					if (t.m_Index == 4) {
						const auto decoyModel = BE::ECS->FindEntitiesByName("Boss Decoy Model", 1);
						if (!decoyModel.empty() && BE::ECS->HasComponent<BE::Renderer>(decoyModel.front())) {
							BE::ECS->GetComponent<BE::EntityInfo>(decoyModel.front()).SetFlag(BE::EntityFlag::active, true);
						}

						const auto decoy = BE::ECS->FindEntitiesByName("Boss Decoy", 1);
						if (!decoy.empty() && BE::ECS->HasComponent<BE::Physics>(decoy.front())) {
							BE::ECS->GetComponent<BE::Physics>(decoy.front()).SetIsTrigger(false);
						}
					}
					break;
				}
				case TriggerType::dialogue:
				{
					std::shared_ptr<DialogueController> ptr = gl.GetScriptDerived<DialogueController>();
					ptr->StartDialogue(t.m_Index);
					break;
				}
				case TriggerType::objective:
				{
					std::shared_ptr<ObjectivesController> ptr = gl.GetScriptDerived<ObjectivesController>();
					ptr->StartObjectives(t.m_Index);
					if (t.m_Index == 0)
					{
						BE::UI->GetCanvas(BE::UI->FindUICanvasesByName("TutorialCanvas", 1).front())->SetIsActive(true);
					}
					break;
				}
				default:
				{
					Assert(t.m_type < TriggerType::last, "CDO Trigger is triggering invalid types");
					break;
				}

			}
		}
	}





	void ObjectivesUIDisabler::Init()
	{
		m_objTopLeft_UIID = BE::UI->FindUIElementByName("ObjectiveTopLeft", 1).front();
	}

	void ObjectivesUIDisabler::OnCollision(BE::BEID collidingID)
	{
		//If collided with the player and not yet activated
		if (m_isActivated || collidingID != BE::GLS->GetPlayerID())
			return;

		m_isActivated = true;
		BE::UI->GetUI<BE::UIImage>(m_objTopLeft_UIID.first, m_objTopLeft_UIID.second)->SetUIElementFlag(BE::UIElementFlags::isActive, false);
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;

	registration::enumeration<TriggerType>(RegistrationNameCreator<TriggerType>())
		(
			value("Cutscene", TriggerType::cutscene),
			value("Dialogue", TriggerType::dialogue),
			value("Objective", TriggerType::objective)
		);

	registration::class_<Trigger>(RegistrationNameCreator<Trigger>())
		.constructor()
		.property("IndexToActivate", &Trigger::m_Index)
		.property("TypeToActivate", &Trigger::m_type);

	registration::class_<CDOTriggers>(RegistrationNameCreator<CDOTriggers>())
		.constructor()
		.property("Triggers", &CDOTriggers::m_triggers);

	registration::class_<ObjectivesUIDisabler>(RegistrationNameCreator<ObjectivesUIDisabler>())
		.constructor();

}