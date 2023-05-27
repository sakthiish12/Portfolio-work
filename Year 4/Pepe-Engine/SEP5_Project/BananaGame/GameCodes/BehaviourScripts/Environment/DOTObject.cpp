#include "DOTObject.h"
#include "../Character/CharacterScript.h"
namespace TH
{
	using namespace BE;

	void DOTObject::DamageOverTime(BEID collidingID)
	{
		if (ignoreTag.size() && ECS->GetComponent<EntityInfo>(collidingID).HasTag(ignoreTag)) return;

		// return if the attacker is colliding with this object
		if (dDamage_CD_timer.IsTimerRunning()) return;

		// return if the attacker is colliding with this object
		if (dAttackerID == collidingID) return;

		// return if it ignores enemy and the colliding object is not player.
		if (dIgnore_Enemy && GLS->GetPlayerID() != collidingID) return;

		// Check if the colliding has characterScript
		if (ECS->HasComponent<GameLogic>(collidingID))
		{
			auto game_script = ECS->GetComponent<GameLogic>(collidingID).GetScript<CharacterScript>();
			CharacterScript* charac_ptr = dynamic_cast<CharacterScript*>(game_script.get());

			if (charac_ptr != nullptr)
			{
				// deal damage to the collidng character script and start the damage cooldown timer
				dDamage_CD_timer.start = true;
				charac_ptr->DamageHealth(dDamage_Multplr * dBase_Attack);
			}
		}
	}

	void DOTObject::Init()
	{
		dDamage_CD_timer.duration = 1 / dDamage_Rate;
	}

	void DOTObject::Update(double dt)
	{
		dDamage_CD_timer.update((float)dt);
	}
	
	void DOTObject::OnPersistedCollision(BEID collidingID)
	{
		DamageOverTime(collidingID);
	}

	void DOTObject::OnCollision(BEID collidingID)
	{
		if (ECS->GetComponent<Physics>(collidingID).GetIsVirtualCharacter()) {
			DamageOverTime(collidingID);
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<DOTObject>(RegistrationNameCreator<DOTObject>())
		.constructor()
		.property("Damage Multiplier", &DOTObject::dDamage_Multplr)
		.property("Base Attack", &DOTObject::dBase_Attack)
		.property("Damage Rate", &DOTObject::dDamage_Rate);
}