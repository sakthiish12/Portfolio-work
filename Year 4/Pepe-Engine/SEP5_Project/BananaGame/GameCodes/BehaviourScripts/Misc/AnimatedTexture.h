#pragma once

#include <BananaEngine.h>
#include "../Misc/Timers.h"
namespace TH
{
	using namespace BE;

	class AnimatedTexture : public IScript
	{
		std::vector<AssetID> frames;
		unsigned int frame_idx = 0;
		Timer swapTimer;
		float framesratePerSecond = 30.f;
		bool playing = false;
		bool InitPlaying = true;

	public:
		inline void PlayAnimation() { playing = true; }
		inline void StopAnimation() { playing = false; }
		inline void SetFPS(float in) {
			framesratePerSecond = in;
			swapTimer.duration = 1 / framesratePerSecond;
		}

		void Awake() override;
		void Init() override;
		void Update(double dt) override;

		
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}