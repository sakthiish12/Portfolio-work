#pragma once
#include "AreaZone.h"


namespace TH
{
	using namespace BE;
	void AreaZone::Init()
	{
		const auto results = ECS->FindEntitiesByName("GameManager");
		if (results.size() != 1)
		{
			AssertCore(false, "Game manager entity is not found");
		}
		else {
			auto script_ptr = ECS->GetComponent<GameLogic>(results.front()).GetScript<LevelManager>();
			levelmgr = dynamic_cast<LevelManager*>(script_ptr.get());
		}
	}

	void AreaZone::OnCollision(EntityID collidingID)
	{
		if (collidingID == GLS->GetPlayerID() && levelmgr) {
			levelmgr->SetPlayerZone(ZoneNumber);
		}
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<AreaZone>(RegistrationNameCreator<AreaZone>())
		.constructor()
		.property("Zone Number", &AreaZone::ZoneNumber);
}