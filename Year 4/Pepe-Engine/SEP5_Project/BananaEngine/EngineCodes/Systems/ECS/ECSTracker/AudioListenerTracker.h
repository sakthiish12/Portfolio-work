#pragma once
#ifndef	AUDIO_LISTENER_TRACKER
#define AUDIO_LISTENER_TRACKER

#include "ECSTracker.h"

namespace BE
{
	class AudioListenerTracker : public IECSTracker
	{
	public:
		AudioListenerTracker();
		~AudioListenerTracker() = default;

		void AdditionalUpdateOnAddition(EntityID _eid) override;
		void AdditionalUpdateOnRemoval(EntityID _eid)  override;

	};
}

#endif