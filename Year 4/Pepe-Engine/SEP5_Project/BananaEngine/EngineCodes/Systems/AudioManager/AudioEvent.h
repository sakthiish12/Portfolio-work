#pragma once
#ifndef AUDIOEVENT_H
#define AUDIOEVENT_H

#include <Utilities/BEID/BEID.h>
#include <Components/AudioSource.h>

namespace BE
{
	//AudioProperties que for interactable elements
	// For button, slider, toggle
	enum class UIAudioQueType
	{
		pressed = 0,		//The sound to play when pressed the button
		hovered,			//The sound to play when hovered over the button

		uiAudioQueueLast
	};

	struct AudioEvent
	{
		//Entity/UI that is going to play sound
		BEID m_entity_or_ui_ID{ 0 };		
		BEID m_canvasID{ 0 };

		/*
			If canvasID is 0, then this audio event is assumed to be for entity
			If canvasID is non-0, then this audio event is assumed to be for canvas
		*/

		UIAudioQueType m_que{ UIAudioQueType::pressed };

		//Audioclip that is going to be updated
		BEID m_audioClip{ 0 };



		/*
			Functions
		*/

		//Do not use this
		AudioEvent() = default;

		//For Entity
		AudioEvent(BEID _entityID, BEID _assetID) 
			: m_entity_or_ui_ID(_entityID), m_audioClip(_assetID) {}
		
		//For UI
		AudioEvent(BEID _canvasID, BEID _uiID, BEID _assetID, UIAudioQueType _que)
			: m_entity_or_ui_ID(_uiID), m_canvasID(_canvasID), m_que(_que), m_audioClip(_assetID) {}
		
		inline void Reset()											{ *this = AudioEvent{}; }
		inline bool IsEmpty() const									{ return m_entity_or_ui_ID == BEID{ 0 } && m_canvasID == BEID{ 0 }; }
		inline bool IsEntity() const								{ return m_entity_or_ui_ID != BEID{ 0 } && m_canvasID == BEID{0}; }
		inline bool IsUI() const									{ return m_entity_or_ui_ID != BEID{ 0 } && m_canvasID != BEID{0}; }
		inline bool IsValid() const									{ return IsEntity() || (IsUI() && m_entity_or_ui_ID); }
		inline bool SameObj(BEID _id, BEID _canvas = BEID(0)) const	{ return _id == m_entity_or_ui_ID && _canvas == m_canvasID; }
		inline bool SameAudio(BEID _audioClip) const				{ return _audioClip == m_audioClip; }
	};

}

#endif
