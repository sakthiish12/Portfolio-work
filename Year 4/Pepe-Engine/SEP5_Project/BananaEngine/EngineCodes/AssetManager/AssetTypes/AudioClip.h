#pragma once
#ifndef	AUDIOCLIP_H
#define AUDIOCLIP_H

#include "IAsset.h"

namespace BE
{
	struct AudioClipInitialiser
	{
		std::string m_filePath{};
		FMOD_MODE m_mode{};

		AudioClipInitialiser() = default;
		AudioClipInitialiser(const AudioClipInitialiser& in) = default;
		AudioClipInitialiser(const std::string filePath, FMOD_MODE mode);
		RTTR_ENABLE()
	};

	class AudioClip : public IAsset
	{
		FMOD::Sound* m_sound = nullptr;

	public:
		//Creates FMOD Sound using the given file name, loads with the given _mode. Put _mode as 0 for default mode
		AudioClip() = default;
		AudioClip(const std::string& _filePath, FMOD_MODE _mode);
		AudioClip(AudioClip&& _rhs) noexcept;
		~AudioClip();
		AudioClip& operator=(AudioClip&& _rhs) noexcept;

		/*
			Sound properties
		*/

		//Is the sound loaded as 3D? True = 3D, False = 2D
		bool GetIs3D() const;
		void SetIs3D(bool _b);

		//Is the sound loaded to be looping? True = looping, false = non-looping
		bool GetIsLooping() const;
		void SetIsLooping(bool _b);

		//Get all the modes as an unsigned integer
		FMOD_MODE GetAudioMode() const;
		void SetAudioMode(FMOD_MODE _mode);

		//Construct the data for non-templated load asset
		void Construct(const AudioClipInitialiser& _data);
		void ConstructFromPtr(AudioClipInitialiser* _data);
		AudioClipInitialiser Serialise();
		inline FMOD::Sound* const& GetSound() const			{ return m_sound; }
		RTTR_ENABLE(IAsset)
	};
}
#endif