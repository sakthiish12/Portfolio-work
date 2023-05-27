#include "AnimatedTexture.h"

namespace TH
{
	using namespace BE;

	void AnimatedTexture::Awake()
	{
	}

	void AnimatedTexture::Init()
	{
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor1.dds"));
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor2.dds"));
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor3.dds"));
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor4.dds"));
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor5.dds"));
		frames.emplace_back(AssetMgr->GetAssetID("../Assets/Textures/ElectricFloor/ElectricFloor6.dds"));

		swapTimer.duration = 1/ framesratePerSecond;
		if (InitPlaying) {
			playing = true;
			swapTimer.start = true;
		}
	}

	void AnimatedTexture::Update(double dt)
	{
		swapTimer.update((float)dt);

		if (playing) {

			if (swapTimer.ended)
			{
				Renderer& renderer = ECS->GetComponent<Renderer>(id);
				frame_idx++;
				if (frame_idx >= frames.size())
					frame_idx = 0;
				renderer.setAlbedoAlphaMap(frames[frame_idx], 0);

				swapTimer.StartTimer();
			}
		}
	
	}

}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<AnimatedTexture>(RegistrationNameCreator<AnimatedTexture>())
		.constructor();
}