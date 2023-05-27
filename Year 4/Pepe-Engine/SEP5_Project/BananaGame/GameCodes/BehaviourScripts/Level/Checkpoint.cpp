#include "Checkpoint.h"
#include "../Character/CharacterScript.h"
#include "../Player/PlayerScript.h"
#include "../Boss/RobotBoss.h"

namespace TH
{
	void CheckpointManager::Awake()
	{
		//Find all the enemies that are active
		m_enemyEntities = std::move(BE::ECS->FindEntitiesByTag("Enemy", 4096, true));

		m_gameoverCanvas = BE::UI->FindUICanvasesByName("GameOverScreen", 1).front();

		baby_monkey = BE::ECS->FindEntitiesByName("Baby Monkey", 1).front();

		boss = BE::ECS->FindEntitiesByName("Boss FireWall", 1).front();
	}

	void CheckpointManager::SetCheckpoint(BE::EntityID _cp)
	{
		//Save all the enemies that are still active, i.e not yet defeated
		m_enemyEntities = std::move(BE::ECS->FindEntitiesByTag("Enemy", 4096, true));

		//Set the checkpoint
		Assert(BE::ECS->GetComponent<BE::EntityInfo>(_cp).HasTag("Checkpoint"), "Entity %llu is not a checkpoint entity", _cp);

		//Set the checkpoint
		m_lastCheckpointTriggered = _cp;
	}

	void CheckpointManager::RestoreCheckpoint()
	{
		//If no checkpoint set then just restart the whole level
		if (m_lastCheckpointTriggered == BE::NULL_ENTITY)
		{
			BE::SceneMgr->RestartScene();
			return;
		}

		//Reenable all the enemies that are dead after the checkpoint
		for (BE::EntityID eid : m_enemyEntities)
		{
			if (eid == boss) continue;
			BE::ECS->GetComponent<BE::EntityInfo>(eid).SetFlag(BE::EntityFlag::active, true);
			BE::Vec3f respawnPoint = BE::ECS->GetComponent<BE::GameLogic>(eid).GetScriptDerived<CharacterScript>()->GetSpawnPos();
			BE::ECS->GetComponent<BE::Transform>(eid).SetPosition(respawnPoint);
			BE::ECS->GetComponent<BE::Physics>(eid).SetIsTrigger(false);
			
			//Init all the scripts for the enemies
			for (const auto& script : BE::ECS->GetComponent<BE::GameLogic>(eid).GetAllScripts())
			{
				script->Init();
			}
			
		}
		auto boss_ptr = BE::ECS->GetComponent<BE::GameLogic>(boss).GetScriptDerived<RobotBoss>();
		if (boss_ptr && boss_ptr->HasStartedBoss()) {
			boss_ptr->Respawn();
		}

		//Run resurrection script for the player
		BE::Vec3f pos = BE::ECS->GetComponent<BE::Transform>(m_lastCheckpointTriggered).GetPosition();
		BE::Vec3f rot = BE::ECS->GetComponent<BE::Transform>(m_lastCheckpointTriggered).GetEulerRotation();
		rot.x = 0.0f; rot.z = 0.0f; rot.y = -rot.y;
		BE::ECS->GetComponent<BE::GameLogic>(GLS->GetPlayerID()).GetScriptDerived<PlayerCharacter>()->ResurretPlayer(pos, rot);

		//Player picks up baby again
		auto bby_ptr = BE::ECS->GetComponent<BE::GameLogic>(baby_monkey).GetScriptDerived<BabyMonkey>();
		if (bby_ptr) bby_ptr->ResetBabyToPlayer();

		//Unpause the game
		//PauseControl::SetPause(false);

		//Hide game over UI
		BE::UI->GetCanvas(m_gameoverCanvas)->SetIsActive(false);
		BE::UI->GetCanvas(m_gameoverCanvas)->InitCanvas();
		BE::UI->RestoreFontState();

		//Get the camera component and set mouse state to false
		BE::Input->SetEnableCursor(false);

		//Mark level as not showing game over
		BE::ECS->GetComponent<BE::GameLogic>(id).GetScriptDerived<LevelManager>()->Retry();

	}



	void Checkpoint::Awake() 
	{
		m_gameMgr = BE::ECS->FindEntitiesByName("GameManager", 1).front();
	}

	void Checkpoint::Init()
	{
		m_isUsed = false;
	}

	void Checkpoint::Update(double dt)
	{
		//If object used, or not being interacted, then return
		if (m_isUsed || !BE::ECS->GetComponent<BE::GameLogic>(id).GetScriptDerived<Interactable>()->GetInteracted())
			return;

		m_isUsed = true;
		
		BE::EntityID parentID = BE::ECS->GetComponent<BE::Transform>(id).GetParent();
		BE::EntityID deviceID = BE::NULL_ENTITY;

		//Get all the children of the parent
		for (BE::EntityID childID : BE::ECS->GetComponent<BE::Transform>(parentID).GetChildren())
		{
			//Find the checkpoint device
			if (BE::ECS->GetComponent<BE::EntityInfo>(childID).HasTag("Checkpoint"))
			{
				deviceID = childID;
				break;
			}
		}

		Assert(deviceID != BE::NULL_ENTITY, "DeviceID should not be null");

		std::shared_ptr<CheckpointManager> script = BE::ECS->GetComponent<BE::GameLogic>(m_gameMgr).GetScriptDerived<CheckpointManager>();
		script->SetCheckpoint(deviceID);		//Parent of the panel is the device itself
		m_isUsed = true;

		//Play sound
		auto triggerSound = BE::ECS->GetComponent<BE::AudioSource>(id).GetAllAudio().begin();
		BE::ECS->GetComponent<BE::AudioSource>(id).Play(triggerSound->first);

		//Change the emissive map of the panel - ResurrectionControlPanel_DefaultMaterial_Emissive
		BE::ECS->GetComponent<BE::Renderer>(id).setEmissiveMap(BE::AssetID(16340833697196766194), 0);
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<CheckpointManager>(RegistrationNameCreator<CheckpointManager>())
		.constructor();

	registration::class_<Checkpoint>(RegistrationNameCreator<Checkpoint>())
		.constructor();
}