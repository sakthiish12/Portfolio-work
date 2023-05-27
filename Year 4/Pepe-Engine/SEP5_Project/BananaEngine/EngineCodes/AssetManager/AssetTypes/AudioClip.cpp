#include "pch.h"
#include "AudioClip.h"
#include <Systems/AudioManager/AudioManager.h>
#include <Serialisation/BananaSerialisation.h>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<AudioClip>(BE::RegistrationNameCreator<AudioClip>())
	.constructor()()
	.method(assetConstructMethodName, &AudioClip::ConstructFromPtr)()
	.method(serializeAssetMethodName, &AudioClip::Serialise)();
	registration::class_<AudioClipInitialiser>(BE::RegistrationNameCreator<AudioClipInitialiser>())
	.constructor()()
	.property(filePathName, &AudioClipInitialiser::m_filePath)
	.property("m_mode", &AudioClipInitialiser::m_mode);
}
namespace BE
{
	AudioClip::AudioClip(const std::string& _filePath, FMOD_MODE _mode)
	{
		AudioClipInitialiser aci{ _filePath, _mode };

		try
		{
			this->Construct(aci);
		}
		catch (const std::runtime_error&)
		{
			throw;
		}
		

#if 0
		//Create all sound as non-looping 2D sound, though can be changed later
		FMOD_RESULT r;
		
		//If the given _mode is 0, then give the default mode for the audio clip
		if (_mode == 0)
		{
			r = AudioMgr->GetFMODSystem()->createSound(_filePath.c_str(),
				FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF,
				nullptr, &m_sound);
		}
		//Otherwise, use the given mode instead
		else
		{
			r = AudioMgr->GetFMODSystem()->createSound(_filePath.c_str(),
				_mode, nullptr, &m_sound);
		}
		
		//If loading failed
		if (r != FMOD_OK)
		{
			throw std::runtime_error("Failed to create FMOD Sound, Error Code: " + std::to_string(r));
		}
#endif
	}


	AudioClip::AudioClip(AudioClip&& _rhs) noexcept
	{
		m_sound = _rhs.m_sound;
		_rhs.m_sound = nullptr;
	}

	void AudioClip::Construct(const AudioClipInitialiser& _data)
	{

		//Create all sound as non-looping 2D sound, though can be changed later
		FMOD_RESULT r;

		//If the given _mode is 0, then give the default mode for the audio clip
		if (_data.m_mode == 0)
		{
			r = AudioMgr->GetFMODSystem()->createSound(_data.m_filePath.c_str(),
				FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF,
				nullptr, &m_sound);
		}
		//Otherwise, use the given mode instead
		else
		{
			r = AudioMgr->GetFMODSystem()->createSound(_data.m_filePath.c_str(),
				_data.m_mode, nullptr, &m_sound);
		}

		//If loading failed
		if (r != FMOD_OK)
		{
			throw std::runtime_error("Failed to create FMOD Sound, Error Code: " + std::to_string(r));
		}
	}

	void AudioClip::ConstructFromPtr(AudioClipInitialiser* _data)
	{
		Construct(*_data);
	}

	AudioClipInitialiser AudioClip::Serialise()
	{
		AudioClipInitialiser temp;
		m_sound->getMode(&temp.m_mode);
		return temp;
	};

	AudioClip::~AudioClip()
	{
		m_sound->release();
	}

	AudioClip& AudioClip::operator=(AudioClip&& _rhs) noexcept
	{
		//Free m_sound here
		m_sound->release();

		m_sound = _rhs.m_sound;
		_rhs.m_sound = nullptr;

		return *this;
	}

	//Is the sound loaded as 3D? True = 3D, False = 2D
	bool AudioClip::GetIs3D() const
	{
		FMOD_MODE m;
		m_sound->getMode(&m);
		return m & FMOD_3D;
	}

	//Set the sound to be 3D (true) or 2D (false)
	void AudioClip::SetIs3D(bool _b)
	{
		(_b) ? m_sound->setMode(FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF) : m_sound->setMode(FMOD_2D);
	}

	//Is the sound loaded to be looping? True = looping, false = non-looping
	bool AudioClip::GetIsLooping() const
	{
		FMOD_MODE m;
		m_sound->getMode(&m);
		return m & FMOD_LOOP_NORMAL;
	}

	//Set the sound to be looping (true) or non-looping (false)
	void AudioClip::SetIsLooping(bool _b)
	{
		(_b) ? m_sound->setMode(FMOD_LOOP_NORMAL) : m_sound->setMode(FMOD_LOOP_OFF);
	}

	//Get all the modes as an unsigned integer
	FMOD_MODE AudioClip::GetAudioMode() const
	{
		FMOD_MODE m;
		m_sound->getMode(&m);
		return m;
	}

	void AudioClip::SetAudioMode(FMOD_MODE _mode)
	{
		m_sound->setMode(_mode);
	}

	AudioClipInitialiser::AudioClipInitialiser(const std::string filePath, FMOD_MODE mode)
		:m_filePath{filePath}, m_mode{mode}
	{

	}
}
