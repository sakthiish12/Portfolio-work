#pragma once
#ifndef	ENEMY_SPECIALIZED_H
#define ENEMY_SPECIALIZED_H

#include <BananaEngine.h>
#include "../Character/BaseSkill.h"
#include "../Character/CharacterScript.h"
#include "../Character/CharacterPather.h"
namespace TH
{

	enum class EnemyType {
		Melee,
		Ranged,
		Boss
	};

	enum class EnemyBaseAnims
	{
		Attack,
		Death,
		Damage,
		Slip,
		Walk,
		Total_Animations
	};

	// used as base class to make derived enemies.
	// derived enemy contains their special attacks and stats etc.
	class EnemySpecialized : public IScript
	{
	protected:
		EnemyType eEnemy_Type;
		CharacterScript* charac_base = nullptr;

	public:
		bool pIsPlayingOneShot = false;
		bool HasSetAnimation_frame = false;

		void Init();
		void Awake();
		virtual void AutoResetAnimation(float dt) {}
		virtual bool BaseSetAnimation(EnemyBaseAnims base, bool force = false) { return true; }
		virtual void CastAttack() = 0;
		virtual BaseSkill* GetBaseSkill() { return nullptr; }
		virtual bool IsAttackCharging() { return false; }
		virtual void CancelCharge() {}
		inline EnemyType GetEnemyType() const { return eEnemy_Type; }
	};

}

#endif