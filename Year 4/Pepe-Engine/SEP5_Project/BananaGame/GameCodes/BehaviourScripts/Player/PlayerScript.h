#pragma once
#ifndef	PLAYER_SCRIPT_H
#define PLAYER_SCRIPT_H

#include <BananaEngine.h>
#include "../Character/BaseSkill.h"
#include "../Character/VirtualCharacterScript.h"
#include "../Character/CharacterScript.h"
#include "BabyMonkey.h"
#include "../Camera/CameraController.h"
#include "../../UserInterface/InGameUIScripts/MilkBottleUIController.h"
#include "PlayerInteract.h"

namespace TH
{
	using namespace BE;

	enum class PlayerAnim
	{
		Idle,
		InteractCrate,
		Jump,
		Melee,
		Death,
		PickUpBaby,
		Run,
		RunShoot,
		RunBaby,
		StandFire,
		StandReload,
		Total_Animations
	};
	//enum class PlayerAnim
	//{
	//	BabyCharge,
	//	BabyCoddleIdle,
	//	BabyCoddleWalk,
	//	BabyJumpFall,
	//	BabyJumpLand,
	//	BabyJumpStart,
	//	Idle,
	//	IdleWBaby,
	//	InteractCrate,
	//	JumpFall,
	//	JumpLand,
	//	Jump,
	//	Melee,
	//	Death,
	//	PickUpBaby,
	//	RunShoot,
	//	Run,
	//	RunBaby,
	//	StandFire,
	//	Total_Animations
	//};

	enum class PlayerSkills {
		Melee,
		Laser,
		Banana,
		Shield
	};

	class PlayerCharacter : public IScript
	{
		CharacterScript* charac_base = nullptr;
		VirtualCharacter* charac_body = nullptr;
		BabyMonkey* baby_monkey = nullptr;
		CameraController* camera_control = nullptr;
		MilkBottleUIController* milk_bottle_ptr = nullptr;
		PlayerInteract* player_interact = nullptr;

		// Player's Skills
		AttackSkill pMelee_attack;
		//RayCastSkill pLaser_attack;
		ProjectileSkill pBanana_shoot;
		ProjectileSkill pPeel_throw;
		ChargeBeamSkill pCharge_beam;

		PlayerSkills pCurrent_Equipped_skill = PlayerSkills::Melee;

		// Animation
		std::array<AnimInfo, static_cast<size_t>(PlayerAnim::Total_Animations)>
			pAnim_Indexes;

		//float pAnim_timer = 0.f;
		PlayerAnim pCurrent_Anim = PlayerAnim::Idle;
		PlayerAnim pPrev_Anim = PlayerAnim::Idle;
		bool pIsPlayingOneShot = false;
		bool HasSetAnimation_frame = false;

		// Character size
		static constexpr float	pCharacterHeightCrouching = 5.0f;
		static constexpr float	pCharacterRadiusCrouching = 0.3f;
		static constexpr float	pJumpSpeed = 13.0f;

		// The different stances for the character
		JPH::RefConst<JPH::Shape>	pCrouchingShape;

		// Smoothed value of the player input
		Vec3f pSmoothMovementDirection;
		Vec3f pModel_scale;
		float pRunning_Multplr = 2.6f;

		//float pLaser_intensity_rate = 1.0f;
		//float pLaser_intensity_max = 20.0f;
		//EntityID pLaserID = NULL_ENTITY;
		//EntityID pLaserLightID = NULL_ENTITY;

		EntityID pModelID = NULL_ENTITY;
		uint8_t pBananas_current = 0;
		constexpr static uint8_t pBananas_capacity = 64;
		bool ADS_mode = false;
		bool Shot_banana = false;
		bool pFirstTimeBaby = false;
		//bool pIsLasering = false;
		bool pCanAttack = true;
		bool pCanSwitchMode = true;

		enum class PlayerMode {
			Attack,
			Nanny
		}pPlayerMode = PlayerMode::Attack;

		CoolDown pModeSwitch = CoolDown(0.5f);

		Timer pFeedBabyDuration = Timer(3.0f);
		float pFeedStressDecrease = 40.f;
		float pLaserStress = 30.f;

		//Reference to the buttons and banana count text
		std::pair<CanvasID, UIElementID> meleeButton;
		std::pair<CanvasID, UIElementID> laserButton;
		std::pair<CanvasID, UIElementID> bananaButton;
		std::pair<CanvasID, UIElementID> shieldButton;
		std::pair<CanvasID, UIElementID> bananaCountText;
		std::pair<CanvasID, UIElementID> bananaCountBG;
		std::pair<CanvasID, UIElementID> abilityIcon;
		std::pair<CanvasID, UIElementID> crosshairTexture;
		
	public:
		float idleSFXTimer = 0.0f;
		bool initialDialogueCompleted = false;
		bool initialDialogueStarted = false;
		bool intialCutSceneStarted = false;
		inline bool IsBabySkill(PlayerSkills skilltocheck) const
		{ 
			return skilltocheck == PlayerSkills::Laser ||
					skilltocheck == PlayerSkills::Shield;
		}
		inline void RotateCharacter(Vec3f eulerangle)
		{
			if (charac_base->IsDead()) return;
			Quaternion quat = Quaternion(eulerangle.x, eulerangle.y , eulerangle.z);
			ECS->GetComponent<Transform>(id).SetRotation(quat);
		}
		~PlayerCharacter() {
			//mCharacter = nullptr;
		}
		void AddBananas(uint8_t Add);
		inline void SetBabyMonkeyPtr(BabyMonkey* in)
		{ 
			baby_monkey = in;
			if (!pFirstTimeBaby){
				pFirstTimeBaby = true;
				auto pair = BE::UI->FindUIElementByName("StressBar", 1);
				BE::UI->GetCanvas(pair[0].first)->GetUI<BE::UIProgressBar>(pair[0].second)->SetUIElementFlag(UIElementFlags::isActive,true);
			}
		}
		void RemoveBabyMonkey();

		inline bool HasBabyMonkey()const { return baby_monkey != nullptr; }
		inline bool IsPlayingOneShotAnim() const { return pIsPlayingOneShot; }
		void SetAnimState(PlayerAnim in, bool forced = false);
		void SetCurrentPlayerSkill(PlayerSkills change);
		inline PlayerSkills GetCurrentPlayerSkill() const { return pCurrent_Equipped_skill; }
		inline void SetCanAttack(bool in) { pCanAttack = in; }
		inline bool GetCanAttack() const { return pCanAttack; }
		inline void SetCanSwitchMode(bool in) { pCanSwitchMode = in; }
		inline bool GetCanSwitchMode() const { return pCanSwitchMode; }
		inline void ToggleADS(bool on) {
			if(on)
				camera_control->ZoomCamera_ADS();
			else camera_control->ResetZoomCamera_ADS();
			ADS_mode = on;
			ToggleCrosshair(on);
		}

		//Respawn the player
		void ResurretPlayer(const Vec3f& _pos, const Vec3f& _rot);
		inline bool IsDead() const { return charac_base->IsDead(); }
		
	private:
		inline void HandleInput(Vec3f inMovementDirection, bool isRunning, bool inJump, bool inSwitchStance, float inDeltaTime);
		inline void MeleeAttack();
		//inline void LaserAttack(float dt);
		inline void ShootBanana(bool isMoving = false);
		inline void PeelThrow(bool isMoving = false);
		inline void BabySkill_Update(float dt, bool IsMoving);
		//inline void LaserSkill_Input(float dt, bool isMoving);
		inline void ShieldSkill_Input(float dt) { if (!pCanAttack) return; }
		inline void MeleeSkill_Input(float dt);
		inline void BeamSkill_Input(float dt);
		inline void BananaSkill_Input(float dt, bool isMoving);
		inline void AttackMode_Update(float dt, bool isMoving);
		inline void NannyMode_Update(float dt);
		inline void Movement_Update(float dt);
		inline void InteractFront();
		inline void ToggleBananaCounter(bool show);
		inline Vec3f GetCrosshairContactPosition();

		inline void SwitchToMode(PlayerMode toSwitch) 
		{
			if (toSwitch == pPlayerMode) return;

			switch (toSwitch) {
			case PlayerMode::Nanny: {
				if (!baby_monkey || !milk_bottle_ptr ||
					milk_bottle_ptr->GetMilkBottleCount() == 0 || baby_monkey->GetCurrentStress() == 0.f) return;
				if (ADS_mode) {
					camera_control->ResetZoomCamera_ADS();
					ADS_mode = false;
					ToggleCrosshair(false);
				}
				baby_monkey->SetBabyToCoddlePosition();
				camera_control->SetCameraPosition_TopDown();
				camera_control->SetAllowZoom(false);
				camera_control->SetAllowVertical(false);

				pFeedBabyDuration.StartTimer();
				baby_monkey->AddStressOvertime(-pFeedStressDecrease, pFeedBabyDuration.duration);
				milk_bottle_ptr->SubtractMilkBottleCount();
				break;
			}
			case PlayerMode::Attack: {
				if (pFeedBabyDuration.IsTimerRunning()) return;
				if (baby_monkey) {
					if (IsBabySkill(GetCurrentPlayerSkill()))
						baby_monkey->SetBabyToAttackPosition();
					else
						baby_monkey->SetBabyToCarryPosition();
				}
				else return;
				camera_control->SetAllowZoom(true);
				camera_control->SetAllowVertical(true);
				camera_control->SetCameraPosition_Original();
				break;
			}
			}
			pPlayerMode = toSwitch;
		}

		void Awake();
		void Init();
		inline void UpdateTimers(float dt);
		void Update(double dt);
		void Cleanup();
		void UponSleep();
		void UponWake();
		void OnCollision(EntityID collidingID);
		void OnPersistedCollision(EntityID collidingID);
		void OnCollisionRemoved(EntityID collidingID);
		void InitAnimationInfo(EntityID modelID);
		void AutoResetAnimation(float dt);




		void ResetLaser();
		void ToggleCrosshair(bool show);

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}

#endif