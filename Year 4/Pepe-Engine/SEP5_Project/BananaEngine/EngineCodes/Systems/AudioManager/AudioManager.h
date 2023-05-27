#pragma once
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "../ISystem.h"
#include "../ECS/ECS.h"
#include "../UIManager/UIManager.h"
#include "AudioEvent.h"

namespace BE
{

	class AudioManager : public Singleton<AudioManager>, public ISystem
	{
		//The fmod system
		FMOD::System* m_fmodsys;

		//AudioProperties Manager properties
		const int m_maxChannelsCount;
		float m_globalMusicVol;
		float m_globalSoundVol;
		float m_globalMusicPitch{1.0f};
		float m_globalSoundPitch{1.0f};
		
		//Keep track of entities that are currently playing sound
		std::vector<AudioEvent> m_audioInPlay;
		
		//A queue of entity entities that are going to play sound
		std::vector<std::pair<AudioEvent, AudioProperties>> m_audioQueue;

		AudioManager(int _numChannels, float _musicVol, float _soundVol);
		~AudioManager();

	public:
		//Gets the FMOD System
		inline FMOD::System* const GetFMODSystem() const	{ return m_fmodsys; }

		/*
			Get audio manager properties
		*/
		//Get the global music and sound volume/pitch
		inline float GetGlobalMusicVol() const				{ return m_globalMusicVol; }
		inline float GetGlobalSoundVol() const				{ return m_globalSoundVol; }
		inline float GetGlobalMusicPitch() const			{ return m_globalMusicPitch; }
		inline float GetGlobalSoundPitch() const			{ return m_globalSoundPitch; }

		//Set the global music and sound volume/pitch
		void SetGlobalMusicVol(float _newVol);
		void SetGlobalSoundVol(float _newVol);
		void SetGlobalMusicPitch(float _newPitch);
		void SetGlobalSoundPitch(float _newPitch);
		
		//Get the number of channels created/used, and whether all are used
		int GetUsedChannelsCount() const;
		inline int GetMaxChannelCount() const				{ return m_maxChannelsCount; }
		inline bool	IsAllChannelsUsed() const				{ return GetUsedChannelsCount() == m_maxChannelsCount; }

	//private:
		/*
			Playing of sound, given the audio source
		*/
		//Play an audio given the audio source
		void PlayAudio(const AudioEvent& _ae, const AudioProperties& _prop);

		//Pause the audio given the audio source
		void PauseAudio(const AudioEvent& _ae, bool _isPause);

		//Stop playing the audio source
		void StopAudio(const AudioEvent& _ae);

		//Check if audio source is being paused. Return false if audio is paused or not in use
		bool IsAudioPause(const AudioEvent& _ae) const;

		//Check if audio source is being played. Return false if audio is not played
        //_index returns the index in m_channelList that contains _ae, if found
		bool IsAudioPlaying(const AudioEvent& _ae) const;
		


        /*
            Modifying audio sources
        */
       //Changes volume of a single audio source
       void SetVolume(const AudioEvent& _ae, float _newVol);

       //Changes Pitch of a single audio source
       void SetPitch(const AudioEvent& _ae, float _newPitch);

       //Changes priority of a single audio source
       void SetPriority(const AudioEvent& _ae, int _newPriority);

       //Mutes an audio source
       void SetMute(const AudioEvent& _ae, bool _isMute);

       //Set the distance of the rolloff
       void Set3DRolloffDistance(const AudioEvent& _ae, float _newMinDist, float _newMaxDist);

	public:
		/*
			Additional control functions
		*/
		//Pause/Unpause all the channels - entities marked as "update when paused" (EntityInfo) will continue to play sound.
		//To forcefully pause all channels, set _forceUpdate to true
		void PauseAllChannels(bool _isPause, bool _forceUpdate = false);

		//Stop playing all the channels
		void StopAllChannels();

		//Remove an audio event from getting played
		void RemoveFromQueue(const AudioEvent& _ae);

		//Functions to overide
		ISYSTEM_FUNCTIONS;



		/*
			Event handling
		*/
		//To stop play audio for entity that is going to be destroyed
		void OnEntityDestroyed(IEvent& e);

		//Pause audio when game is paused
		void OnPause(IEvent& e);

		friend class Singleton<AudioManager>;
		friend class AudioSource;
		friend class UIButton;
		friend class UIToggle;
		friend class UISlider;
	};

#define AudioMgr AudioManager::GetInstance()
}

#endif