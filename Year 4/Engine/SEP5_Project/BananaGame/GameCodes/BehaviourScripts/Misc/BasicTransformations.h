#pragma once

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;

	class RotationScript : public IScript
	{
	private:
		float rateOfRotation = 90.0f;
		bool enable = true;

	public:			
		void Update(double dt) override;
		inline void ToggleStartStop(bool in) { enable = in; }

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}
