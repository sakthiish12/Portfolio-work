#pragma once
#ifndef AUDIOLISTENER_H
#define AUDIOLISTENER_H

#include "ECSComponent.h"

namespace BE
{
	class AudioListener : public IECSComponent
	{
		//No parameters.
		
		//Requires audio source to work.
		//Attach to any object to act as "Ears"
		//When receiving sound from audio source, will play on your computer's speakers
	public:
		AudioListener() = default;
		~AudioListener() = default;

		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	};
}
#endif