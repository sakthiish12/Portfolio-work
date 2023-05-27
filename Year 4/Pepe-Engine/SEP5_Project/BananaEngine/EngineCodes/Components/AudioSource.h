#pragma once
#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "ECSComponent.h"

namespace BE
{
	enum class AudioFlag
	{
		isMute,
		isMusic,

		last
	};

	//AudioProperties constants
	#define MAX_AUDIO_PITCH 8.0f
	#define MIN_AUDIO_PITCH 0.125f
	#define DEF_AUDIO_PITCH 1.0f

	#define MAX_AUDIO_VOLUME 1.0f
	#define MIN_AUDIO_VOLUME 0.0f
	#define DEF_AUDIO_VOLUME 1.0f

	#define MAX_AUDIO_PRIORITY 0
	#define MIN_AUDIO_PRIORITY 256
	#define DEF_AUDIO_PRIORITY 128

	#define MIN_AUDIO_DISTANCE 0.0f
	#define MAX_AUDIO_DISTANCE 10000.0f	

	struct AudioProperties
	{
		//Pitch, Volume, priority
		float m_pitch;
		float m_volume;				//Volume played at minDistance
		int m_priority;				//Higher number = Higher priority, higher priority sound will play over lower priority

		//Properties
		std::bitset<(size_t)AudioFlag::last> m_flags;

		//3D sound settings
		float m_minDistance;        //At min distance, play at max volume
		float m_maxDistance;		//At max distance, play at min volume

		AudioProperties();
		AudioProperties(float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist);
	
		inline bool GetIsBGM() const		{ return m_flags.test((size_t)AudioFlag::isMusic); }
		inline bool GetIsMuted() const		{ return m_flags.test((size_t)AudioFlag::isMute); }
		inline void SetIsBGM  (bool _b)		{ m_flags.set((size_t)AudioFlag::isMusic, _b); }
		inline void SetIsMuted(bool _b)		{ m_flags.set((size_t)AudioFlag::isMute, _b); }
	};

	class AudioSource : public IECSComponent
	{
		//links the audio clip ID to it's info, like volume, pitch etc
		std::unordered_map<BEID, AudioProperties> m_allAudio{};

	public:
		AudioSource() = default;

		/*
			Asset Linkage
		*/

		void AddAudio(BEID _assetID, float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist);
		void AddAudio(BEID _assetID);
		void AddAudio(const std::string& _filePath, float _pitch, float _volume, int _priority, bool _isBGM, bool _isMuted, float _minDist, float _maxDist);
		void AddAudio(const std::string& _filePath);

		void RemoveAudio(BEID _assetID);
		void RemoveAudio(const std::string& _filePath);
		void RemoveAllAudio();

		inline bool HasAudioLinked() const							{ return !m_allAudio.empty(); }



		/*
			Volume, Pitch, and Priority
		*/
		//Get the volume of a specific audio clip linked to this source
		inline float GetVolume(BEID _assetID) const					{ return m_allAudio.at(_assetID).m_volume; }

		//Set the volume of a specific audio clip linked to this source, 
		//Put 0 on _assetID to change for all audioclips in this audio source
		void SetVolume(float _newVol, BEID _assetID/* = BEID(0)*/);

		//Get the pitch of a specific audio clip linked to this source
		inline float GetPitch(BEID _assetID) const					{ return m_allAudio.at(_assetID).m_pitch; }

		//Set the pitch of a specific audio clip linked to this source, 
		//Put 0 on _assetID to change for all audioclips in this audio source		
		void SetPitch(float _newPitch, BEID _assetID/* = BEID(0)*/);

		//Get the priority of this audiosource
		inline int GetPriority(BEID _assetID) const					{ return m_allAudio.at(_assetID).m_priority; }

		//Set the priority of a specific audio clip linked to this source,
		//Put 0 on _assetID to change for all audioclips in this audio source
		//can only be between 0 (highest) to 256 (lowest)
		void SetPriority(int _newPriority, BEID _assetID/* = BEID(0)*/);



		/*
			AudioProperties Properties
		*/

		//Get whether this audio is a BGM, which affects SFX and BGM volume sliders. True = BGM, False = SFX
		inline bool GetIsBGM(BEID _assetID) const					{ return m_allAudio.at(_assetID).GetIsBGM(); }

		//Set whether this audio should be played as BGM, affected by SFX and BGM volume sliders. True = BGM, False = SFX
		//Put 0 on _assetID to change for all audioclips in this audio source
		void SetIsBGM(bool _b, BEID _assetID /*= BEID(0)*/);

		//Get whether the audio is being muted. True = mute, False = not mute
		inline bool GetIsMuted(BEID _assetID) const					{ return m_allAudio.at(_assetID).GetIsMuted(); }

		//Set whether the audio is to be muted. True = mute, False = not mute
		//Put 0 on _assetID to change for all audioclips in this audio source
		void SetIsMuted(bool _isMute, BEID _assetID /*= BEID(0)*/);



		/*
			3D sound setting
		*/
		//Get minimum distance in which the volume is at the maximum
		inline float GetMinDist(BEID _assetID) const					{ return m_allAudio.at(_assetID).m_minDistance; }
		
		//Set minimum distance in which the volume is at the maximum.
		//Put 0 on _assetID to change for all audioclips in this audio source
		void SetMinDist(float _newDist, BEID _assetID/* = BEID(0)*/);

		//Get maximum distance in which the volume is at 0
		inline float GetMaxDist(BEID _assetID) const					{ return m_allAudio.at(_assetID).m_maxDistance; }
		
		//Set maximum distance in which the volume is at 0
		void SetMaxDist(float _newDist, BEID _assetID/* = BEID(0)*/);

		//Note that minDist <= maxDist



		/*
			Playback of sounds
		*/
		//Plays a given audio clip at the source's position
		void Play(BEID _assetID);

		//Pause/Unpaused this audio source, does nothing if audio is not played
		void Pause(BEID _assetID, bool _isPause);

		//Stop playing this audio source
		void Stop(BEID _assetID);

		//Check if audio source is being paused. Return false if audio is paused or not in use
		bool IsAudioPause(BEID _assetID) const;

		//Check if audio source is being played. Return false if audio is not played
		bool IsAudioPlaying(BEID _assetID) const;

		//Pause/Unpause all the audio linked to this audio source
		void PauseAllAudio(bool _isPause);

		//PauseStop all audio linked to this audio source
		void StopAllAudio();

			
		/*
			For serialisation only
		*/
		const AudioProperties& GetAudio(BEID _assetID) const		{ return m_allAudio.at(_assetID); }
		const auto& GetAllAudio() const								{ return m_allAudio; }

		
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	};
}
#endif