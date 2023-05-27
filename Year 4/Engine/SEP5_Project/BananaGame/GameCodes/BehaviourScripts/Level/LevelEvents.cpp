#include "../Character/CharacterScript.h"
#include "../Enemies/EnemyScript.h"
#include "../Character/CharacterPather.h"
#include "../Enemies/EnemyRobot.h"
#include "LevelEvents.h"
#include "../Environment/Door.h"
#include "../../DialogueCutsceneObjectivesControl/ObjectivesController.h"
#include "../../DialogueCutsceneObjectivesControl/DialogueController.h"

namespace TH
{
	using namespace BE;

	void SpawnRobot::ActivateResolution()
	{
		for (size_t i = 0; i < Locations.size(); i++)
		{
			EntityID pid = ECS->SpawnPrefab(AssetMgr->GetAssetID("../Assets/Prefabs/MeleeRobot.prefab"));
			Transform& transf = ECS->GetComponent<Transform>(pid);
			transf.SetPosition(Locations[i]);

			EntityID mpid = ECS->SpawnPrefab(AssetMgr->GetAssetID("../Assets/Prefabs/RobotModel.prefab"));
			Transform& mtransf = ECS->GetComponent<Transform>(mpid);
			mtransf.SetPosition(Locations[i]);
			mtransf.AttachParent(pid);
			mtransf.SetScale({ 3.f, 3.f, 3.f });
			mtransf.SetLocalPosition({ 0.f, -0.55f, 0.f });
			mtransf.SetLocalEulerRotation({ -90.f, 0.f, 0.f });
		}
	}

	void UnlockDoors::ActivateResolution()
	{
		for (auto eid : tTargetIDs)
		{
			auto ptr = ECS->GetComponent<GameLogic>(eid).GetScript<DoorScript>();
			if (ptr) {
				DoorScript* temp = dynamic_cast<DoorScript*>(ptr.get());
				if (temp)
					temp->SetDoorLock(false);
			}
		}
	}

	void ObjectiveEvent::ActivateResolution()
	{
		EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();

		BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);

		std::shared_ptr<ObjectivesController> ptr = gl.GetScriptDerived<ObjectivesController>();
		ptr->StartObjectives(m_Index);
	}


	void UnlockDoorsAndObjective::ActivateResolution()
	{
		for (auto eid : tTargetIDs)
		{
			auto ptr = ECS->GetComponent<GameLogic>(eid).GetScript<DoorScript>();
			if (ptr) {
				DoorScript* temp = dynamic_cast<DoorScript*>(ptr.get());
				if (temp)
					temp->SetDoorLock(false);
			}
		}
		EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
		BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
		std::shared_ptr<ObjectivesController> ptr = gl.GetScriptDerived<ObjectivesController>();
		ptr->StartObjectives(m_Index);
	}

	void UnlockDoorsAndDialogue::ActivateResolution() 
	{
		for (auto eid : tTargetIDs)
		{
			auto ptr = ECS->GetComponent<GameLogic>(eid).GetScript<DoorScript>();
			if (ptr) {
				DoorScript* temp = dynamic_cast<DoorScript*>(ptr.get());
				if (temp)
					temp->SetDoorLock(false);
			}
			
		}
		EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
		BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
		std::shared_ptr<DialogueController> ptr = gl.GetScriptDerived<DialogueController>();
		ptr->StartDialogue(m_Index);
	}
}