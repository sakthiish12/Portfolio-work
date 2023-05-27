#include "pch.h"
#include "SpeedControl.h"

namespace BE
{
	void SpeedControl::SetEngineSpeedMultiplier(float multiplier)
	{
		dt_Multiplier = multiplier;
	}

	 float SpeedControl::GetEngineSpeedMultiplier()
	 { 
		 return dt_Multiplier; 
	 }
}