#include "pch.h"
#include <Maths/MathLib.h>
#include "AudioSource.h"

#include <AssetManager/AssetManager.h>
#include <Systems/AudioManager/AudioManager.h>
#include <Serialisation/BananaSerialisation.h>


namespace BE
{
	AudioProperties::AudioProperties() : m_pitch(1.0f), m_volume(1.0f), m_priority(128), m_flags(0), m_minDistance(1.0f), m_maxDistance(10000.0f)
	{

	}

	AudioProperties::AudioProperties(float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist)
		: m_pitch(_pitch), m_volume(_volume), m_priority(_priority), m_flags(0), m_minDistance(_minDist), m_maxDistance(_maxDist)
	{
		m_flags.set((size_t)AudioFlag::isMusic, _isBGM);
		m_flags.set((size_t)AudioFlag::isMute, _isMuted);
	}





	void AudioSource::AddAudio(BEID _assetID, float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist)
	{
		AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(_assetID), "Asset %llu is not loaded for audio clip", _assetID);
		m_allAudio[_assetID] = AudioProperties(_pitch, _volume, _priority, _isBGM, _isMuted, _minDist, _maxDist);
	}

	void AudioSource::AddAudio(BEID _assetID)
	{
		AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(_assetID), "Asset %llu is not loaded for audio clip", _assetID);
		m_allAudio[_assetID] = AudioProperties();
	}

	void AudioSource::AddAudio(const std::string& _filePath, float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist)
	{
		AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(_filePath), "Asset %s is not loaded for audio clip", _filePath.c_str());
		AddAudio(AssetMgr->GetAssetID(_filePath), _pitch, _volume, _priority, _isBGM, _isMuted, _minDist, _maxDist);
	}

	void AudioSource::AddAudio(const std::string& _filePath)
	{
		AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(_filePath), "Asset %s is not loaded for audio clip", _filePath.c_str());
		AddAudio(AssetMgr->GetAssetID(_filePath));
	}

	void AudioSource::RemoveAudio(BEID _assetID)
	{
		//Remove the sound from being played
		AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);

		AudioMgr->StopAudio(ae);
		AudioMgr->RemoveFromQueue(ae);

		m_allAudio.erase(_assetID);
	}

	void AudioSource::RemoveAudio(const std::string& _filePath)
	{
		this->RemoveAudio(AssetMgr->GetAssetID(_filePath));
	}
	
	void AudioSource::RemoveAllAudio()
	{
		auto begin = m_allAudio.begin();
		auto end = m_allAudio.end();

		while (begin != end)
		{
			RemoveAudio((begin++)->first);
		}
	}


	void AudioSource::SetVolume(float _newVol, BEID _assetID)
	{
		if (_assetID)
		{
			m_allAudio.at(_assetID).m_volume = Math::Clamp(MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME, _newVol);
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);
			AudioMgr->SetVolume(ae, _newVol);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				m_allAudio.at(aID).m_volume = Math::Clamp(MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME, _newVol);
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);
				AudioMgr->SetVolume(ae, _newVol);
			}
		}
	}

	void AudioSource::SetPitch(float _newPitch, BEID _assetID)
	{
		if (_assetID)
		{
			m_allAudio.at(_assetID).m_pitch = Math::Clamp(MIN_AUDIO_PITCH, MAX_AUDIO_PITCH, _newPitch);
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);
			AudioMgr->SetPitch(ae, _newPitch);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				m_allAudio.at(aID).m_pitch = Math::Clamp(MIN_AUDIO_PITCH, MAX_AUDIO_PITCH, _newPitch);
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);
				AudioMgr->SetVolume(ae, _newPitch);
			}
		}
	}

	void AudioSource::SetPriority(int _newPriority, BEID _assetID)
	{
		if (_assetID)
		{
			m_allAudio.at(_assetID).m_priority = Math::Clamp(MAX_AUDIO_PRIORITY, MIN_AUDIO_PRIORITY, _newPriority);
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);
			AudioMgr->SetPriority(ae, m_allAudio.at(_assetID).m_priority);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				m_allAudio.at(aID).m_priority = Math::Clamp(MAX_AUDIO_PRIORITY, MIN_AUDIO_PRIORITY, _newPriority);
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);
				AudioMgr->SetPriority(ae, _newPriority);
			}
		}
	}

	void AudioSource::SetIsBGM(bool _b, BEID _assetID)
	{
		if (_assetID)
		{
			m_allAudio.at(_assetID).SetIsBGM(_b);
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);

			//Change the volume
			AudioMgr->SetVolume(ae, m_allAudio.at(_assetID).m_volume);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				m_allAudio.at(aID).SetIsBGM(_b);
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);

				//Change the volume
				AudioMgr->SetVolume(ae, m_allAudio.at(aID).m_volume);
			}
		}
	}

	void AudioSource::SetIsMuted(bool _isMute, BEID _assetID)
	{
		if (_assetID)
		{
			m_allAudio.at(_assetID).SetIsMuted(_isMute);
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);

			//Update in the audio manager side
			AudioMgr->SetMute(ae, _isMute);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				m_allAudio.at(aID).SetIsMuted(_isMute);
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);

				//Update in the audio manager side
				AudioMgr->SetMute(ae, _isMute);
			}
		}
	}

	void AudioSource::SetMinDist(float _newDist, BEID _assetID)
	{
		if (_assetID)
		{
			//Min distance cannot be above max distance
			m_allAudio.at(_assetID).m_minDistance = BEMath::Clamp(MIN_AUDIO_DISTANCE, m_allAudio.at(_assetID).m_maxDistance, _newDist);

			//Update on audio manager side
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);
			AudioMgr->Set3DRolloffDistance(ae, m_allAudio.at(_assetID).m_minDistance, m_allAudio.at(_assetID).m_maxDistance);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				//Min distance cannot be above max distance
				m_allAudio.at(aID).m_minDistance = BEMath::Clamp(MIN_AUDIO_DISTANCE, m_allAudio.at(aID).m_maxDistance, _newDist);

				//Update on audio manager side
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);
				AudioMgr->Set3DRolloffDistance(ae, m_allAudio.at(aID).m_minDistance, m_allAudio.at(aID).m_maxDistance);
			}
		}
	}

	void AudioSource::SetMaxDist(float _newDist, BEID _assetID)
	{
		if (_assetID)
		{
			//Max distance cannot be below min distance
			m_allAudio.at(_assetID).m_maxDistance = BEMath::Clamp(m_allAudio.at(_assetID).m_minDistance, MAX_AUDIO_DISTANCE, _newDist);

			//Update on audio manager side
			AudioEvent ae(ECS->FindOwnerOfComponentData(*this), _assetID);
			AudioMgr->Set3DRolloffDistance(ae, m_allAudio.at(_assetID).m_minDistance, m_allAudio.at(_assetID).m_maxDistance);
		}
		else
		{
			for (const BEID& aID : std::views::keys(m_allAudio))
			{
				//Min distance cannot be above max distance
				m_allAudio.at(aID).m_maxDistance = BEMath::Clamp(m_allAudio.at(aID).m_minDistance, MAX_AUDIO_DISTANCE, _newDist);

				//Update on audio manager side
				AudioEvent ae(ECS->FindOwnerOfComponentData(*this), aID);
				AudioMgr->Set3DRolloffDistance(ae, m_allAudio.at(aID).m_minDistance, m_allAudio.at(aID).m_maxDistance);
			}
		}
	}

	void AudioSource::Play(BEID _assetID)
	{
		AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(_assetID), "Audio clip %llu is not loaded", _assetID);

		EntityID e = ECS->FindOwnerOfComponentData(*this);
		AudioEvent ae(e, _assetID);

		//If there's such an audio clip attached then play it
		if (m_allAudio.contains(_assetID))
		{
			AudioMgr->PlayAudio(ae, m_allAudio.at(_assetID));
			return;
		}
		std::stringstream ss;
		ss << "Audio file" << _assetID << "was not attached to the audio source for entity " << e << "(" << ECS->GetComponent<EntityInfo>(e).GetName().c_str() << ")";
		LogCoreWarning(ss.str().c_str());
	}

	//Pause/Unpaused this audio source, does nothing if audio is not played
	void AudioSource::Pause(BEID _assetID, bool _isPause)
	{
		EntityID e = ECS->FindOwnerOfComponentData(*this);
		
		//Don't do anything if this audio source does not have the given audio asset
		if (!m_allAudio.contains(_assetID))
		{
			LogCoreWarning("Audio file %llu was not attached to the audio source for entity %llu (%s)", _assetID, e, ECS->GetComponent<EntityInfo>(e).GetName().c_str());
			return;
		}
		
		AudioEvent ae(e, _assetID);
		AudioMgr->PauseAudio(ae, _isPause);
	}

	//Stop playing this audio source
	void AudioSource::Stop(BEID _assetID)
	{
		EntityID e = ECS->FindOwnerOfComponentData(*this);

		//Don't do anything if this audio source does not have the given audio asset
		if (!m_allAudio.contains(_assetID))
		{
			LogCoreWarning("Audio file %llu was not attached to the audio source for entity %llu (%s)", _assetID, e, ECS->GetComponent<EntityInfo>(e).GetName().c_str());
			return;
		}

		AudioEvent ae(e, _assetID);
		AudioMgr->StopAudio(ae);
	}

	//Check if audio source is being paused. Return false if audio is paused or not in use
	bool AudioSource::IsAudioPause(BEID _assetID) const
	{
		EntityID e = ECS->FindOwnerOfComponentData(*this);

		//Don't do anything if this audio source does not have the given audio asset
		if (!m_allAudio.contains(_assetID))
		{	
			LogCoreWarning("Audio file %llu was not attached to the audio source for entity %llu (%s)", _assetID, e, ECS->GetComponent<EntityInfo>(e).GetName().c_str());
			return false;
		}

		AudioEvent ae(e, _assetID);
		return AudioMgr->IsAudioPause(ae);
	}

	//Check if audio source is being played. Return false if audio is not played
	bool AudioSource::IsAudioPlaying(BEID _assetID) const
	{
		EntityID e = ECS->FindOwnerOfComponentData(*this);

		//Don't do anything if this audio source does not have the given audio asset
		if (!m_allAudio.contains(_assetID))
		{
			LogCoreWarning("Audio file %llu was not attached to the audio source for entity %llu (%s)", _assetID, e, ECS->GetComponent<EntityInfo>(e).GetName().c_str());
			return false;
		}

		AudioEvent ae(e, _assetID);
		return AudioMgr->IsAudioPlaying(ae);
	}

	//Pause/Unpause all the audio linked to this audio source
	void AudioSource::PauseAllAudio(bool _isPause)
	{
		for (const BEID& aID : std::views::keys(m_allAudio))
			this->Pause(aID, _isPause);
	}

	//PauseStop all audio linked to this audio source
	void AudioSource::StopAllAudio()
	{
		for (const BEID& aID : std::views::keys(m_allAudio))
			this->Stop(aID);	
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<AudioProperties>(RegistrationNameCreator<AudioProperties>())
		.constructor<>()(
			policy::ctor::as_object
		)
		.constructor<float, float, int, bool, bool, float, float>()
		.property("Volume", &AudioProperties::m_volume)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Pitch", &AudioProperties::m_pitch)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Priority", &AudioProperties::m_priority)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Is BGM", &AudioProperties::GetIsBGM, &AudioProperties::SetIsBGM)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Is Muted", &AudioProperties::GetIsMuted, &AudioProperties::SetIsMuted)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Min Rolloff Distance", &AudioProperties::m_minDistance)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Max Rolloff Distance", &AudioProperties::m_maxDistance)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);

	registration::class_<AudioSource>(RegistrationNameCreator<AudioSource>())
		.constructor()
		.property("Audio", &AudioSource::m_allAudio)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}