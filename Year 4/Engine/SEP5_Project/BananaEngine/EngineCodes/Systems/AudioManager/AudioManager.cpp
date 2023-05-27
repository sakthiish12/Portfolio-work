#include "pch.h"
#include "AudioManager.h"
#include "../ECS/ECS.h"
#include "../InputController/InputController.h"

namespace BE
{
	AudioManager::AudioManager(int _numChannels, float _musicVol, float _soundVol)
		: m_maxChannelsCount(_numChannels), m_globalMusicVol(_musicVol), m_globalSoundVol(_soundVol)
	{
		//Create a new FMOD System
		if (FMOD::System_Create(&m_fmodsys) == FMOD_OK)
		{
			m_fmodsys->init((int)m_maxChannelsCount, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);
			LogCore("Created Audio Manager");
		}
		else
		{
			LogCoreError("Failed to create fmod system");
			throw std::runtime_error("Failed to create fmod system");
		}

		//Init the 3D attribute of the fmod system
		FMOD_VECTOR pos =		{ 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR forward =	{ Vec3f::Backward().x, Vec3f::Backward().y, Vec3f::Backward().z };
		FMOD_VECTOR up =		{ Vec3f::Up().x,	  Vec3f::Up().y,	  Vec3f::Up().z };

		m_fmodsys->set3DListenerAttributes(0, &pos, nullptr, &forward, &up);
		m_fmodsys->setSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT, -1.0f, 0.0f, true);
		m_fmodsys->setSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT, 1.0f, 0.0f, true);

		//Init all channels to be nullptr
		m_audioInPlay = decltype(m_audioInPlay)(_numChannels, AudioEvent{});

		m_audioQueue.reserve(50);
		
		//Subscribing to events
		EventMgr->Subscribe(EntityDestroyedEvent, this, AudioManager::OnEntityDestroyed);
		EventMgr->Subscribe(GamePauseEvent, this, AudioManager::OnPause);
	}

	AudioManager::~AudioManager()
	{
		//Stop all the channels
		StopAllChannels();

		//destroy fmod system
		m_fmodsys->release();

		LogCore("Destroyed Audio Manager");
	}

	//Set the global music volume
	void AudioManager::SetGlobalMusicVol(float _newVol)
	{
		//Clamp the _newVol
		m_globalMusicVol = BEMath::Clamp(MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME, _newVol);
		
		//For each channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			//If playing audio from an entity
			if (m_audioInPlay[i].IsEntity())
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(m_audioInPlay[i].m_entity_or_ui_ID);
			
				//IF the audio source is "music", then change it's volume
				if (as.GetIsBGM(m_audioInPlay[i].m_audioClip))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setVolume(m_globalMusicVol * as.GetVolume(m_audioInPlay[i].m_audioClip));
				}
			}

			//Ignore UI Element because they will always be "sound" instead of "music"
		}
	}

	//Set the sound volume volume
	void AudioManager::SetGlobalSoundVol(float _newVol)
	{
		//Clamp the _newVol
		m_globalSoundVol = BEMath::Clamp(MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME, _newVol);
		
		//For each channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			//If playing audio from an entity
			if (m_audioInPlay[i].IsEntity())
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(m_audioInPlay[i].m_entity_or_ui_ID);

				//IF the audio source is "music", then change it's volume
				if (!as.GetIsBGM(m_audioInPlay[i].m_audioClip))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setVolume(m_globalSoundVol * as.GetVolume(m_audioInPlay[i].m_audioClip));
				}
			}
			//If playing audio from a UI element
			else if(m_audioInPlay[i].IsUI())
			{
				//Get the canvas
				const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(m_audioInPlay[i].m_canvasID);
				std::shared_ptr<UIElement>& uiBase = canvas->GetUI(m_audioInPlay[i].m_entity_or_ui_ID);

				//dynamic cast to interactable UI Element
				std::shared_ptr<UIInteractable> uiInteract = std::dynamic_pointer_cast<UIInteractable>(uiBase);
				AssertCore((bool)uiInteract, "Incorrect dynamic casting");

				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setVolume(m_globalSoundVol * uiInteract->GetVolume());
			}
		}
	}

	//Set the global music pitch
	void AudioManager::SetGlobalMusicPitch(float _newPitch)
	{
		//Clamp the _newPitch
		m_globalMusicPitch = BEMath::Clamp(MIN_AUDIO_PITCH, MAX_AUDIO_PITCH, _newPitch);

		//For each channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			//If playing audio from an entity
			if (m_audioInPlay[i].IsEntity())
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(m_audioInPlay[i].m_entity_or_ui_ID);

				//IF the audio source is "music", then change it's pitch
				if (as.GetIsBGM(m_audioInPlay[i].m_audioClip))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setPitch(m_globalMusicPitch * as.GetPitch(m_audioInPlay[i].m_audioClip));
				}
			}

			//Ignore UI Element because they will always be "sound" instead of "music"
		}
	}

	//Set the global sound pitch
	void AudioManager::SetGlobalSoundPitch(float _newPitch)
	{
		//Clamp the _newPitch
		m_globalSoundPitch = BEMath::Clamp(MIN_AUDIO_PITCH, MAX_AUDIO_PITCH, _newPitch);

		//For each channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			//If playing audio from an entity
			if (m_audioInPlay[i].IsEntity())
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(m_audioInPlay[i].m_entity_or_ui_ID);

				//IF the audio source is "music", then change it's pitch
				if (!as.GetIsBGM(m_audioInPlay[i].m_audioClip))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setPitch(m_globalSoundPitch * as.GetPitch(m_audioInPlay[i].m_audioClip));
				}
			}
			//If playing audio from a UI element
			else if (m_audioInPlay[i].IsUI())
			{
				//Get the canvas
				const std::unique_ptr<UICanvas>& canvas = UI->GetCanvas(m_audioInPlay[i].m_canvasID);
				std::shared_ptr<UIElement>& uiBase = canvas->GetUI(m_audioInPlay[i].m_entity_or_ui_ID);

				//dynamic cast to interactable UI Element
				std::shared_ptr<UIInteractable> uiInteract = std::dynamic_pointer_cast<UIInteractable>(uiBase);
				AssertCore((bool)uiInteract, "Incorrect dynamic casting");

				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPitch(uiInteract->GetPitch());
			}
		}
	}



	//Number of channels currently playing sound
	int AudioManager::GetUsedChannelsCount() const
	{
		int count = 0;
		m_fmodsys->getChannelsPlaying(&count);
		return count;
	}

	//Play a given audio
	void AudioManager::PlayAudio(const AudioEvent& _ae, const AudioProperties& _prop)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);
		m_audioQueue.push_back(std::make_pair(_ae, _prop));
	}

	//Pause the audio given the audio source
	void AudioManager::PauseAudio(const AudioEvent& _ae, bool _isPause)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPaused(_isPause);
			}
		}
	}

	//Stop playing the audio source
	void AudioManager::StopAudio(const AudioEvent& _ae)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->stop();

				//Also remove the audio from the active audio sources
				m_audioInPlay[i].Reset();
			}
		}
	}

	//Check if audio source is being paused. Return false if audio is paused or not in use
	bool AudioManager::IsAudioPause(const AudioEvent& _ae) const
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return false;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		bool result = false;

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->getPaused(&result);
				break;
			}
		}
		return result;
	}

	//Check if audio source is being played. Return false if audio is not played
	bool AudioManager::IsAudioPlaying(const AudioEvent& _ae) const
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return false;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
				return true;
		}
		return false;
	}

	//Changes volume of a single audio source
	void AudioManager::SetVolume(const AudioEvent& _ae, float _newVol)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If not same audio then skip
			if (!m_audioInPlay[i].SameAudio(_ae.m_audioClip))
				continue;

			//Found the entity that is playing sound
			if (_ae.IsEntity() && m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID))
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(_ae.m_entity_or_ui_ID);

				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setVolume(_newVol * (as.GetIsBGM(_ae.m_audioClip) ? m_globalMusicVol : m_globalSoundVol));
			}
			//Found the UI that is playing sound
			else if (_ae.IsUI() && m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setVolume(_newVol * m_globalSoundVol);
			}
		}
	}

	//Changes Pitch of a single audio source
	void AudioManager::SetPitch(const AudioEvent& _ae, float _newPitch)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity that is playing sound
			if (_ae.IsEntity() && m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID))
			{
				//Retrieve the audio source
				const AudioSource& as = ECS->GetComponent<AudioSource>(_ae.m_entity_or_ui_ID);

				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPitch(_newPitch * (as.GetIsBGM(_ae.m_audioClip) ? m_globalMusicPitch : m_globalSoundPitch));
			}
			//Found the UI that is playing sound
			else if (_ae.IsUI() && m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPitch(_newPitch);
			}
		}
	}

	//Changes priority of a single audio source
	void AudioManager::SetPriority(const AudioEvent& _ae, int _newPriority)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPriority(_newPriority);
			}
		}
	}

	//Mutes an audio source
	void AudioManager::SetMute(const AudioEvent& _ae, bool _isMute)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setMute(_isMute);
			}
		}
	}

	//Set the distance of the rolloff
	void AudioManager::Set3DRolloffDistance(const AudioEvent& _ae, float _newMinDist, float _newMaxDist)
	{
		//IF the audio event is empty then don't do anything
		if (_ae.IsEmpty())
			return;

		AssertCore(_ae.IsValid(), "Invalid Audio Event: (ID: %llu, Canvas: %llu)", _ae.m_entity_or_ui_ID, _ae.m_canvasID);
		AssertCore(_newMinDist <= _newMaxDist, "Min Distance must be <= Max Distance");

		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioInPlay[i].SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioInPlay[i].SameAudio(_ae.m_audioClip))
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->set3DMinMaxDistance(_newMinDist, _newMaxDist);
			}
		}
	}

	//Set the distance of the rolloff
	void AudioManager::PauseAllChannels(bool _isPause, bool _forceUpdate)
	{
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			//If it's entity playing sound
			if (m_audioInPlay[i].IsEntity())
			{
				//Found the audio source 
				if (_forceUpdate || !ECS->GetComponent<EntityInfo>(m_audioInPlay[i].m_entity_or_ui_ID).HasFlag(EntityFlag::updateWhenPaused))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setPaused(_isPause);
				}
			}
			//If it's UI playing sound
			else if(m_audioInPlay[i].IsUI())
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->setPaused(_isPause);
			}
		}

		m_fmodsys->update();
	}

	//Stop playing all the channels
	void AudioManager::StopAllChannels()
	{
		//For each channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			//If no entity or UI is playing sound, then skip
			if (m_audioInPlay[i].IsEmpty())
				continue;

			FMOD::Channel* ch;
			m_fmodsys->getChannel(i, &ch);
			ch->stop();

			m_audioInPlay[i].Reset();
		}

		m_fmodsys->update();
	}

	//Remove audio from queue
	void AudioManager::RemoveFromQueue(const AudioEvent& _ae)
	{
		for (int i = 0; i < m_audioQueue.size(); ++i)
		{
			//Found the entity or UI, and the audio clip
			if (m_audioQueue[i].first.SameObj(_ae.m_entity_or_ui_ID, _ae.m_canvasID) &&
				m_audioQueue[i].first.SameAudio(_ae.m_audioClip))
			{
				//Also remove the audio from the audio queues
				m_audioQueue.erase(m_audioQueue.begin() + i);
				--i;
			}
		}
	}



	void AudioManager::Init()
	{

	}

	void AudioManager::Awake()
	{

	}

	void AudioManager::Update(double dt)
	{
		UNUSED(dt);

		//Update position of the listener
		//If there is no listener, then default to 0,0,0
		if (ECS->GetTracker<AudioListenerTracker>()->GetTrackedEntities().size() == 0)
		{
			FMOD_VECTOR pos =		{ 0.0f, 0.0f, 0.0f };
			FMOD_VECTOR forward =	{ Vec3f::Backward().x, Vec3f::Backward().y, Vec3f::Backward().z };
			FMOD_VECTOR up =		{ Vec3f::Up().x,	  Vec3f::Up().y,	  Vec3f::Up().z };

			m_fmodsys->set3DListenerAttributes(0, &pos, nullptr, &forward, &up);
		}
		//Otherwise use the listener position
		else
		{
			EntityID listener = ECS->GetTracker<AudioListenerTracker>()->GetTrackedEntities().front();
			const Vec3f& p = ECS->GetComponent<Transform>(listener).GetPosition();

			//1st column is Left axis, 2nd column is Up axis, 3rd column is Forward axis
			Mat3f m = Mat3f{}.SetRotation(ECS->GetComponent<Transform>(listener).GetRotation());
			
			FMOD_VECTOR pos =		{ p.x, p.y, p.z };
			FMOD_VECTOR forward =	{ -m[0][2], -m[1][2], -m[2][2] };
			FMOD_VECTOR up =		{ m[0][1], m[1][1], m[2][1] };

			m_fmodsys->set3DListenerAttributes(0, &pos, nullptr, &forward, &up);
		}



		//Go through the audio source queue and play their sounds
		for (const auto& [ae, ap] : m_audioQueue)
		{
			AssertCore(AssetMgr->IsAssetDataLoadedForType<AudioClip>(ae.m_audioClip),
				"Audio clip %llu was not loaded", ae.m_audioClip);
			
			//If it is EntityID playing sound
			if (ae.IsEntity())
			{
				FMOD::Channel* ch;
				m_fmodsys->playSound(AssetMgr->GetAssetData<AudioClip>(ae.m_audioClip).GetSound(), nullptr, true, &ch);

				//Set channel properties
				ch->setVolume(ap.m_volume * (ap.GetIsBGM() ? m_globalMusicVol : m_globalSoundVol));
				ch->setPitch(ap.m_pitch * (ap.GetIsBGM() ? m_globalMusicPitch : m_globalSoundPitch));
				ch->setPriority(ap.m_priority);
				ch->setMute(ap.GetIsMuted());

				//Update that this entity is using the channel
				int idx = 0;
				ch->getIndex(&idx);
				m_audioInPlay[idx] = ae;

				//If dealing with 3D sound
				if (AssetMgr->GetAssetData<AudioClip>(ae.m_audioClip).GetIs3D())
				{
					AssertCore(ap.m_minDistance <= ap.m_maxDistance, "Min Distance must be <= Max Distance");

					//Set the roll off distances
					ch->set3DMinMaxDistance(ap.m_minDistance, ap.m_maxDistance);

					//Set position of the audio source
					const Vec3f& pos = ECS->GetComponent<Transform>(ae.m_entity_or_ui_ID).GetPosition();
					FMOD_VECTOR p = { pos.x, pos.y, pos.z };
					ch->set3DAttributes(&p, nullptr);

					//Get the position of the listener
					FMOD_VECTOR listenerPos = { 0.0f, 0.0f, 0.0f };
					m_fmodsys->get3DListenerAttributes(0, &listenerPos, nullptr, nullptr, nullptr);
					Vec3f listenerPos2{ listenerPos.x, listenerPos.y, listenerPos.z };

					//Determine the new distance blending
					/*float distPercentage = 1.0f - (listenerPos2.DistBetween(pos) - ap.m_minDistance) / (ap.m_maxDistance - ap.m_minDistance);
					ch->set3DLevel(1.0f - BEMath::Clamp<float>(0.0f, 1.0f, distPercentage) * 0.5);*/

					float distPercentage = (listenerPos2.DistBetween(pos) - ap.m_minDistance) / (ap.m_maxDistance - ap.m_minDistance);
					ch->set3DLevel(BEMath::Clamp<float>(0.0f, 1.0f, distPercentage));
				}
				else
				{
					ch->set3DLevel(0.0f);
				}

				//Start playing the sound
				ch->setPaused(false);

			}
			//If it is UI Element playing sound
			else if(ae.IsUI())
			{
				FMOD::Channel* ch;

				//Pla sound
				m_fmodsys->playSound(AssetMgr->GetAssetData<AudioClip>(ae.m_audioClip).GetSound(), nullptr, true, &ch);

				//Set channel properties
				ch->setMode(FMOD_2D | FMOD_LOOP_OFF);
				ch->setVolume(ap.m_volume * m_globalSoundVol);
				ch->setPitch(ap.m_pitch);
				ch->setPriority(ap.m_priority);

				//Update that this UI is using the channel
				int idx = 0;
				ch->getIndex(&idx);
				m_audioInPlay[idx] = ae;

				//Start playing the sound
				ch->setPaused(false);
			}
		}

		//Clear the audio queue
		m_audioQueue.clear();

		//Update fmod
		m_fmodsys->update();

		//Look through all the channels, if the channel isn't playing but the active audio sources is still non-null, then set as null 
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			FMOD::Channel* ch;
			m_fmodsys->getChannel(i, &ch);

			bool isPlaying = false;
			ch->isPlaying(&isPlaying);

			//If channel is no longer playing then set that no one is playing audio source in that channel
			if (!isPlaying && (m_audioInPlay[i].IsEntity() || m_audioInPlay[i].IsUI()))
				m_audioInPlay[i].Reset();
			//If channel is still playing, then update the position of the audio source (if it's 3D)
			else if(isPlaying && m_audioInPlay[i].IsEntity())
			{
				//If audio source is 3D
				if (AssetMgr->GetAssetData<AudioClip>(m_audioInPlay[i].m_audioClip).GetIs3D())
				{
					//Set position of the audio source
					const Vec3f& pos = ECS->GetComponent<Transform>(m_audioInPlay[i].m_entity_or_ui_ID).GetPosition();
					FMOD_VECTOR p = { pos.x, pos.y, pos.z };
					ch->set3DAttributes(&p, nullptr);

					//Get the position of the listener
					FMOD_VECTOR listenerPos = { 0.0f, 0.0f, 0.0f };
					m_fmodsys->get3DListenerAttributes(0, &listenerPos, nullptr, nullptr, nullptr);
					Vec3f listenerPos2{ listenerPos.x, listenerPos.y, listenerPos.z };
					
					//Get the min max distance of the audio sound
					float minDist = 1.0f, maxDist = 10000.0f;
					ch->get3DMinMaxDistance(&minDist, &maxDist);

					//Determine the new distance blending
					//float distPercentage = 1.0f - (listenerPos2.DistBetween(pos) - minDist) / (maxDist - minDist);
					//ch->set3DLevel(1.0f - BEMath::Clamp<float>(0.0f, 1.0f, distPercentage) * 0.5f);

					float distPercentage = (listenerPos2.DistBetween(pos) - minDist) / (maxDist - minDist);
					ch->set3DLevel(BEMath::Clamp<float>(0.0f, 1.0f, distPercentage));
				}
			}
		}
	}

	void AudioManager::Reload()
	{
		StopAllChannels();
		m_audioQueue.clear();
	}

	void AudioManager::Shutdown()
	{
		StopAllChannels();
		m_audioQueue.clear();
	}

	void AudioManager::OnEntityDestroyed(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::entityDestroy, "Wrong event, expected EntityDestroyedEvent, \
			passed in %s instead.", e.GetEventName());

		EntityDestroyedEvent& ed = dynamic_cast<EntityDestroyedEvent&>(e);
		EntityID audioSrc = ed.GetDestroyedEntity();

		//If audioSrc does not have audio source component, then dont need do anything
		if (!ECS->HasComponent<AudioSource>(audioSrc))
			return;

		//Go through all the audio sources being played and stop the channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			if (audioSrc == m_audioInPlay[i].m_entity_or_ui_ID)
			{
				FMOD::Channel* ch;
				m_fmodsys->getChannel(i, &ch);
				ch->stop();

				//also remove the entity from the active audio sources
				m_audioInPlay[i].Reset();
			}
		}

		//Also remove from audio event queue
		for (size_t i = 0; i < m_audioQueue.size(); ++i)
		{
			if (m_audioQueue[i].first.m_entity_or_ui_ID == audioSrc)
			{
				m_audioQueue.erase(m_audioQueue.begin() + i);
				--i;
			}
		}
	}

	void AudioManager::OnPause(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::pause, "Wrong event, expected GamePauseEvent, \
			passed in %s instead.", e.GetEventName());

		GamePauseEvent& gpe = dynamic_cast<GamePauseEvent&>(e);

		//Pause all entity's sound
		//Go through all the audio sources being played and stop the channel
		for (int i = 0; i < m_maxChannelsCount; ++i)
		{
			if (m_audioInPlay[i].IsEntity())
			{
				const AudioSource& as = ECS->GetComponent<AudioSource>(m_audioInPlay[i].m_entity_or_ui_ID);

				if (!as.GetIsBGM(m_audioInPlay[i].m_audioClip))
				{
					FMOD::Channel* ch;
					m_fmodsys->getChannel(i, &ch);
					ch->setPaused(gpe.GetIsPaused());
				}			
			}
		}
	}
}