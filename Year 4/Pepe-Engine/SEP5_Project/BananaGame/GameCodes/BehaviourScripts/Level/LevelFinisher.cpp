#include "LevelFinisher.h"
#include "LevelManager.h"
#include "../../DialogueCutsceneObjectivesControl/CDOTriggers.h"

namespace TH
{
	void LevelFinisher::Init()
	{
		//Assert(BE::ECS->GetComponent<BE::EntityInfo>(id).GetName() == "Cutscene4Trigger", "Level Finisher script not attached to Cutscene4Trigger");

		gameManager = BE::ECS->FindEntitiesByName("GameManager", 1).front();
		cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
	}

	void LevelFinisher::Update(double dt)
	{
		
	}


	void LevelFinisher::OnCollision(BE::EntityID collidingID)
	{
		// if not player, return
		if( collidingID != GLS->GetPlayerID()) return;

		//If cutscene is not activated then don't do anything
		std::shared_ptr<CDOTriggers> cdoTrig = BE::ECS->GetComponent<BE::GameLogic>(id).GetScriptDerived<CDOTriggers>();
		if (cdoTrig && !cdoTrig->IsActivated())
			return;

		//If cutscene is running then don't do anything
		std::shared_ptr<CutsceneController> csCtrl = BE::ECS->GetComponent<BE::GameLogic>(cdoController).GetScriptDerived<CutsceneController>();
		if (csCtrl->HasCutsceneRunning())
			return;

		if (!levelFinished)
		{
			levelFinished = true;
			std::shared_ptr<LevelManager> lvlMgr = BE::ECS->GetComponent<BE::GameLogic>(gameManager).GetScriptDerived<LevelManager>();
			lvlMgr->ShowLevelComplete();
		}
	}
}