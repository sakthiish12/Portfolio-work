#include "Triggers.h"
#include "../Character/CharacterScript.h"
#include "../../DialogueCutsceneObjectivesControl/ObjectivesController.h"
#include "../../DialogueCutsceneObjectivesControl/DialogueController.h"
#include "../../DialogueCutsceneObjectivesControl/CutsceneController.h"
#include "../../UserInterface/InGameUIScripts/TutorialUIController.h"
#include "../Boss/RobotBoss.h"
namespace TH
{
	void EndPoint::OnCollision(EntityID collidingID)
	{
		if (once) return;
		if (collidingID == GLS->GetPlayerID())
		{
			auto game = ECS->FindEntitiesByName("GameManager");
			if (game.size()) {
				once = true;
				if (ECS->HasComponent<GameLogic>(game.front())) {
					auto ptr = ECS->GetComponent<GameLogic>(game.front()).GetScript<LevelManager>();
					if (ptr != nullptr) {
						LevelManager* gameptr = dynamic_cast<LevelManager*>(ptr.get());
						//camptr->SetMouseDisabled(false);
						gameptr->ShowLevelComplete();
					}
				}
			}
		}
	}
	void Teleporter::OnCollision(EntityID collidingID)
	{
		if (ECS->GetComponent<EntityInfo>(id).HasFlag(BE::EntityFlag::active) && collidingID == GLS->GetPlayerID()) {
			auto ptr = ECS->GetComponent<GameLogic>(collidingID).GetScript<VirtualCharacter>();
			if (ptr) {
				VirtualCharacter* temp = dynamic_cast<VirtualCharacter*>(ptr.get());
				if (temp) {
					temp->cCharacter->SetPosition(JPHtoBE(EndLocation));
					temp->cCharacter->SetRotation(JPHtoBE(Quatf(EndRotation.x, EndRotation.y, EndRotation.z)));
					TutorialUIController::inst->SetTutorialCanvasEnabled(false);
				}
			}

			if (!ShownCutscene) {
				// start cutscene
				EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
				BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
				std::shared_ptr<CutsceneController> cutsceneptr = gl.GetScriptDerived<CutsceneController>();
				cutsceneptr->StartCutscene(CutsceneIndex);
				ShownCutscene = true;
			}

		}
	}
	void DoorUnlockTrigger::OnCollision(EntityID collidingID)
	{
		if (collidingID == GLS->GetPlayerID())
		{
			for (auto eid : doors)
			{
				auto ptr = ECS->GetComponent<GameLogic>(eid).GetScript<DoorScript>();
				if (ptr) {
					DoorScript* temp = dynamic_cast<DoorScript*>(ptr.get());
					if (temp) {
						temp->SetDoorLock(false);
						temp->OpenDoors();
						temp->SetDoorLock(true);
					}
				}
				for (auto cid : ECS->GetComponent<Transform>(eid).GetChildren()) {
					if (ECS->HasComponent<lightComponent>(cid)) {
						ECS->GetComponent<lightComponent>(cid).setColor({ 0.f, 1.f, 0.f });
					}
				}
			}

			if (!ShownDialogue) {
				EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
				BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
				std::shared_ptr<DialogueController> ptr = gl.GetScriptDerived<DialogueController>();
				ptr->StartDialogue(3);
				ShownDialogue = true;
			}

			const auto SecRoomRobots = ECS->FindEntitiesByName("Enemy_Robot3");
			for (auto eid : SecRoomRobots) {
				auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(eid).GetScript<CharacterScript>();
				CharacterScript* charac = dynamic_cast<CharacterScript*>(script_ptr.get());
				if (charac) {
					charac->SetTarget(GLS->GetPlayerID());
				}
			}
			const auto SecRoomRanged = ECS->FindEntitiesByName("RangedRobot3");
			for (auto eid : SecRoomRanged) {
				auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(eid).GetScript<CharacterScript>();
				CharacterScript* charac = dynamic_cast<CharacterScript*>(script_ptr.get());
				if (charac) {
					charac->SetTarget(GLS->GetPlayerID());
				}
			}
		}
	}

	void AggroPlayerTrigger::OnCollision(EntityID collidingID)
	{
		if (collidingID == GLS->GetPlayerID())
		{
			for (auto eid : ECS->FindEntitiesByTag(tagGroup, 10))
			{
				auto ptr = ECS->GetComponent<GameLogic>(eid).GetScript<CharacterScript>();
				if (ptr) {
					CharacterScript* temp = dynamic_cast<CharacterScript*>(ptr.get());
					if (temp) {
						temp->SetTarget(GLS->GetPlayerID());
					}
				}
			}
		}
	}

	void BossRoomTrigger::Init()
	{
		const auto bosslist = ECS->FindEntitiesByName("Boss FireWall", 1);
		if (bosslist.empty()) {
			LogCoreError("Error finding Boss FireWall entity!");
		}
		else {
			boss = bosslist.front();
		}

	}
	void BossRoomTrigger::OnCollision(EntityID collidingID)
	{
		if (!HasPlayed && collidingID == GLS->GetPlayerID())
		{
			HasPlayed = true;
			if (ECS->DoesEntityExists(boss)) {
				auto bossptr = std::dynamic_pointer_cast<RobotBoss>(ECS->GetComponent<GameLogic>(boss).GetScript<RobotBoss>());
				if (bossptr) {
					bossptr->StartBossVoiceOver();
				}
			}
		}
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<EndPoint>(RegistrationNameCreator<EndPoint>())
		.constructor();
	registration::class_<Teleporter>(RegistrationNameCreator<Teleporter>())
		.constructor()
		.property("Teleport to Position", &Teleporter::EndLocation)
		.property("Teleport with Rotation", &Teleporter::EndRotation);
	registration::class_<DoorUnlockTrigger>(RegistrationNameCreator<DoorUnlockTrigger>())
		.constructor();
	registration::class_<AggroPlayerTrigger>(RegistrationNameCreator<AggroPlayerTrigger>())
		.constructor()
		.property("Enemy Tag Group", &AggroPlayerTrigger::tagGroup);
}