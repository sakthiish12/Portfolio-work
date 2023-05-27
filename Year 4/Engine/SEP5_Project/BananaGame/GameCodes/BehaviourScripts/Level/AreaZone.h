#pragma once
#include <BananaEngine.h>
#include "LevelManager.h"


namespace TH
{
	using namespace BE;

	class AreaZone : public IScript
	{
		LevelManager* levelmgr = nullptr;
		// default 0 meaning not a zone.
		int ZoneNumber = 0;

	public:
		void Init() override;
		void OnCollision(EntityID collidingID) override;


		inline int GetZoneNumber() const {
			return ZoneNumber;
		}

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}