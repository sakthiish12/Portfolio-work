#pragma once
#ifndef	STRESS_TEST_H
#define STRESS_TEST_H

#include <BananaEngine.h>
namespace TH
{
	using namespace BE;

	class StressTest : public IScript
	{
		std::vector<std::pair<BEID,bool>> entities;
	private:
		void Awake();
		void Init();
		void UpdateTimers(float dt);
		void Update(double dt);
		void Cleanup();
		void UponSleep();
		void UponWake();
		void OnCollision(EntityID collidingID);
		void OnPersistedCollision(EntityID collidingID);
		void OnCollisionRemoved(EntityID collidingID);
	};
}

#endif