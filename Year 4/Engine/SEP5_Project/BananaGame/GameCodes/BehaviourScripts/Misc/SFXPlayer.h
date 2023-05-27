#pragma once
#ifndef	SFXPLAYER_H
#define SFXPLAYER_H

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;
	class SFXPlayer : public IScript
	{
		float counter;
	public:
		void Setup(const AssetID sfx, float length, float volume = 1.0f);
		void Update(double);
		RTTR_ENABLE(IScript)
	};

}

#endif
