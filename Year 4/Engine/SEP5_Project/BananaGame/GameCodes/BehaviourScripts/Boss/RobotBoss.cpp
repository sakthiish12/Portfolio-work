
#include "RobotBoss.h"
#include "BossDecoy.h"
#include "../../MusicPlayer/MusicPlayer.h"
#include "../../DialogueCutsceneObjectivesControl/DialogueController.h"
#include "../../DialogueCutsceneObjectivesControl/CutsceneController.h"
#include "../Level/Triggers.h"

namespace TH
{
	using namespace BE;
	RobotBoss::RobotBossFSM RobotBoss::fsm;

	template <typename T>
	void BossIdle<T>::Update(T& script, double dt)
	{
	}

	// Boss Phase 1 
	template <typename T>
	void BossPhase1<T>::Enter(T& script)
	{
		script.SetAnimState(BossAnim::Idle, true);
		script.boss_attack = BossAttacks::SpawnMinions;
		script.SpawningCD.ResetTimer();
		script.SpawningCD.ended = true;
		script.LockUnlockBossRoomDoors(true);
		script.numSpawnOpenDoors = 4;
		script.hasStartedFight = true;

		UI->GetCanvas(script.uiBossHPBar.first)->SetCanvasFlag(UICanvasFlag::isActive, true);
	}
	template <typename T>
	void BossPhase1<T>::Update(T& script, double dt)
	{
		if (ECS->DoesEntityExists(script.rDecoyID)) {
			script.charac_base->SetCurrentHealth(script.bossActualHP +
				(script.bossActualHP / 2) * script.decoy_charac_base->GetCurrentHealth() / script.decoy_charac_base->GetTotalHealth());

			if (script.decoy_charac_base->IsDead()) {
				script.charac_base->SetCurrentHealth(script.bossActualHP);
				script.CanPhase2 = true;
			}
		}

		if (script.SpawningCD.IsTimerEnded() && script.currSpawnTimes < script.maxSpawnTimes) {
			script.SpawnMinions();
			script.SpawningCD.StartTimer();
			script.currSpawnTimes++;
		}
	}

	// Boss Phase 2
	template <typename T>
	void BossPhase2<T>::Enter(T& script)
	{
		script.SetAnimState(BossAnim::Spawn, true);
		script.boss_attack = BossAttacks::SpinLaser;
		script.AttackChangeTimer.ResetTimer();
		script.SpinningCD.ResetTimer();
		enterAttack = false;
		script.FixedPos = ECS->GetComponent<Transform>(script.rBottomModelID).GetPosition();
		script.MainBodySpawnsIn();

		IGMusicPlayer::inst->StartBossMusic();

		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		aSource.Play(script.VO3_Neurons);
		aSource.Stop(script.VO1_Welcome);

	}

	template <typename T>
	void BossPhase2<T>::Update(T& script, double dt)
	{
		script.FaceTarget();

		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		if (!aSource.IsAudioPlaying(script.SFX_BossHover)) aSource.Play(script.SFX_BossHover);
		if (!script.VO4_HasPlayed && !aSource.IsAudioPlaying(script.VO3_Neurons) && script.charac_base->GetHealthPercentage() <= 0.5f ) {
			script.VO4_HasPlayed = true; 
			aSource.Play(script.VO4_Plutonium);
		}

		// wait for the boss to finish spawning animation
		if (script.pCurrent_Anim == BossAnim::Spawn &&
			!ECS->GetComponent<AnimationComponent>(script.rTopModelID).getCompletedOnce()) {
			return;
		}
		else if (script.pCurrent_Anim == BossAnim::Spawn &&
			ECS->GetComponent<AnimationComponent>(script.rTopModelID).getCompletedOnce())
		{
			script.AttackChangeTimer.StartTimer();
			enterAttack = true;
		}

		if (script.hasFixedTopPos)
			ECS->GetComponent<Transform>(script.rTopModelID).SetPosition(script.FixedTopPos);

		// Boss using Laser attack
		if (script.boss_attack == BossAttacks::SpinLaser) {
			
			if (enterAttack) {
				script.SetAnimState(BossAnim::CogTransform, true);
				script.OpenCeilingDoorsAndMoveCogs(true);
				enterAttack = false;
			}
			if (script.SummonCogTimer.IsTimerEnded() && script.HasSummonedCog) {
				script.RotatingLasersAttack();
				script.SpinningCD.StartTimer();
			}
			if (script.AttackChangeTimer.IsTimerEnded() ) {
				script.OpenCeilingDoorsAndMoveCogs(false);
			}
			if (script.SummonCogTimer.IsTimerEnded() && !script.HasSummonedCog) {
				script.boss_attack = BossAttacks::RisePlatforms;
				script.AttackChangeTimer.StartTimer();
				enterAttack = true;

				if (!script.hasFixedTopPos) {
					script.hasFixedTopPos = true;
					script.FixedTopPos = ECS->GetComponent<Transform>(script.rTopModelID).GetPosition();
				}
			}

		}
		// Boss using rise platform attack
		else if (script.boss_attack == BossAttacks::RisePlatforms) {

			if (enterAttack) {
				script.SetAnimState(BossAnim::RisePlatforms, true);
				script.LiftElectricPlatforms();
				enterAttack = false;
			}
			if (script.AttackChangeTimer.IsTimerEnded()) {
				script.boss_attack = BossAttacks::SpinLaser;
				script.AttackChangeTimer.StartTimer();
				enterAttack = true;
			}
		}
	}

	// Boss Phase 3 
	template <typename T>
	void BossPhase3<T>::Enter(T& script)
	{
		script.LowerElectricPlatforms();
		script.TryKeepLaser();
		script.SetAnimState(BossAnim::Damage, true);
		script.boss_attack = BossAttacks::SpawnMinions;
		script.AttackChangeTimer.ResetTimer();
		script.AttackChangeTimer.StartTimer();
		script.SpawningCD.ResetTimer();
		script.numSpawnOpenDoors = 6;
		script.SpawningCD.duration = 10.f;

		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		aSource.Stop(script.VO3_Neurons);
		aSource.Stop(script.VO4_Plutonium);
		aSource.Play(script.SFX_BossDamaged);
		aSource.Play(script.VO5_HowAmILosing);

		audioCanPlayNext = false;
	}
	template <typename T>
	void BossPhase3<T>::Update(T& script, double dt)
	{
		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		if(!aSource.IsAudioPlaying(script.SFX_BossHover)) aSource.Play(script.SFX_BossHover);
		if (audioCanPlayNext) {
			AudioSource& player_aSource = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
			if (!aSource.IsAudioPlaying(script.VO5_HowAmILosing) && !script.VO5_HasPlayed)
			{
				// Play teehee VO 5
				script.VO5_HasPlayed = true;
				player_aSource.Play(script.VO5_Teehee);
				audioCanPlayNext = false;
			}
			else if (!player_aSource.IsAudioPlaying(script.VO5_Teehee) && !script.VO6_HasPlayed && script.VO5_HasPlayed)
			{
				// Play VO 6
				script.VO6_HasPlayed = true;
				aSource.Play(script.VO6_Formation3);
				audioCanPlayNext = false;
			}
			else if (script.VO6_HasPlayed && !script.VO7_HasPlayed && !aSource.IsAudioPlaying(script.VO6_Formation3) && script.charac_base->GetHealthPercentage() <= 0.1666f)
			{
				// Play VO 7
				script.VO7_HasPlayed = true;
				aSource.Play(script.VO7_ProtectMe);
				audioCanPlayNext = false;
			}
		}
		
		if (!audioCanPlayNext) {
			audioCanPlayNext = true;
		}

		if (script.hasFixedTopPos)
			ECS->GetComponent<Transform>(script.rTopModelID).SetPosition(script.FixedTopPos);

		script.FaceTarget();

		// Boss using Laser attack
		if (script.boss_attack == BossAttacks::SpinAndRise) {

			if (enterAttack) {
				script.SetAnimState(BossAnim::RisePlatforms, true);
				script.OpenCeilingDoorsAndMoveCogs(true);
				script.LiftElectricPlatforms();

				enterAttack = false;
			}

			if (script.SummonCogTimer.IsTimerEnded() && script.HasSummonedCog) {

				if (!script.SpinningCD.IsTimerRunning()) {
					script.RotatingLasersAttack();
					script.SpinningCD.StartTimer();
				}
			}
				
			if (script.AttackChangeTimer.IsTimerEnded()) {
				script.OpenCeilingDoorsAndMoveCogs(false);
			}
			if (script.SummonCogTimer.IsTimerEnded() && !script.HasSummonedCog) {
				script.boss_attack = BossAttacks::SpawnMinions;
				script.AttackChangeTimer.StartTimer();
				enterAttack = true;
			}
		}
		// Boss using rise platform attack
		else if (script.boss_attack == BossAttacks::SpawnMinions) {

			// return lasers if still on field
			script.TryKeepLaser();

			if (!script.SpawningCD.start) {
				script.SpawnMinions();
				script.SpawningCD.StartTimer();
			}
			if (script.AttackChangeTimer.IsTimerEnded()) {
				script.boss_attack = BossAttacks::SpinAndRise;
				script.AttackChangeTimer.StartTimer();
				enterAttack = true;
			}
		}
	}

	template <typename T>
	void BossDeadState<T>::Enter(T& script)
	{
		// boss skills end
		script.LowerElectricPlatforms();
		script.OpenCeilingDoorsAndMoveCogs(false);
		script.CancelLasers();
		script.minion_spawner->KillAllRemainingSpawns();

		// death animation
		script.SetAnimState(BossAnim::Death, true);
		ECS->GetComponent<Physics>(script.id).SetIsTrigger(true);

		IGMusicPlayer::inst->StartGameMusic();
		UI->GetCanvas(script.uiBossHPBar.first)->SetCanvasFlag(UICanvasFlag::isActive, false);


		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		aSource.Stop(script.VO5_HowAmILosing);
		aSource.Stop(script.VO6_Formation3);
		aSource.Stop(script.VO7_ProtectMe);
		aSource.Play(script.VO8_CantCompute);

		playedDialogue = false;
		audioCanPlayNext = false;
	}

	template <typename T>
	void BossDeadState<T>::Update(T& script, double dt)
	{
		AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		if (audioCanPlayNext) {
			AudioSource& player_aSource = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
			if (!aSource.IsAudioPlaying(script.VO8_CantCompute) && !script.VO8_HasPlayed)
			{
				// Play teehee VO 8
				script.VO8_HasPlayed = true;
				player_aSource.Play(script.VO8_Teehee);
				audioCanPlayNext = false;
			}
			else if (!playedDialogue && !player_aSource.IsAudioPlaying(script.VO8_Teehee) && script.VO8_HasPlayed)
			{
				playedDialogue = true;
				script.StartEndBossCutscene();
				script.LockUnlockBossRoomDoors(false);
			}
		}

		if (!audioCanPlayNext) {
			audioCanPlayNext = true;
		}

		// return lasers if still on field
		script.TryKeepLaser();
		script.LowerElectricPlatforms();
	}
	void RobotBoss::Awake()
	{
	}
	void RobotBoss::Init()
	{
		GetOrAddOtherRequiredScripts<CharacterScript>(charac_base, id);
		GetOrAddOtherRequiredScripts<ElectricTowers>(electric_towers, id);
		GetOrAddOtherRequiredScripts<BossMinionSpawner>(minion_spawner, id);
		const auto lasercog1 = ECS->FindEntitiesByName("BossLaser 1", 1);
		if (lasercog1.empty()) {
			LogCoreError("Error finding BossLaser 1 entity!");
		}
		else {
			GetOrAddOtherRequiredScripts<SpinningLasers>(spinning_laser1, lasercog1.front());
			spinning_laser1->SetBaseAttack(charac_base->GetBaseAttack());
		}

		const auto lasercog2 = ECS->FindEntitiesByName("BossLaser 2", 1);
		if (lasercog2.empty()) {
			LogCoreError("Error finding BossLaser 2 entity!");
		}
		else {
			GetOrAddOtherRequiredScripts<SpinningLasers>(spinning_laser2, lasercog2.front());
			spinning_laser2->SetBaseAttack(charac_base->GetBaseAttack());
		}


		const auto cdoor = ECS->FindEntitiesByName("CeilingDoor_Boss", 1);
		if (cdoor.empty()) {
			LogCoreError("Error finding BossCeilingDoor entity!");
		}
		else {
			GetOrAddOtherRequiredScripts<DoorScript>(ceiling_door, cdoor.front());
			ceiling_door->SetDoorOpenCloseDuration(0.5f);
		}
		
		const auto& model = ECS->GetComponent<Transform>(id).GetChildren();
		if (!model.empty()) {
			rTopModelID = model.front();
		}

		const auto CutsceneTrigger = ECS->FindEntitiesByName("Cutscene5Trigger", 1);
		if (!CutsceneTrigger.empty()) {
			rCutsceneTrigger = CutsceneTrigger.front();
		}

		const auto base = ECS->FindEntitiesByName("FireWall Base", 1);
		if (!base.empty()) {
			rBottomModelID = base.front();
			SpawnPos = ECS->GetComponent<Transform>(rBottomModelID).GetPosition();
		}

		uiBossHPBar = UI->FindUIElementByName("BossHealthBar", 1).front();
		InitAnimationInfo(rTopModelID);



		charac_base->SetKnockBackImmune(true);
		charac_base->SetModelID(rTopModelID);
		charac_base->SetDestroyDuration(15.f);

		Respawn();
	}

	void RobotBoss::Respawn()
	{
		const auto decoy = ECS->FindEntitiesByName("Boss Decoy", 1);
		if (decoy.empty()) {
			LogCoreError("Error finding Boss Decoy entity!");
		}
		else {
			rDecoyID = decoy.front();

			ECS->GetComponent<EntityInfo>(rDecoyID).SetFlag(EntityFlag::active, true);
			Vec3f respawnPoint = ECS->GetComponent<BE::GameLogic>(rDecoyID).GetScriptDerived<CharacterScript>()->GetSpawnPos();
			if(respawnPoint != Vec3f()) ECS->GetComponent<Transform>(rDecoyID).SetPosition(respawnPoint);
			ECS->GetComponent<Physics>(rDecoyID).SetIsTrigger(false);
			ECS->GetComponent<GameLogic>(rDecoyID).GetScriptDerived<BossDecoy>()->Respawn();

			GetOrAddOtherRequiredScripts<CharacterScript>(decoy_charac_base, rDecoyID);
			float decoyHP = charac_base->GetTotalHealth() / 3;
			bossActualHP = 2 * decoyHP;
			decoy_charac_base->SetTotalHealth(decoyHP / 3);
			decoy_charac_base->SetCurrentHealth(decoyHP / 3);
		}
		fsm.Init(current_state, previous_state);

		SpawningCD.ResetTimer();
		SpinningCD.ResetTimer();
		AttackChangeTimer.ResetTimer();
		PlatformTimer.ResetTimer();
		SummonCogTimer.ResetTimer();
		BossSlam.ResetTimer();
		currSpawnTimes = 0;
		HasSummonedCog = false;
		CanPhase2 = false;
		hasFixedTopPos = false;
		hasStartedFight = false;

		VO1_CanPlay = false;
		VO4_HasPlayed = false;
		VO5_HasPlayed = false;
		VO6_HasPlayed = false;
		VO7_HasPlayed = false;
		VO8_HasPlayed = false;

		charac_base->SetTarget(NULL_ENTITY);
		charac_base->SetCurrentHealth(charac_base->GetTotalHealth());


		//ECS->GetComponent<Transform>(id).SetPosition(charac_base->GetSpawnPos());
		ECS->GetComponent<Transform>(rBottomModelID).SetPosition(SpawnPos);
		//ECS->GetComponent<Transform>(id).SetLocalPosition(Vec3f());

		FixedPos = ECS->GetComponent<Transform>(rBottomModelID).GetPosition();

		rBossRoomDoors = ECS->FindEntitiesByName("BossRoomDoor", 2);
		auto script = std::dynamic_pointer_cast<DoorScript>(ECS->GetComponent<GameLogic>(rBossRoomDoors.front()).GetScript<DoorScript>());
		if (script) {
			script->SetDoorLock(false);
		}
		spinning_laser1->Respawn();
		spinning_laser2->Respawn();
		minion_spawner->Respawn();
		electric_towers->Respawn();

		auto roomtrigger = std::dynamic_pointer_cast<BossRoomTrigger>(ECS->GetComponent<GameLogic>(rCutsceneTrigger).GetScript<BossRoomTrigger>());
		if (roomtrigger) {
			roomtrigger->ResetTrigger();
		}

		IGMusicPlayer::inst->StartGameMusic();

		UI->GetCanvas(uiBossHPBar.first)->SetCanvasFlag(UICanvasFlag::isActive, false);

		AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
		aSource.StopAllAudio();

		SetAnimState(BossAnim::Idle, true);

		AddRemoveBossOnGrid(false);
	}

	void RobotBoss::Update(double dt)
	{
		UpdateTimers(static_cast<float>(dt));
		fsm.Update(current_state, previous_state, dt);

		if(charac_base)
			UI->GetCanvas(uiBossHPBar.first)->GetUI<UIProgressBar>(uiBossHPBar.second)->SetPercentage(charac_base->GetCurrentHealth() / charac_base->GetTotalHealth());
		
		if (IsDead()) {
			return;
		}
		// lower platform.
		if (PlatformTimer.IsTimerEnded()) {
			LowerElectricPlatforms();
		}

		if (BossSlam.IsTimerEnded()) {
			SlamsBossRoomFloor();
		}

		// fix boss to position
		FixedPos.y = ECS->GetComponent<Transform>(rBottomModelID).GetPosition().y;
		ECS->GetComponent<Transform>(rBottomModelID).SetPosition(FixedPos);

		AutoResetAnimation(static_cast<float>(dt));

		// VO
		if (VO1_CanPlay) {
			VO1_CanPlay = false;
			AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
			aSource.Play(VO1_Welcome);
		}

	}

	void RobotBoss::UpdateTimers(float dt)
	{
		SpawningCD.update(dt);
		SpinningCD.update(dt);
		AttackChangeTimer.update(dt);
		PlatformTimer.update(dt);
		SummonCogTimer.update(dt);
		BossSlam.update(dt);
	}

	void RobotBoss::InitAnimationInfo(EntityID modelID)
	{
		if (ECS->HasComponent<AnimationComponent>(modelID) && ECS->HasComponent<Renderer>(modelID))
		{
			//Check if model is loaded
			if (AssetMgr->IsAssetDataLoaded(ECS->GetComponent<Renderer>(modelID).getModelID()))
			{
				//now go through all animations
				auto& vector = AssetMgr->GetAssetData<BeModel>(ECS->GetComponent<Renderer>(modelID).getModelID()).GetAnimationDetails();
				for (size_t i{}; i < static_cast<size_t>(BossAnim::Total_Animations); ++i)
				{

					if (i >= pAnim_Indexes.size()) continue;
					if (pAnim_Indexes[i].index < 0) {
						pAnim_Indexes[i].index = static_cast<int>(i);
						pAnim_Indexes[i].duration = vector[i].GetDuration() / vector[i].GetTicksPerSecond();
					}
				}
			}
		}
	}

	void RobotBoss::AutoResetAnimation(float dt)
	{
		if (!ECS->HasComponent<AnimationComponent>(rTopModelID)) return;

		HasSetAnimation_frame = false;
		if (!pIsPlayingOneShot) return;

		if (ECS->GetComponent<AnimationComponent>(rTopModelID).getCompletedOnce()) {
			pIsPlayingOneShot = false;
			SetAnimState(BossAnim::Idle);
		}
	}

	bool RobotBoss::SetAnimState(BossAnim in, bool forced)
	{
		if (!ECS->HasComponent<AnimationComponent>(rTopModelID)) return false;
		size_t index = static_cast<size_t>(in);
		// Check if animation was found and loaded.
		if (pAnim_Indexes[index].index < 0) return false;
		if (in == pCurrent_Anim) return false;

		if (!forced) {
			if (pIsPlayingOneShot || HasSetAnimation_frame) return false;
			else pPrev_Anim = pCurrent_Anim;
		}
		else {
			if (pIsPlayingOneShot) pIsPlayingOneShot = false;
		}

		pCurrent_Anim = in;
		ECS->GetComponent<AnimationComponent>(rTopModelID).SetAnimation(pAnim_Indexes[index].index);
		HasSetAnimation_frame = true;

		ECS->GetComponent<AnimationComponent>(rTopModelID).setTickFactor(1.f);
		ECS->GetComponent<AnimationComponent>(rTopModelID).setFreezeAtLastFrame(false);

		switch (in) {
		case BossAnim::Damage: {
			pIsPlayingOneShot = true;
			break;
		}
		case BossAnim::CogTransform: {
			//ECS->GetComponent<AnimationComponent>(rTopModelID).setTickFactor(0.75f);
			pIsPlayingOneShot = true;
			break;
		}
		case BossAnim::Death: {
			ECS->GetComponent<AnimationComponent>(rTopModelID).setFreezeAtLastFrame(true);
			pIsPlayingOneShot = true;
			break;
		}
		case BossAnim::RisePlatforms: {
			pIsPlayingOneShot = true;
			break;
		}
		case BossAnim::Idle: {
			//ECS->GetComponent<AnimationComponent>(rTopModelID).setTickFactor(0.75f);
			break;
		}
		case BossAnim::Spawn: {
			ECS->GetComponent<AnimationComponent>(rTopModelID).setTickFactor(0.75f);
			pIsPlayingOneShot = true;
			break;
		}
		default: {
			break;
		}
		}

		return true;
	}

	inline void RobotBoss::OpenCeilingDoorsAndMoveCogs(bool show)
	{
		if (SummonCogTimer.IsTimerRunning()) return;
		if (ceiling_door)
		{
			SummonCogTimer.StartTimer();
			ceiling_door->OpenDoorsAndCloseAfter(SummonCogTimer.duration);

			if (spinning_laser1)
			{
				Vec3f pos = ECS->GetComponent<Transform>(spinning_laser1->GetID()).GetPosition();
				Vec3f Endpos1 = show ? pos - CogHiddenOffset : pos + CogHiddenOffset;
				spinning_laser1->MoveAndSpinToPosition(Endpos1, SummonRotSpeed, SummonCogTimer.duration);
			}
			if (spinning_laser2) {
				Vec3f pos = ECS->GetComponent<Transform>(spinning_laser2->GetID()).GetPosition();
				Vec3f Endpos2 = show ? pos - CogHiddenOffset : pos + CogHiddenOffset;
				spinning_laser2->MoveAndSpinToPosition(Endpos2, SummonRotSpeed, SummonCogTimer.duration);
			}

			HasSummonedCog = show;
		}
	}

	inline void RobotBoss::DeathFallingSlam() const
	{

	}

	inline void RobotBoss::FaceTarget()
	{
		if (!charac_base->HasTarget()) return;
		Vec3f dir = ECS->GetComponent<Transform>(charac_base->GetTarget()).GetPosition() - ECS->GetComponent<Transform>(id).GetPosition();
		dir.y = 0.f;
		dir.Normalise();
		ECS->GetComponent<Transform>(id).SetRotation(Vec3f::Forward().get_rotation_between(dir));
		float euler = ECS->GetComponent<Transform>(id).GetEulerRotation().y;
		ECS->GetComponent<Transform>(id).SetEulerRotation(Vec3f(0.f, euler, 0.f));
	}

	inline void RobotBoss::LockUnlockBossRoomDoors(bool lock)
	{
		for (auto doorid : rBossRoomDoors) {
			auto script = std::dynamic_pointer_cast<DoorScript>(ECS->GetComponent<GameLogic>(doorid).GetScript<DoorScript>());
			if (script) {
				if(lock) script->CloseDoors();
				script->SetDoorLock(lock);
			}
		}
	}

	inline void RobotBoss::MainBodySpawnsIn()
	{
		if (ceiling_door)
		{
			ceiling_door->OpenDoorsAndCloseAfter(4.0f);
			ECS->GetComponent<Physics>(rBottomModelID).SetGravityFactor(10.f);
			ECS->GetComponent<Physics>(rBottomModelID).SetLinearVelocity(Vec3f(0.f, -20.f, 0.f));
			BossSlam.StartTimer();

			/*AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
			aSource.Play(SFX_BossFlyDown);*/
		}
	}

	inline void RobotBoss::SlamsBossRoomFloor()
	{
		AddRemoveBossOnGrid(true);

		AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
		aSource.Play(SFX_BossLands);
		if(minion_spawner) minion_spawner->KnockBackMinionsActive(100.f);
		auto playercharac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<CharacterScript>());
		if (playercharac)
		{
			Vec3f kb_dir = ECS->GetComponent<Transform>(GLS->GetPlayerID()).GetPosition() - ECS->GetComponent<Transform>(id).GetPosition();
			kb_dir.y = 0.f;
			playercharac->KnockBackCharacter(kb_dir, 500.f);
		}
	}

	inline void RobotBoss::AddRemoveBossOnGrid(bool add) 
	{
		const auto results = ECS->FindEntitiesByName("GameManager", 1);
		if (!results.empty())
		{
			auto scptr = ECS->GetComponent<GameLogic>(results.front()).GetScript<PathFinder>();
			PathFinder* patherfinder = dynamic_cast<PathFinder*>(scptr.get());
			if (patherfinder) {
				Vec3f pos = ECS->GetComponent<Transform>(rBottomModelID).GetPosition();
				Vec3f size = ECS->GetComponent<Transform>(rBottomModelID).GetScale();
				pos.y = 0.f;
				patherfinder->SetGridOccupyInBox(pos, size.x, size.z, 0, add);
			}
		}
	}

	inline void RobotBoss::StartEndBossCutscene()
	{
		EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
		BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
		std::shared_ptr<CutsceneController> ptr = gl.GetScriptDerived<CutsceneController>();
		ptr->StartCutscene(5);
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<RobotBoss>(RegistrationNameCreator<RobotBoss>())
		.constructor();
}
