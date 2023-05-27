#pragma once
#ifndef	ENEMY_ROBOT_H
#define ENEMY_ROBOT_H

#include "EnemyScript.h"

namespace TH
{
	using namespace BE;

	enum class RobotAnim
	{
		Attack,
		Death,
		Damage,
		Slip,
		Walk,
		Total_Animations
	};

	class EnemyRobot : public EnemySpecialized
	{
		AttackSkill eMelee_attack;
		EntityID eModelID = NULL_ENTITY;

		// Animation
		std::array<AnimInfo, static_cast<size_t>(RobotAnim::Total_Animations)>
			pAnim_Indexes;
		float lastHPknown;
		RobotAnim pCurrent_Anim = RobotAnim::Walk;
		RobotAnim pPrev_Anim = RobotAnim::Walk;
		bool CastedAttack = false;

		void Awake();
		void Init();
		void Update(double dt);
		inline void UpdateTimers(float dt);

		inline void MeleeAttack()
		{
			CastedAttack = true;
		}

		inline void MeleeAttackDamage()
		{
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f cast_origin = ECS->GetComponent<Transform>(id).GetPosition() + rotMatrix * Vec3f::Forward() * eMelee_attack.aAOE_size.z;
			std::vector<Vec3f> contact_pts;
			eMelee_attack.CastSkill(id, cast_origin, charac_base->GetBaseAttack(), contact_pts,
				NULL_ENTITY, EntityTagFilter({ "Enemy" }, FilterType::DoesNotContainAny));
		}
		void CastAttack()		  { MeleeAttack();	}
		BaseSkill* GetBaseSkill() { return &eMelee_attack; }
		void AutoResetAnimation(float dt);
		void InitAnimationInfo(EntityID modelID);

		//damaged sound

	public:
		bool SetAnimState(RobotAnim in, bool forced = false);

		inline bool BaseSetAnimation(EnemyBaseAnims base, bool force = false) override
		{
			switch (base) {
			case EnemyBaseAnims::Attack: {
				return SetAnimState(RobotAnim::Attack, force);
				break;
			}
			case EnemyBaseAnims::Damage: {
				return SetAnimState(RobotAnim::Damage, force);
				break;
			}
			case EnemyBaseAnims::Death: {
				return SetAnimState(RobotAnim::Death, force);
				break;
			}
			case EnemyBaseAnims::Slip: {
				return SetAnimState(RobotAnim::Slip, force);
				break;
			}
			case EnemyBaseAnims::Walk: {
				return SetAnimState(RobotAnim::Walk, force);
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