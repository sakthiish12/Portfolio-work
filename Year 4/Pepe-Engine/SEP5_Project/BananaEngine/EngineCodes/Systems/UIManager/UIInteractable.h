#pragma once
#include <Maths/MathLib.h>
#include <Systems/AudioManager/AudioEvent.h>
#include <Components/AudioSource.h>	//For the macros

namespace BE
{
	//UI Button textures type
	// For buttons and toggle
	enum class UIInteractStatus
	{
		normal = 0,
		hovered,
		pressed,
		disabled,

		uiInteractStatusLast
	};

	


	//Base class for all Interactable UI Elements, like button toggle and slider
	class UIInteractable
	{
	protected:
		//The texture IDs of the UI Button corresponding to the UIInteractStatus - For when UIElementFlags::isUsingTexture is false
		std::array<BEID, (size_t)UIInteractStatus::uiInteractStatusLast> m_textures;

		//The audio que IDs of the UI Button corresponding to the UIAudioQueType
		std::array<BEID, (size_t)(UIAudioQueType::uiAudioQueueLast)> m_audioQues;

		//Pitch, Volume, priority of the audio ques
		float m_pitch;
		float m_volume;
		int m_priority;				//Higher number = Higher priority, higher priority sound will play over lower priority

		//Current interaction status
		UIInteractStatus m_currentStatus;

		bool m_hasTexturesChanged{};
	public:
		UIInteractable();
		virtual ~UIInteractable();

		/*
			Getters for Textures and audio ques
		*/

		//Get the texture ID for the given interaction status
		inline BEID GetTexture(UIInteractStatus _interactStatus) const	{ return m_textures[(size_t)_interactStatus]; }

		//Get the audio for the given audio que type
		inline BEID GetAudioQue(UIAudioQueType _audioQueType) const		{ return m_audioQues[(size_t)_audioQueType]; }

		//Get the current texture to use - used by graphics side to determine the texture to use
		inline BEID GetTextureToUse() const								{ return m_textures[(size_t)m_currentStatus]; }

		//Current button interaction status
		inline UIInteractStatus GetCurrentInteractionStatus() const			{ return m_currentStatus; }

		//Get audio info
		inline float GetVolume() const										{ return m_volume; }
		inline float GetPitch() const										{ return m_pitch; }
		inline int   GetPriority() const									{ return m_priority; }
		inline bool GetHasTexturesChanged()					 { return m_hasTexturesChanged; };

		/*
			Setters for textures, and audio ques
		*/

		//Set the texture ID for the given interaction status, set _aID to 0 to remove texture
		void SetTexture(UIInteractStatus _interactStatus, BEID _aID);

		//Set the audio for the given audio que type, set _aID to 0 to remove audio que
		void SetAudioQue(UIAudioQueType _audioQueType, BEID _aID);

		//Set audio info
		inline void SetVolume(float _newVol)								{ m_volume = BEMath::Clamp(MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME, _newVol); }
		inline void SetPitch(float _newPitch)								{ m_pitch = BEMath::Clamp(MIN_AUDIO_PITCH, MAX_AUDIO_PITCH, _newPitch); }
		inline void SetPriority(int _newPriority)							{ m_priority = BEMath::Clamp(MAX_AUDIO_PRIORITY, MIN_AUDIO_PRIORITY, _newPriority); }
		inline void SetHasTexturesChanged(bool b)					{ m_hasTexturesChanged = b; };
		/*
			RTTR Stuff
		*/
		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND;
	};
}
