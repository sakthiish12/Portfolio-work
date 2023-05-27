#pragma once
#ifndef	ENEMY_SCRIPT_H
#define ENEMY_SCRIPT_H

#include <BananaEngine.h>
#include "../Character/BaseSkill.h"
#include "../Character/CharacterScript.h"
#include "../Character/CharacterPather.h"
#include "EnemySpecialized.h"
#include "../Player/PlayerSurround.h"

namespace TH
{
	using namespace BE;
	template <typename T>
	class AttackState : public State<T>
	{
	public:
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	template <typename T>
	class IdleState : public State<T>
	{
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	template <typename T>
	class AggroPathingState : public State<T>
	{
	public:
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	template <typename T>
	class ResetPathingState : public State<T>
	{
	public:
		void Enter(T& in);
		void Update(T& in, double dt);
		void Leave(T& in);
	};

	template <typename T>
	class DeadState : public State<T>
	{
	public:
		void Enter(T& in);
	};

	template <typename T>
	class StunState : public State<T>
	{
	public:
		void Enter(T& in);
		void Leave(T& in);
	};

	class EnemyBase : public IScript
	{
		using This = EnemyBase;

		friend class AttackState<This>;
		friend class IdleState<This>;
		friend class DeadState<This>;
		friend class AggroPathingState<This>;
		friend class ResetPathingState<This>;
		friend class StunState<This>;

		struct EnemyFSM : public FSM<This>
		{
			AttackState<This> Attack_state;
			IdleState<This> Idle_state;
			DeadState<This> Dead_state;
			AggroPathingState<This> AggroPathing_state;
			ResetPathingState<This> ResetPathing_state;
			StunState<This> Stunned_state;
			EnemyFSM()
			{
				RegisterStates({ &Idle_state, &AggroPathing_state,
								 &Attack_state, &ResetPathing_state, &Dead_state, &Stunned_state });
				AddTransition(nullptr,				&Dead_state,			&EnemyBase::IsDead							);
				AddTransition(&Idle_state,			&Stunned_state,			&EnemyBase::IsStunned						);
				AddTransition(&Idle_state,			&AggroPathing_state,	&EnemyBase::Idle_AggroPathing				);
				AddTransition(&AggroPathing_state,	&Stunned_state,			&EnemyBase::IsStunned						);
				AddTransition(&AggroPathing_state,	&Idle_state,			&EnemyBase::NoTarget						);
				AddTransition(&AggroPathing_state,	&Attack_state,			&EnemyBase::AggroPathing_Attack);
				AddTransition(&AggroPathing_state,	&ResetPathing_state,	&EnemyBase::TargetNotWithinReset			);
				AddTransition(&ResetPathing_state,	&Idle_state,			&EnemyBase::IsBackAtSpawn					);
				AddTransition(&Attack_state,		&Stunned_state,			&EnemyBase::IsStunned						);
				AddTransition(&Attack_state,		&Idle_state,			&EnemyBase::Attack_Idle						);
				AddTransition(&Attack_state,		&ResetPathing_state,	&EnemyBase::Attack_ResetPathing				);
				AddTransition(&Attack_state,		&AggroPathing_state,	&EnemyBase::Attack_AggroPathing				);
				AddTransition(&Stunned_state,		nullptr,				&EnemyBase::IsNotStunned					);
			}
		};
		static EnemyFSM fsm;

		// static FSM Construction END_____________________________________________________
		struct EnemyStatics
		{
			PlayerSurround* playerSurround = nullptr;
			float tile_size = 1.5f;
		};
		static EnemyStatics statics;

		Vec3f oldPosition;
		PersonalState<This> current_state{ this };
		State<This>* previous_state;
		SurroundPos surroundPosition = SurroundPos(0,0);
		Vec3f playerOldPos;
		bool SkipOnce = false;
		bool IsMelee = false;
		bool Asleep = false;
		
		CharacterScript* charac_base = nullptr;
		CharacterPather* charac_pather = nullptr;
		EnemySpecialized* enemy_specialize = nullptr;

		float LinearDamping = 5.0;

		// SFX
		//AssetID SFX_Death = AssetID(4209584761553053619);

	private:
		inline bool TargetIsWithin_AttackRange() const;
		inline bool TargetNotWithinReset() const;
		inline bool IsBackAtSpawn() const;
		inline bool TargetNotWithin_AttackRange() const;
		inline Vec3f GetNormDirectionToEntity(EntityID in);
		inline float SqrDistFromEntity(EntityID in) const;
		inline bool IsAtPoint(Vec3f const& point) const;

	public:
		inline void SetTarget(EntityID in) { charac_base->SetTarget(in); }
		inline void SetAsleep(bool sleep) { Asleep = sleep; }
		inline bool IsDead() const { return charac_base->IsDead(); }
		inline bool HasTarget() const { return charac_base->HasTarget(); }
		inline bool NoTarget() const { return charac_base->NoTarget(); }
		inline bool IsNotStunned()const { return !charac_base->IsStunned();}
		inline bool IsStunned() const { return charac_base->IsStunned();}
		inline bool IsAttackCharging() const { if (!enemy_specialize) return false; return enemy_specialize->IsAttackCharging(); }
		inline void SetSpecialization(EnemySpecialized* in) { enemy_specialize = in; }
		
		void Awake();
		void Init();
		void Update(double dt);
		void Cleanup();
		// calls by System when the behaviour instance is asleep, if returns true, wakes the behaviour itself
		bool SelfWakeCondition();
		void UponSleep();
		void UponWake();
		void OnCollision(EntityID collidingID);
		void OnPersistedCollision(EntityID collidingID);
		void OnCollisionRemoved(EntityID collidingID);

		inline void UpdateTimers(float dt);

	// transition functions
	private:
		inline bool Attack_Idle() const {
			return NoTarget();
		}
		inline bool Attack_ResetPathing() const{
			return TargetNotWithinReset() && !IsAttackCharging();
		}
		inline bool Attack_AggroPathing() const{
			return TargetNotWithin_AttackRange() && !IsAttackCharging();
		}
		inline bool AggroPathing_Attack() const {
			bool finishPath = IsMelee ? charac_pather->NoPath() : true;
			return finishPath && TargetIsWithin_AttackRange();
		}
		inline bool Idle_AggroPathing() const {
			return !Asleep && HasTarget() && ECS->GetComponent<Physics>(id).GetLinearVelocity().y >= 0.f;
		}
		inline Vec3f GetColliderFront() const {
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f frontal = ECS->GetComponent<Transform>(id).GetPosition() + rotMatrix * (Vec3f::Forward() * ECS->GetComponent<Physics>(id).GetShapeSize().z * 0.5f);
			return frontal;
		}
		inline float SqrDistToTargetColliderFront(EntityID _target) const {
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(_target).GetRotation());
			Vec3f targetsfront = ECS->GetComponent<Transform>(_target).GetPosition() + rotMatrix * (Vec3f::Forward() * ECS->GetComponent<Physics>(_target).GetShapeSize().z * 0.5f);
			return GetColliderFront().SqDistBetween(targetsfront);
		}
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}

#endif