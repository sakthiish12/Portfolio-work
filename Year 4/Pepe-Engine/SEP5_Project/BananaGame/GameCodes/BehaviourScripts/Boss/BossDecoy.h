#pragma once

#include <BananaEngine.h>
#include "../Character/CharacterScript.h"
#include "../Misc/Timers.h"
namespace TH
{
	using namespace BE;

	class BossDecoy : public IScript
	{
		CharacterScript* charac_base = nullptr;
		Timer spawnInvuln = Timer(2.0f);
		Vec3f fixedPos;
		EntityID bossMainBody;
		bool firstHit = false;
	public:
		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		 void Respawn();
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}