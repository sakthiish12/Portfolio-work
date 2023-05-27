#pragma once

#include <BananaEngine.h>
#include "../Misc/Timers.h"

namespace TH
{
	using namespace BE;
	class DOTObject : public IScript
	{
		std::string ignoreTag = "";
		BEID dAttackerID = NULL_ENTITY;
		float dDamage_Multplr = 0.1f;
		float dBase_Attack = 10.f;
		float dDamage_Rate = 1.f;
		Timer dDamage_CD_timer;

		bool dIgnore_Enemy = false;

		void DamageOverTime(BEID collidingID);
		void Init() override;
		void Update(double) override;
		void OnPersistedCollision(BEID collidingID) override;
		void OnCollision(BEID collidingID) override;

	public:
		inline void SetAttackerID(BEID attacker) { dAttackerID = attacker; }
		inline void SetBaseAttack(float baseattack) { dBase_Attack = baseattack; }
		inline void SetDamageMultiplier(float damageM) { dDamage_Multplr = damageM; }
		inline void SetIgnoreTag(std::string const& ignore) { ignoreTag = ignore; }
		inline void SetDamageRate(float rate) {
			dDamage_Rate = rate;
			dDamage_CD_timer.duration = 1 / rate;
		}

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND

	};

}

