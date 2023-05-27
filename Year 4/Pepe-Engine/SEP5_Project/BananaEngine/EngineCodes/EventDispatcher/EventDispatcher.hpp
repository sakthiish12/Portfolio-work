#pragma once

namespace BE
{
	template <typename EventTypeClass>
	void EventDispatcher::CreateSubscription(EventFn _fn, int _priority)
	{
		//EventFn = std::function<void(IEvent&)>
		static_assert(std::is_base_of<IEvent, EventTypeClass>::value, "Static assert failed: EventTypeClass must inherit IEvent");

		//Index of array to be writing into
		size_t idx = (size_t)EventTypeClass::GetStaticType();

		//Add a new subscriber to the vector, indexed by event's enum value
		m_allCallbacks[idx].push_back(EventCallback(_fn, _priority));

		//Bring the callback forward until the correct position on priority. Priority sorted in descending order
		for (size_t i = m_allCallbacks[idx].size() - 1; i > 0; --i)
		{
			//If the callback before current index has lower priority, swap them
			if (m_allCallbacks[idx][i - 1].m_priority < m_allCallbacks[idx][i].m_priority)
				std::swap(m_allCallbacks[idx][i - 1], m_allCallbacks[idx][i]);
			//Otherwise, achieved sorted list
			else
				break;
		}
	}
	template <typename EventTypeClass>
	void EventDispatcher::Dispatch(EventTypeClass& _event)
	{
		static_assert(std::is_base_of<IEvent, EventTypeClass>::value, "Static assert failed: EventTypeClass must inherit IEvent");
		
		//Index of array to be reading from
		size_t idx = (size_t)EventTypeClass::GetStaticType();

		//Go through the vector calling the callback functions
		for (EventCallback& fn : m_allCallbacks[idx])
		{
			//Call the callback function
			fn.m_callbackFunc(_event);

			//If the event is marked as handled, stop propagating the event
			if (_event.IsHandled())
				break;
		}
	}
}