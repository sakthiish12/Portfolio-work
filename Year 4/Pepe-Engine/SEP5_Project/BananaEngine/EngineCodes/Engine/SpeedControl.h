#pragma once
#include "../Utilities/Singleton/Singleton.h"

namespace BE
{
	class SpeedControl : public Singleton<SpeedControl>
	{
	private:
		float dt_Multiplier = 1.f;
	public:
		void SetEngineSpeedMultiplier(float multiplier);
		float GetEngineSpeedMultiplier();
		inline void ResetEngineSpeedMultiplier() { dt_Multiplier = 1.f; }

#define EngineSpeed SpeedControl::GetInstance()
	};



}
