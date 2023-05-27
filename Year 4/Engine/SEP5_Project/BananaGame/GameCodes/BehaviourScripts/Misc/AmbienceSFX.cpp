#include "AmbienceSFX.h"
namespace TH
{
	void AmbienceSFX::Init()
	{
		AssertCore(ECS->HasComponent<AudioSource>(id),"Ambience emitter does not have audio source component");
		AudioSource& as = ECS->GetComponent<AudioSource>(id);
		for (const auto& pair : as.GetAllAudio())
		{
			as.Play(pair.first);
		}
	}
	void AmbienceSFX::Update(double dt)
	{
		if (upperRange == 0.0f)
		{
			return;
		}
		AudioSource& as = ECS->GetComponent<AudioSource>(id);
		AssetID audioID = as.GetAllAudio().begin()->first;
		float fdt = static_cast<float>(dt);
		if (!as.IsAudioPlaying(audioID))
		{
			if (timer <= 0.0f)
			{
				as.Play(audioID);
				timer = Rand::Random(lowerRange, upperRange);
			}
			else
			{
				timer -= fdt;
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
	registration::class_<AmbienceSFX>(RegistrationNameCreator<AmbienceSFX>())
		.constructor()
		.property("x", &AmbienceSFX::lowerRange)
		.property("y", &AmbienceSFX::upperRange);
}
