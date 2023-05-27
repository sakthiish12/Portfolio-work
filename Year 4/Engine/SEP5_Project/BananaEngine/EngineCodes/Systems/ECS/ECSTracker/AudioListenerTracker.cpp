#include "pch.h"
#include "AudioListenerTracker.h"
#include "../ECS.h"

namespace BE
{
	AudioListenerTracker::AudioListenerTracker() 
	{
		m_trackedEntities.reserve(2);
	}

	void AudioListenerTracker::AdditionalUpdateOnAddition(EntityID _eid)
	{
		//There can only be 1 audio listener
		if (m_trackedEntities.size() > 1)
		{
			LogCoreWarning("There can only be 1 Audio Listener per scene, the previous entity %llu \
				no longer has AudioListener component", *m_trackedEntities.front());

			ECS->RemoveComponent<AudioListener>(m_trackedEntities.front());
		}
	}

	void AudioListenerTracker::AdditionalUpdateOnRemoval(EntityID _eid)
	{
		UNUSED(_eid);
	}
}