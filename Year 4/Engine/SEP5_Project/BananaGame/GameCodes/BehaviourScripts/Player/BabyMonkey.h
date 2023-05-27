#pragma once
#ifndef	BABY_MONKEY_H
#define BABY_MONKEY_H

#include <BananaEngine.h>
#include "../Environment/Interactable.h"
#include "../Character/CharacterPather.h"
#include "../Character/BaseSkill.h"

namespace TH
{
	using namespace BE;

	template <typename T>
	class Baby_HidingState : public State<T>
	{
	public:
		float cryTimer = 0.0f;
		void Enter(T& in);
		void Update(T& in, double dt);
		void Leave(T& in) {}
	};

	template <typename T>
	class Baby_RunningState : public State<T>
	{
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	template <typename T>
	class Baby_HeldState : public State<T>
	{
		//For completing the "rescue baby" objective
		bool firstTimePickup = true;

	public:
		void Enter(T& in);
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	enum class BabySkill
	{
		Laser,
		Shield
	};

	class PlayerCharacter;

	class BabyMonkey : public IScript
	{
		using This = BabyMonkey;

		friend class Baby_HidingState<This>;
		friend class Baby_RunningState<This>;
		friend class Baby_HeldState<This>;

		struct BabyFSM : public FSM<This>
		{
			Baby_HidingState<This> Hiding_state;
			Baby_RunningState<This> Running_state;
			Baby_HeldState<This> Held_state;
			BabyFSM()
			{
				RegisterStates({ &Hiding_state, &Running_state, &Held_state
								 });
				AddTransition(&Held_state,		&Running_state,		&BabyMonkey::IsMaxedStress);
				AddTransition(&Held_state,		&Running_state,		&BabyMonkey::PlayerIsDead);
				AddTransition(&Running_state,	&Hiding_state,		&BabyMonkey::IsAt_ChosenHidingSpot);
				AddTransition(&Running_state,	&Held_state,		&BabyMonkey::PlayerHasPickedUp);
				AddTransition(&Hiding_state,	&Held_state,		&BabyMonkey::PlayerHasPickedUp);
			}
		};
		static BabyFSM fsm;
		PersonalState<This> current_state{ this };
		State<This>* previous_state;
		// static FSM Construction END_____________________________________________________
		
		Interactable* interact_base = nullptr;
		CharacterPather* charac_pather = nullptr;
		PlayerCharacter* player = nullptr;
		std::multimap<int, Vec3f> bHidingSpots_map;
		Vec3f  bChosen_hidingspot;

		AttackSkill pExplosion;
		//static constexpr uint8_t bNum_of_hidingspots = 1;
		//uint8_t bChosen_hidingspot = 0;
		//std::array<Vec3f, bNum_of_hidingspots> bHiding_spots;
		Vec3f bCarry_position = Vec3f(0.f, 0.6f, -0.3f);
		Vec3f bAttack_position = Vec3f(0.2f, 0.5f, 0.5f);
		float bSpeed_base = 8.5f;
		float bStress_capacity = 100.f;
		float bStress_current = 0.f;
		float bStress_decre_rate = 2.f;
		float bStress_coddle_rate = 20.f;
		float bStress_laser_rate = 15.f;
		float eyesHeight;

		BabySkill bSkill; 
		bool CalledPickup = false;
		bool ToBurp = false;
		static constexpr float bOverstressed_duration = 15.f;
		static constexpr float bOverstressed_startflickerAt = 10.f;
		Vec3f bOverstress_Color = Vec3f(1.f, 0.f, 0.f); //red 
		float bOverstressed_timer = 0.f;
		float bOverstressed_flickerduration = 0.4f;		// every 0.4s change color
		float bOverstressed_flickertimer = 0.f;
		float bOverstressed_flickerRate = 0.95f;		// muliplier to flicker duration every time it flickers
		bool bIsOverstressed = false;
		bool bOverstressed_flicker = true;
		std::pair<CanvasID, UIElementID> uiCountdownText;

		float bStress_overtime_inc = 0.f;
		float bStress_overtime_timer = 0.f;
		float bStress_overtime_duration = 0.f;

		void InitBabySpotsZone();
	public:
		inline void ToggleStartStopOverstressed(bool start) {
			bIsOverstressed = start;
			bOverstressed_timer = bOverstressed_duration;
			bOverstressed_flickerduration = 0.4f;
		}
		inline void UpdateOverstressedTimerUI(float dt)
		{
			std::stringstream ss;
			ss << static_cast<int>(bOverstressed_timer);
			BE::UI->GetCanvas(uiCountdownText.first)->GetUI<BE::UIFontText>(uiCountdownText.second)->SetText(ss.str());

			if (bOverstressed_timer <= bOverstressed_startflickerAt) {
				bOverstressed_flickertimer += dt;
				if (bOverstressed_flickertimer >= bOverstressed_flickerduration) {
					bOverstressed_flickertimer = 0.f;
					bOverstressed_flickerduration *= bOverstressed_flickerRate;

					// flicker color between red and white
					glm::vec3 textcolor = bOverstressed_flicker ? glm::vec3({ bOverstress_Color.x, bOverstress_Color.y, bOverstress_Color.z }) : glm::vec3({ 1.f, 1.f, 1.f });
					BE::UI->GetCanvas(uiCountdownText.first)->GetUI<BE::UIFontText>(uiCountdownText.second)->SetColour(textcolor);
					bOverstressed_flicker = !bOverstressed_flicker;
				}
			}
		}

		inline void UpdateStressMeter()
		{
			auto pair = BE::UI->FindUIElementByName("StressBar", 1);
			BE::UI->GetCanvas(pair[0].first)->GetUI<BE::UIProgressBar>(pair[0].second)->SetPercentage(bStress_current / bStress_capacity);
		}
		inline void AddStress(float add)
		{ 
			bStress_current += add;
			bStress_current = BEMath::Clamp<float>(0.f, bStress_capacity, bStress_current);
			UpdateStressMeter();
		}
		inline void AddStressOvertime(float add, float duration)
		{
			bStress_overtime_inc = add;
			bStress_overtime_timer = 0.f;
			bStress_overtime_duration = duration;
				
		}
		inline void ResetStressOvertime(){
			bStress_overtime_inc = 0.f;
			bStress_overtime_timer = 0.f;
			bStress_overtime_duration = 0.f;
		}

		inline bool IsMaxedStress() const			{ return bStress_current >= bStress_capacity;}
		inline void Random_Assign_HidingSpot();
		inline bool IsAt_ChosenHidingSpot() const	{ 
			float dist = ECS->GetComponent<Transform>(id).GetPosition().SqDistBetween(bChosen_hidingspot);
			return dist <= 1.0f;
		}
		inline void AddFixedLaserStress() { AddStress(bStress_laser_rate * static_cast<float>(Engine->GetFixedDeltaTime() * EngineSpeed->GetEngineSpeedMultiplier())); }

		//Getters
		bool PlayerIsDead() const;
		bool PlayerHasPickedUp() const;
		inline bool IsInteracted() const { return interact_base->GetInteracted(); }
		inline bool HasNoStress() const { return bStress_current == 0.f; }
		inline BabySkill GetCurrentSkill() const { return bSkill; }
		inline float GetCurrentStress() const { return bStress_current; }

		void Awake();
		void Init();
		void Update(double dt);
		void OnCollision(EntityID collidingID)
		{
		}

		inline Vec3f GetAttackPos() const { return bAttack_position; }
		inline void AutoDecreasingStress(float dt) { AddStress(dt * -bStress_decre_rate); }
		inline void CoddleDecreasingStress(float dt) { AddStress(dt * -bStress_coddle_rate); }
		inline void UpdateTimers(float dt);
		inline Vec3f GetFrontalPos(float forward_dist = 0.f)
		{
			Transform& transf = ECS->GetComponent<Transform>(id);
			Mat3f rotMatrix;
			rotMatrix.SetRotation(transf.GetRotation());
			Vec3f frontal = transf.GetPosition() + rotMatrix * (Vec3f::Forward() * (transf.GetScale().z * 0.5f * 0.75f + forward_dist));
			return frontal;
		}
		inline void SetBabyToCarryPosition() {
			ECS->GetComponent<Transform>(id).SetLocalPosition(bCarry_position);
			ECS->GetComponent<Transform>(id).SetLocalEulerRotation(Vec3f(0.f, 180.f, 0.f));
		}
		inline void SetBabyToAttackPosition() {
			ECS->GetComponent<Transform>(id).SetLocalPosition(bAttack_position);
			ECS->GetComponent<Transform>(id).SetLocalEulerRotation(Vec3f(0.f, 40.f, 0.f));
		}
		inline void SetBabyToCoddlePosition() {
			ECS->GetComponent<Transform>(id).SetLocalPosition(bAttack_position);
			ECS->GetComponent<Transform>(id).SetLocalEulerRotation(Vec3f(-90.f, 180.f, 90.f));

		}
		inline Vec3f GetEyesPosition()
		{
			Vec3f eyespos = GetFrontalPos();
			eyespos.y += eyesHeight;
			return eyespos;
		}
		inline void RemoveFromParent()
		{
			ECS->GetComponent<Transform>(id).DetachParent();
			ECS->GetComponent<Physics>(id).SetIsTrigger(false);
			ECS->GetComponent<Physics>(id).SetInvalidateCollide(false);
			ECS->GetComponent<Physics>(id).SetGravityFactor(1.f);
		}
		inline void ResetBabyToPlayer() {
			fsm.ForceTransition(current_state, &fsm.Held_state, nullptr);
			interact_base->InteractObject();
			bStress_current = 0;
			UpdateStressMeter();
		}

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}
#endif

//void UpdateExplosion(float dt);
//float bStress_Explosion_rate = 2.f;
//float bStress_Explosion_size = 10.0f;
//EntityID bExplosion_current = NULL_ENTITY;
//AssetID bExplosion_Asset = NULL_ENTITY;
//bool bCastedExplosion = false;
//bool bShowExplosion = false;
//
//inline void DecreaseStress(float dec)
//{
//	if (bStress_current < dec)
//		bStress_current = 0.f;
//	else bStress_current -= dec;
//
//	if (bShowExplosion) {
//		Transform& transf = ECS->GetComponent<Transform>(bExplosion_current);
//		float size = transf.GetScale().x - bStress_Explosion_rate * 2.f * static_cast<float>(Engine->GetDeltaTime());
//		transf.SetScale(Vec3f(size, size, size));
//	}
//	UpdateStressMeter();
//}

//inline bool CastedExplosion() const { return bCastedExplosion; }
//
//inline bool IsGoingExplode() const { return bShowExplosion; }
//
//inline void CastExplosion()
//{
//	if (bExplosion_current != NULL_ENTITY)
//	{
//		std::vector<Vec3f> contacts;
//		pExplosion.CastSkill(id, ECS->GetComponent<Transform>(id).GetPosition(), 1.f, contacts);
//		ECS->GetComponent<Transform>(bExplosion_current).SetScale(Vec3f(0.1f, 0.1f, 0.1f));
//		ECS->GetComponent<EntityInfo>(bExplosion_current).SetFlag(EntityFlag::active, false);
//		bCastedExplosion = true;
//		bShowExplosion = false;
//	}
//}
//inline void CancelExplosion()
//{
//	if (bExplosion_current != NULL_ENTITY)
//	{
//		ECS->GetComponent<Transform>(bExplosion_current).SetScale(Vec3f(0.1f, 0.1f, 0.1f));
//		ECS->GetComponent<EntityInfo>(bExplosion_current).SetFlag(EntityFlag::active, false);
//		bShowExplosion = false;
//	}
//}
//inline void ShowExplosion()
//{
//	if (bExplosion_current != NULL_ENTITY)
//	{
//		ECS->GetComponent<EntityInfo>(bExplosion_current).SetFlag(EntityFlag::active, true);
//		bShowExplosion = true;
//	}
//}