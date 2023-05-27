#pragma once
#ifndef	ENEMY_SCIENTIST_H
#define ENEMY_SCIENTIST_H

#include "EnemyScript.h"

namespace TH
{
	using namespace BE;

	enum class RangedAnim
	{
		Walk,
		Charge,
		Attack,
		Death,
		Damage,
		Total_Animations
	};

	class EnemyScientist : public EnemySpecialized
	{
		ChargedProjectile eRange_attack;
		EntityID eModelID = NULL_ENTITY;
		float lastHPknown;

		// Animation
		std::array<AnimInfo, static_cast<size_t>(RangedAnim::Total_Animations)>
			pAnim_Indexes;
		RangedAnim pCurrent_Anim = RangedAnim::Walk;
		RangedAnim pPrev_Anim = RangedAnim::Walk;

		void Awake();
		void Init();
		void Update(double dt);
		inline void UpdateTimers(float dt);

		inline void RangedAttack()
		{
			EntityID target = charac_base->GetTarget();
			//float forward_scale = ECS->GetComponent<Physics>(id).GetShapeSize().z * 0.5f * 0.5f;

			eRange_attack.ChargeAndCast(Vec3f(0.f, 0.25f, 0.35f + eRange_attack.pRadius), Quatf() ,id, charac_base->GetBaseAttack());
		}
		void CastAttack() { RangedAttack(); }
		BaseSkill* GetBaseSkill() { return &eRange_attack; }
		bool IsAttackCharging() override { return eRange_attack.IsCharging(); }
		void CancelCharge() override
		{
			eRange_attack.CancelCharge();
		}
		void AutoResetAnimation(float dt);
		void InitAnimationInfo(EntityID modelID);

	public:
		bool SetAnimState(RangedAnim in, bool forced = false);

		inline bool BaseSetAnimation(EnemyBaseAnims base, bool force = false) override
		{
			switch (base) {
			case EnemyBaseAnims::Attack: {
				return SetAnimState(RangedAnim::Attack, force);
				break;
			}
			case EnemyBaseAnims::Damage: {
				return SetAnimState(RangedAnim::Damage, force);
				break;
			}
			case EnemyBaseAnims::Death: {
				return SetAnimState(RangedAnim::Death, force);
				break;
			}
			case EnemyBaseAnims::Slip: {
				break;
			}
			case EnemyBaseAnims::Walk: {
				return SetAnimState(RangedAnim::Walk, force);
				break;
			}
			}
			return true;
		}
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}
#endif