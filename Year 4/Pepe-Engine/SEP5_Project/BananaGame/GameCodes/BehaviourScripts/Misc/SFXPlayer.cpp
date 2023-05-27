#include "SFXPlayer.h"

void TH::SFXPlayer::Setup(const AssetID sfx, float length, float volume)
{
	ECS->AddComponent<AudioSource>(id);
	AudioSource& as = ECS->GetComponent<AudioSource>(id);
	as.AddAudio(sfx);
	if (volume != 1.0f)
		as.SetVolume(volume, sfx);
	counter = length;
	as.Play(sfx);
}

void TH::SFXPlayer::Update(double dt)
{
	counter -= static_cast<float>(dt);
	if (counter <= 0.0f)
	{
		ECS->DestroyEntity(id);
	}
}
