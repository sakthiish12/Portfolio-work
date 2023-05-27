#pragma once

#include <BananaEngine.h>
#include "../Character/CharacterScript.h"
#include "BossMinionSpawner.h"
#include "ElectricTowers.h"
#include "SpinningLasers.h"
#include "../Misc/Timers.h"
#include "../Environment/Door.h"

namespace TH
{
	using namespace BE;

	template <typename T>
	class BossIdle : public State<T>
	{
	public:
		void Update(T& in, double dt);
	
	};

	template <typename T>
	class BossPhase1 : public State<T>
	{
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
	};

	template <typename T>
	class BossPhase2 : public State<T>
	{
		bool enterAttack = true;
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
	};

	template <typename T>
	class BossPhase3 : public State<T>
	{
		bool audioCanPlayNext = false;
		bool enterAttack = true;
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
	};

	template <typename T>
	class BossDeadState : public State<T>
	{
		bool audioCanPlayNext = false;
		bool playedDialogue = false;
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
	};
		
	enum class BossAnim
	{
		CogTransform,
		Attack2,
		RisePlatforms,
		Damage,
		Death,
		Death2,
		DeathPose,
		Idle,
		Spawn,
		Total_Animations
	};

	enum class BossAttacks
	{
		SpinLaser,
		SpawnMinions,
		RisePlatforms,
		SpinAndRise
	};

	class RobotBoss : public IScript
	{
		using This = RobotBoss;

		friend class BossIdle<This>;
		friend class BossPhase1<This>;
		friend class BossPhase2<This>;
		friend class BossPhase3<This>;
		friend class BossDeadState<This>;

		struct RobotBossFSM : public FSM<This>
		{
			BossIdle<This> bossIdle;
			BossPhase1<This> bossPhase1;
			BossPhase2<This> bossPhase2;
			BossPhase3<This> bossPhase3;
			BossDeadState<This> bossDeadState;

			RobotBossFSM()
			{
				RegisterStates({ &bossIdle, &bossPhase1, &bossPhase2,
								 &bossPhase3, &bossDeadState });
				AddTransition(nullptr, &bossDeadState, &RobotBoss::IsDead);
				AddTransition(&bossIdle, &bossPhase1, &RobotBoss::ConditionToPhase1);
				AddTransition(&bossPhase1, &bossPhase2, &RobotBoss::ConditionToPhase2);
				AddTransition(&bossPhase2, &bossPhase3, &RobotBoss::ConditionToPhase3);
			}
		};
		static RobotBossFSM fsm;

		// fsm states
		PersonalState<This> current_state{ this };
		State<This>* previous_state;

		// pointers to work with other script this entity owns.
		CharacterScript* charac_base = nullptr;
		CharacterScript* decoy_charac_base = nullptr;
		SpinningLasers* spinning_laser1 = nullptr;
		SpinningLasers* spinning_laser2 = nullptr;
		ElectricTowers* electric_towers = nullptr;
		BossMinionSpawner* minion_spawner = nullptr;
		DoorScript* ceiling_door = nullptr;

		// Entity IDs of the other models this entity owns
		std::vector<EntityID> rBossRoomDoors;
		EntityID rDecoyID = NULL_ENTITY;
		EntityID rTopModelID = NULL_ENTITY;
		EntityID rBottomModelID = NULL_ENTITY;
		EntityID rCogModel1ID = NULL_ENTITY;
		EntityID rCogModel2ID = NULL_ENTITY;
		EntityID rCutsceneTrigger = NULL_ENTITY;

		// Spawning minions
		// Vec3f Cog1LocalPos = Vec3f(0.f, -4.f, 0.f);
		// Vec3f Cog2LocalPos = Vec3f(0.f, -2.f, 0.f);
		Vec3f CogHiddenOffset = Vec3f(0.f, 60.f, 0.f);
		Vec3f SummonRotSpeed = Vec3f(0.f, 1.5f, 0.f);
		Vec3f FixedPos;
		Vec3f FixedTopPos;
		Timer SummonCogTimer = Timer(2.f);
		uint8_t numSpawnOpenDoors = 3;
		BossAttacks boss_attack = BossAttacks::SpawnMinions;
		bool HasSummonedCog = false;
		bool hasFixedTopPos = false;

		// Animation
		std::array<AnimInfo, static_cast<size_t>(BossAnim::Total_Animations)>
			pAnim_Indexes;
		BossAnim pCurrent_Anim = BossAnim::Idle;
		BossAnim pPrev_Anim = BossAnim::Idle;
		bool pIsPlayingOneShot = false;
		bool HasSetAnimation_frame = false;
		inline void InitAnimationInfo(EntityID modelID);
		inline void AutoResetAnimation(float dt);
		inline bool SetAnimState(BossAnim in, bool forced = false);

		// SFX
		AssetID SFX_BossDamaged = AssetID(14759058113631186209);
		AssetID SFX_BossFlyDown = AssetID(9395880562869272380);
		AssetID SFX_BossHover = AssetID(1342811344480189752);
		AssetID SFX_BossLands = AssetID(14079363658526716051);

		//Voice Over assets
		AssetID VO1_Welcome = AssetID(14980488254007229132);
		/* AssetID VO2_RevelMe = AssetID(5348416422448476105); */
		AssetID VO3_Neurons = AssetID(4251258056058411229);
		AssetID VO4_Plutonium = AssetID(9587519300879731171);
		AssetID VO5_HowAmILosing = AssetID(11480235183548728303);
		AssetID VO6_Formation3 = AssetID(9832298664131413725);
		AssetID VO7_ProtectMe = AssetID(15651211174197394720);
		AssetID VO8_CantCompute = AssetID(5083137900985624292);

		AssetID VO5_Teehee = AssetID(289892750197749472);
		AssetID VO8_Teehee = AssetID(1086358731751608403);

		bool VO1_CanPlay = false;
		bool VO4_HasPlayed = false;
		bool VO5_HasPlayed = false;
		bool VO6_HasPlayed = false;
		bool VO7_HasPlayed = false;
		bool VO8_HasPlayed = false;

		// Boss UI
		std::pair<CanvasID, UIElementID> uiBossHPBar;

		// Phase changing with FSM.
		float bossActualHP;
		bool CanPhase2 = false;
		inline static const float healthPercentToPhaseChange = 0.333333f;
		inline bool HasTarget() const { return charac_base->HasTarget(); }
		inline bool ConditionToPhase1() const { return HasTarget(); }
		inline bool ConditionToPhase2() const { return CanPhase2; }
		inline bool ConditionToPhase3() const { return charac_base->GetCurrentHealth() / charac_base->GetTotalHealth() <= healthPercentToPhaseChange; }
		inline bool IsDead() const { return charac_base->IsDead(); }

		// Boss Attacks
		CoolDown SpawningCD = CoolDown(10.f);
		CoolDown SpinningCD = CoolDown(8.f);
		Timer AttackChangeTimer = Timer(18.f);
		Timer PlatformTimer = Timer(15.f);
		Timer BossSlam = Timer(0.5f);
		uint8_t maxSpawnTimes = 2;
		uint8_t currSpawnTimes = 0;
		inline void OpenCeilingDoorsAndMoveCogs(bool show);
		inline void RotatingLasersAttack() {
			spinning_laser1->StartLasering();
			spinning_laser2->StartLasering();
		}
		inline void LiftElectricPlatforms() { 
			electric_towers->LiftTowersAndElectricFloor();
			PlatformTimer.StartTimer();
		}
		inline void LowerElectricPlatforms(){
			electric_towers->LowerTowersAndElectricFloor();
		}
		inline void SpawnMinions() const { minion_spawner->OpenDoorsAndSpawnRandom(numSpawnOpenDoors); }
		inline void DeathFallingSlam() const;
		inline void CancelLasers() {
			spinning_laser1->CancelLasers();
			spinning_laser2->CancelLasers();
		}
		// Boss UI
		std::pair<CanvasID, UIElementID> BossHealthBar;
		std::pair<CanvasID, UIElementID> BossInfo;

		inline void FaceTarget();
		inline void LockUnlockBossRoomDoors(bool lock);
		inline void MainBodySpawnsIn();
		inline void SlamsBossRoomFloor();
		inline void AddRemoveBossOnGrid(bool add);
		inline void TryKeepLaser() {
			if (HasSummonedCog) {
				OpenCeilingDoorsAndMoveCogs(false);
				CancelLasers();
			}
		}

		inline void StartEndBossCutscene();

		Vec3f SpawnPos;
		bool hasStartedFight = false;
	public:
		inline void UpdateTimers(float dt);
		void Awake() override;
		void Init() override;
		void Update(double dt) override;

		void Respawn();
		inline bool HasStartedBoss() const { return hasStartedFight; }
		inline void StartBossVoiceOver() { VO1_CanPlay = true; }
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}