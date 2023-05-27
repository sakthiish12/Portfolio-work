#pragma once

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;

	struct Timer
	{
		float duration = 1.0f;
		float timer = 0.f;
		bool start = false;
		bool ended = false;
		Timer() = default;
		Timer(float _duration) : duration{ _duration }
		{
		}
		void update(float dt) {
			ended = false;
			if (start) {
				timer += dt;
				if (timer >= duration) {
					timer = 0.f;
					start = false;
					ended = true;
				}
			}
		}
		inline bool IsTimerEnded() { return ended; }
		inline void StartTimer() { start = true; }
		inline bool IsTimerRunning() const { return timer > 0.f && start; }
		inline void ResetTimer() { timer = 0.f; }
	};

	struct CoolDown: public Timer
	{
		CoolDown() = default;
		CoolDown(float _duration) : Timer(duration)
		{
		}
		inline bool IsCooldownEnded() { return IsTimerEnded(); }
		inline bool IsOnCoolDown() { return IsTimerRunning(); }
		inline void reset_Cooldown() { ResetTimer(); }
	};

	struct Charge
	{
		float duration = 2.0f;
		float timer = 0.f;
		bool isCharged = false;
		Charge() = default;
		Charge(float _duration) : duration{ _duration }
		{
		}
		void update_Charging(float dt) {
			timer += dt;
			if (timer >= duration) {
				timer = 0.f;
				isCharged = true;
			}
		}
		inline void reset_Charge() { isCharged = false;  timer = 0.f; }
		inline bool IsCharging() const { return timer > 0.f; }
	};


}