#include "BabyMonkey.h"
#include "PlayerScript.h"
#include "../../UserInterface/InGameUIScripts/AbilityMenu.h"
#include "../../DialogueCutsceneObjectivesControl/CDOTriggers.h"
#include "../Level/AreaZone.h"
#include "../Level/LevelManager.h"

namespace TH
{
	using namespace BE;
	glm::vec2 cryTimerRange{ 5.0f,9.0f };
	const std::vector<AssetID> crySFX{
		AssetID(13342839652874937072),//BabyCry1.wav
		AssetID(10400503852312599108)//BabyCry3.wav
	};
	BabyMonkey::BabyFSM BabyMonkey::fsm;
	template<typename T>
	void Baby_HidingState<T>::Enter(T& in)
	{
		cryTimer = Rand::Random(cryTimerRange.x, cryTimerRange.y);
	}
	template <typename T>
	void Baby_HidingState<T>::Update(T& script, double dt)
	{
		if (script.interact_base->GetInteracted()) {
			script.player->SetAnimState(PlayerAnim::PickUpBaby);
			script.CalledPickup = true;
		}
		if (!script.HasNoStress()) {
			script.AutoDecreasingStress(static_cast<float>(dt));
		}
		if (!DialogueController::inst->IsDialogueShowing())
		{
			cryTimer -= static_cast<float>(dt);
			if (cryTimer < 0.0f)
			{
				AudioSource& as = ECS->GetComponent<AudioSource>(script.id);
				as.StopAllAudio();
				as.Play(crySFX[Rand::Random<size_t>(0, crySFX.size() - 1)]);
				cryTimer = Rand::Random(cryTimerRange.x, cryTimerRange.y);
			}
		}
	}

	template <typename T>
	void Baby_RunningState<T>::Enter(T& script)
	{
		script.ToggleStartStopOverstressed(true);
		script.Random_Assign_HidingSpot();
		script.interact_base->ResetInteracted();
	}

	template <typename T>
	void Baby_RunningState<T>::Update(T& script, double dt)
	{
		if (script.interact_base->GetInteracted()) {
			script.player->SetAnimState(PlayerAnim::PickUpBaby);
			script.CalledPickup = true;
		}

		script.charac_pather->PathTo(script.bSpeed_base, script.bChosen_hidingspot);

		if (!script.HasNoStress()) {
			script.AutoDecreasingStress(static_cast<float>(dt));
		}
	}

	template <typename T>
	void Baby_RunningState<T>::Leave(T& script)
	{
		script.charac_pather->ClearPath();
		ECS->GetComponent<Physics>(script.id).SetLinearVelocity({ 0.f, 0.f, 0.f });
		ECS->GetComponent<Physics>(script.id).SetAngularVelocity({ 0.f, 0.f, 0.f });
	}

	template <typename T>
	void Baby_HeldState<T>::Enter(T& script)
	{
		////If first time picking up the baby, then this objective is done
		//if (firstTimePickup)
		//{
		//	firstTimePickup = false;

		//	//Go to next objective
		//	BE::BEID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
		//	BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
		//	std::shared_ptr<ObjectivesController> ptr1 = std::dynamic_pointer_cast<ObjectivesController>(gl.GetScript<ObjectivesController>());
		//	ptr1->StartObjectives(2);
		//}

		// restart stress timer
		script.ToggleStartStopOverstressed(false);

		script.CalledPickup = false;
		ECS->GetComponent<Transform>(script.id).AttachParent(GLS->GetPlayerID());
		script.SetBabyToCarryPosition();

		ECS->GetComponent<Physics>(script.id).SetLinearVelocity({ 0.f, 0.f, 0.f });
		ECS->GetComponent<Physics>(script.id).SetAngularVelocity({ 0.f, 0.f, 0.f });
		ECS->GetComponent<Physics>(script.id).SetIsTrigger(true);
		ECS->GetComponent<Physics>(script.id).SetGravityFactor(0.f);
		ECS->GetComponent<Physics>(script.id).SetInvalidateCollide(true);

		//Enable the button
		std::pair<BE::CanvasID, BE::UIElementID> laserButton = BE::UI->FindUIElementByName("LaserAbility", 1).front();
		BE::UI->GetCanvas(laserButton.first)->GetUI(laserButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);

		//Disable the Babypointer canvas
		BE::CanvasID babyPointerCanvas = BE::UI->FindUICanvasesByName("BabyPointer", 1).front();
		BE::UI->GetCanvas(babyPointerCanvas)->SetIsActive(false);

		if (ECS->HasComponent<GameLogic>(GLS->GetPlayerID())) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerCharacter>();
			PlayerCharacter* p_ptr = dynamic_cast<PlayerCharacter*>(scrpt_ptr.get());
			if (p_ptr != nullptr) {
				p_ptr->SetBabyMonkeyPtr(&script);
			}
			AudioSource& as = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
			as.StopAllAudio();
			as.Play(AssetID(8019658770557800846));//Baby!.wav;
		}
	}

	template <typename T>
	void Baby_HeldState<T>::Update(T& script, double dt)
	{
		//script.UpdateExplosion(static_cast<float>(dt));
	}

	template <typename T>
	void Baby_HeldState<T>::Leave(T& script)
	{
		//script.bCastedExplosion = false;
		script.ResetStressOvertime();
		if (ECS->HasComponent<GameLogic>(GLS->GetPlayerID())) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerCharacter>();
			PlayerCharacter* p_ptr = dynamic_cast<PlayerCharacter*>(scrpt_ptr.get());
			if (p_ptr != nullptr) {
				p_ptr->RemoveBabyMonkey();
			}
		}
		script.interact_base->ResetInteracted();
	}

	bool BabyMonkey::PlayerHasPickedUp() const
	{
		return interact_base->GetInteracted() && !player->IsPlayingOneShotAnim() && CalledPickup;
	}

	bool BabyMonkey::PlayerIsDead() const {
		return player->IsDead();
	}
	void BabyMonkey::Awake()
	{
	}

	void BabyMonkey::Init()
	{
		/*float explosion_Damage = 100.f;
		float explosion_CD = 1.0f;
		pExplosion = AttackSkill(explosion_CD, explosion_Damage, Vec3f(bStress_Explosion_size, bStress_Explosion_size, bStress_Explosion_size), AttackSkill::AOEType::Sphere);*/
		pExplosion.sAllyResistance = 0.5f;
		InitBabySpotsZone();

		// Get pointer to interactable base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<Interactable>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<Interactable>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<Interactable>();
		}
		interact_base = dynamic_cast<Interactable*>(script_ptr.get());
		interact_base->ResetInteracted();

		// Get pointer to Character Pather base
		script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<CharacterPather>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<CharacterPather>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterPather>();
		}
		charac_pather = dynamic_cast<CharacterPather*>(script_ptr.get());

		if (ECS->HasComponent<GameLogic>(GLS->GetPlayerID())) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerCharacter>();
			PlayerCharacter* p_ptr = dynamic_cast<PlayerCharacter*>(scrpt_ptr.get());
			if (p_ptr != nullptr) {
				player = p_ptr;
			}
		}

		eyesHeight = ECS->GetComponent<Transform>(id).GetScale().y * 0.5f * 0.45f;

		bChosen_hidingspot = ECS->GetComponent<Transform>(id).GetPosition();


		auto uiFind = BE::UI->FindUIElementByName("BabyTimer", 1);
		if (uiFind.size()) {
			uiCountdownText = uiFind.front();
		}

		bStress_current = 0.f;
		UpdateStressMeter();
		fsm.Init(current_state, previous_state);
	}

	void BabyMonkey::Update(double dt)
	{
		if (!ECS->GetComponent<BE::Transform>(id).HasParent()) {
			// deny collider flipping over
			Vec3f eulerangle = ECS->GetComponent<BE::Transform>(id).GetEulerRotation();
			eulerangle.x = 0.f; eulerangle.z = 0.f;
			ECS->GetComponent<BE::Transform>(id).SetEulerRotation(eulerangle);
		}
		float dt_f = static_cast<float>(dt);
		UpdateTimers(dt_f);

		fsm.Update(current_state, previous_state, dt_f);

		if (bStress_overtime_timer < bStress_overtime_duration && bStress_overtime_duration > 0.f) {
			bStress_overtime_timer += dt_f;
			AddStress(bStress_overtime_inc / bStress_overtime_duration * dt_f);

			if (bStress_overtime_timer >= bStress_overtime_duration) {
				if (bStress_overtime_inc < 0.0f)
				{
					AudioSource& as = ECS->GetComponent<AudioSource>(id);
					if (!as.IsAudioPlaying(AssetID(17521126164417407763)))//Burp.wav
					{
						as.Play(AssetID(17521126164417407763));
					}
				}
				ResetStressOvertime();
			}
		}


		if (bIsOverstressed)
		{
			// update timer
			bOverstressed_timer -= dt_f;
			UpdateOverstressedTimerUI(dt_f);

			if (bOverstressed_timer <= 0.f) {
				bIsOverstressed = false;

				// show game over
				auto game = ECS->FindEntitiesByName("GameManager");
				if (game.size()) {
					if (ECS->HasComponent<GameLogic>(game.front())) {
						auto ptr = ECS->GetComponent<GameLogic>(game.front()).GetScript<LevelManager>();
						if (ptr != nullptr) {
							LevelManager* gameptr = dynamic_cast<LevelManager*>(ptr.get());
							gameptr->ShowGameOver();
						}
					}
				}
			}
		}
	}

	inline void BabyMonkey::UpdateTimers(float dt) {

	}


	void BabyMonkey::InitBabySpotsZone()
	{
		const auto results = ECS->FindEntitiesByTag("BabySpot");
		for (auto spotID : results) {
			Vec3f spotPos = ECS->GetComponent<Transform>(spotID).GetPosition();
			std::vector<EntityID> zones;
			PhySys->CollidePoint(zones, spotPos, EntityTagFilter({ "Zone" }, FilterType::ContainsAny, 1));

			if (!zones.empty() && ECS->HasComponent<GameLogic>(zones.front())) {
				auto zoneptr = ECS->GetComponent<GameLogic>(zones.front()).GetScriptDerived<AreaZone>();
				if (zoneptr) {
					bHidingSpots_map.insert(std::make_pair(zoneptr->GetZoneNumber(), spotPos));
				}
			}
		}
	}

	inline void BabyMonkey::Random_Assign_HidingSpot()
	{
		const auto results = ECS->FindEntitiesByName("GameManager", 1);
		if (results.empty()) return;
		auto lvlmgr = ECS->GetComponent<GameLogic>(results.front()).GetScriptDerived<LevelManager>();

		// Get the range of baby spots in the map with the zone key the player is in.
		auto zoneRange = bHidingSpots_map.equal_range(lvlmgr->GetPlayerZone());
		auto itr = zoneRange.first;

		// Get a random index of the baby spots in the same zone.
		int random_index = std::rand() % std::distance(zoneRange.first, zoneRange.second);

		// assign the random baby spot at that index.
		std::advance(itr, random_index);
		bChosen_hidingspot = itr->second;
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<BabyMonkey>(RegistrationNameCreator<BabyMonkey>())
		.constructor()
		.property("Baby Stressed Explosion", &BabyMonkey::pExplosion)
		.property("Base Speed", &BabyMonkey::bSpeed_base)
		.property("Stress Capacity", &BabyMonkey::bStress_capacity)
		.property("Stress Auto Decrease Rate", &BabyMonkey::bStress_decre_rate)
		.property("Stress Coddle Rate", &BabyMonkey::bStress_coddle_rate)
		.property("Stress Laser Rate", &BabyMonkey::bStress_laser_rate);
}

//auto children = ECS->GetComponent<Transform>(id).GetChildren();
//for (auto child : children) {
//	if (ECS->GetComponent<EntityInfo>(child).GetName() == "ExplosionSphere") {
//		bExplosion_current = child;
//		break;
//	}
//}
//if (bExplosion_current != NULL_ENTITY)
//	ECS->GetComponent<EntityInfo>(bExplosion_current).SetFlag(EntityFlag::active, false);
//bExplosion_Asset = AssetMgr->GetAssetID("../Assets/Prefabs/ExplosionSphere.prefab");

//void BabyMonkey::UpdateExplosion(float dt)
//{
//	if (bShowExplosion && bExplosion_current != NULL_ENTITY)
//	{
//		Transform& transf = ECS->GetComponent<Transform>(bExplosion_current);
//		transf.SetScale(transf.GetScale() + bStress_Explosion_rate * dt);
//		if (transf.GetScale().x >= bStress_Explosion_size)
//		{
//			CastExplosion();
//		}
//		if (transf.GetScale().x < 0.f) {
//			CancelExplosion();
//		}
//	}
//	else if (IsMaxedStress() && !bShowExplosion)
//	{
//		ShowExplosion();
//	}
//}