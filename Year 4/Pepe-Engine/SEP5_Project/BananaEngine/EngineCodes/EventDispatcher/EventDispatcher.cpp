#include "pch.h"
#include "EventDispatcher.h"

namespace BE
{
	EventDispatcher::EventCallback::EventCallback(typename EventDispatcher::EventFn _callbackFn, int _prio)
		: m_callbackFunc(_callbackFn), m_priority(_prio)
	{

	}

	void EventDispatcher::ClearAllSubscriptions()
	{
		for (auto& callbacks : m_allCallbacks)
			callbacks.clear();
	}
}