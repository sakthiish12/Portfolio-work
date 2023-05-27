
#include "BossDecoy.h"

namespace TH
{
	using namespace BE;

	void BossDecoy::Awake()
	{

	}
	void BossDecoy::Init()
	{
		firstHit = false;
		GetOrAddOtherRequiredScripts<CharacterScript>(charac_base, id);

		const auto boss = ECS->FindEntitiesByName("Boss FireWall", 1);
		if (boss.empty()) {
			LogCoreError("Error finding Boss FireWall entity!");
		}
		else {
			bossMainBody = boss.front();
		}

		const auto model = ECS->FindEntitiesByName("Boss Decoy Model", 1);
		if (model.empty()) {
			LogCoreError("Error finding Boss Decoy Model entity!");
		}
		else {
			charac_base->SetModelID(model.front());
			ECS->GetComponent<EntityInfo>(model.front()).SetFlag(EntityFlag::active, false);
		}
		charac_base->SetKnockBackImmune(true);
		fixedPos = ECS->GetComponent<Transform>(id).GetPosition();
		firstHit = false;
		ECS->GetComponent<EntityInfo>(id).AddTag("Boss");
		ECS->GetComponent<Physics>(id).SetIsTrigger(true);

	}

	void BossDecoy::Update(double dt)
	{
		spawnInvuln.update((float)dt);
		if (spawnInvuln.ended) {
			auto charac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>());
			if (charac) {
				charac->SetInvulnerable(false);
			}
		}
		ECS->GetComponent<Transform>(id).SetPosition(fixedPos);
		if (!firstHit && charac_base->cTookDamageFrame) {
			firstHit = true;
			if (ECS->DoesEntityExists(bossMainBody)) {
				auto bossCharac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(bossMainBody).GetScript<CharacterScript>());
				if (bossCharac) {
					bossCharac->SetTarget(GLS->GetPlayerID());
				}
			}
		}
	}
	void BossDecoy::Respawn()
	{
		firstHit = false;
		auto charac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>());
		if (charac) {
			charac->SetInvulnerable(true);
			charac->cTookDamageFrame = false;
		}
		spawnInvuln.StartTimer();
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<BossDecoy>(RegistrationNameCreator<BossDecoy>())
		.constructor();
}