#include "Level1.h"
#include "LevelManager.h"
#include "../../DialogueCutsceneObjectivesControl/CutsceneController.h"
#include <Environment/Interactable.h>
namespace TH
{
	using namespace BE;

	void LevelOne::Awake()
	{
		auto ptr = ECS->GetComponent<GameLogic>(id).GetScript<LevelScript>();
		if (ptr) {
			LevelScript* temp = dynamic_cast<LevelScript*>(ptr.get());
			if (temp) levelscript = temp;
		}
	}

	void LevelOne::Init()
	{
		if (levelscript) {

			LevelEvent event1;
			const auto room1 = ECS->FindEntitiesByName("Enemy_Robot2");
			for (auto eid : room1) {
				event1.eTriggerTargets.insert({ eid, false });
			}
			const auto doors1 = ECS->FindEntitiesByTag("DoorUnlock1");
			event1.eResolution = std::make_shared<UnlockDoors>();
			for (auto eid : doors1) {
				event1.eResolution->tTargetIDs.insert(eid);
			}
			event1.eType = EventTriggerType::CharacterDied;
			levelscript->AddEvent(event1);



			LevelEvent event2;
			event2.eType = EventTriggerType::InteractedFirstTime;
			const auto baby = ECS->FindEntitiesByName("Baby Monkey", 1);
			if (baby.size()) event2.eTriggerTargets.insert({ baby.front(), false });
			event2.eResolution = std::make_shared<ObjectiveEvent>();
			ObjectiveEvent* objective = dynamic_cast<ObjectiveEvent*>(event2.eResolution.get());
			if (objective) {
				objective->m_Index = 2;
			}
			levelscript->AddEvent(event2);

			LevelEvent event3;
			const auto securityEnemiesCollider = ECS->FindEntitiesByTag("EnemySecurityRoom");
			for (auto eid : securityEnemiesCollider) {
				if (ECS->HasComponent<GameLogic>(eid) && ECS->GetComponent<GameLogic>(eid).HasScript("EnemyBase"))
				{
					event3.eTriggerTargets.insert({ eid, false });
				}
				else
				{
					LogCoreError("Tagged entity is not a enemy");
				}
			}
			LogCore("Door Unlock 2 size %d", event3.eTriggerTargets.size());
			const auto doors3 = ECS->FindEntitiesByTag("DoorUnlock2");
			event3.eResolution = std::make_shared<UnlockDoors>();
			for (auto eid : doors3) {
				event3.eResolution->tTargetIDs.insert(eid);
			}
			event3.eType = EventTriggerType::CharacterDied;
			levelscript->AddEvent(event3);
			LevelEvent event5;
			const auto longEnemiesCollider = ECS->FindEntitiesByTag("EnemyLongRoom");
			for (auto eid : longEnemiesCollider) {
				if (ECS->HasComponent<GameLogic>(eid) && ECS->GetComponent<GameLogic>(eid).HasScript("EnemyBase"))
				{
					event5.eTriggerTargets.insert({ eid, false });
				}
				else
				{
					LogCoreError("Tagged entity is not a enemy");
				}
			}
			LogCore("Door Unlock 3 size %d", event5.eTriggerTargets.size());
			const auto longDoors = ECS->FindEntitiesByTag("DoorUnlock3");
			event5.eResolution = std::make_shared<UnlockDoors>();
			for (auto eid : longDoors) {
				event5.eResolution->tTargetIDs.insert(eid);
			}
			event5.eType = EventTriggerType::CharacterDied;
			levelscript->AddEvent(event5);



			LevelEvent event4;
			const auto room4melee = ECS->FindEntitiesByName("Enemy_Robot4");
			for (auto eid : room4melee) {
				event4.eTriggerTargets.insert({ eid, false });
			}
			const auto room4ranged = ECS->FindEntitiesByName("RangedRobot4");
			for (auto eid : room4ranged) {
				event4.eTriggerTargets.insert({ eid, false });
			}
			const auto doors4 = ECS->FindEntitiesByTag("DoorUnlock4");
			event4.eResolution = std::make_shared<UnlockDoorsAndObjective>();
			UnlockDoorsAndObjective* objective2 = dynamic_cast<UnlockDoorsAndObjective*>(event4.eResolution.get());
			if (objective2) {
				objective2->m_Index = 3;
			}
			for (auto eid : doors4) {
				event4.eResolution->tTargetIDs.insert(eid);
			}
			event4.eType = EventTriggerType::CharacterDied;
			levelscript->AddEvent(event4);

			LevelEvent event6;
			const auto serverEnemiesCollider = ECS->FindEntitiesByTag("EnemyServerRoom");
			for (auto eid : serverEnemiesCollider) {
				if (ECS->HasComponent<GameLogic>(eid) && ECS->GetComponent<GameLogic>(eid).HasScript("EnemyBase"))
				{
					event6.eTriggerTargets.insert({ eid, false });
				}
				else
				{
					LogCoreError("Tagged entity is not a enemy");
				}
			}
			LogCore("Console Interactable Trigger size %d", event6.eTriggerTargets.size());
			auto callback = std::make_shared<CustomCallback>();
			const auto consoleID = ECS->FindEntitiesByName("ConsoleCollider").front();
			Interactable& console = dynamic_cast<Interactable&>(*ECS->GetComponent<GameLogic>(
				consoleID).GetScript<Interactable>());
			console.disableInteraction = true;
			auto callbackFunction = [consoleID]()
			{
				Interactable& console = dynamic_cast<Interactable&>(*ECS->GetComponent<GameLogic>(
					consoleID).GetScript<Interactable>());
				console.disableInteraction = false;
			};
			callback->callback = callbackFunction;
			event6.eResolution = callback;
			event6.eType = EventTriggerType::CharacterDied;
			levelscript->AddEvent(event6);
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<LevelOne>(RegistrationNameCreator<LevelOne>())
		.constructor();
}