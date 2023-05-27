#pragma once
#ifndef	AmbienceSFX_H
#define AmbienceSFX_H

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;
	class AmbienceSFX : public IScript
	{
		
	public:
		float timer = 0.0f;
		float lowerRange = 0.0f;
		float upperRange = 0.0f;
		void Init() override;
		void Update(double dt) override;
		RTTR_ENABLE(IScript)
	};

}

#endif
