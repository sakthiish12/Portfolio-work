#pragma once
#ifndef	EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <Precompiled/pch.h>
#include <Utilities/Singleton/Singleton.h>
#include "Events.h"

namespace BE
{
	//Manages events within the engine
	class EventDispatcher : public Singleton<EventDispatcher>
	{
		using EventFn = std::function<void(IEvent&)>;

		//Keep track of the callback function and priority
		struct EventCallback
		{
			EventFn m_callbackFunc;	//the function to call
			int m_priority = 0;		//Higher priority callback are called first, higher number = higher prio

			//Constructor
			EventCallback(EventFn _callbackFn, int _prio);
		};

		//A container that contains a vector of callback functions to call. 
		//Index of std::array corresponds to EventType enum value
		std::array<std::vector<EventCallback>, (size_t)EventType::last> m_allCallbacks{};

		EventDispatcher() = default;
		~EventDispatcher() = default;

	public:


		//Subscribes an engine system to an event with a callback function to call when event happens
		//Callback function (_fn) signature must be --> void(IEvent&)
		//Please use the "Subscribe" and "SubscribeWPrio" macro unless you can write std::bind
		template <typename EventTypeClass>
		void CreateSubscription(EventFn _fn, int _priority);

		//For when going to dispatch a certain event
		template <typename EventTypeClass>
		void Dispatch(EventTypeClass& _event);

		//Clears all the subscriptions
		void ClearAllSubscriptions();

		friend class Singleton<EventDispatcher>;
	};

	//Subscribes an object class to an event with a callback member function to call when event happens
	//eg usage: Subscribe(QuitEvent, EngineSystem::GetInstance().get(), EngineSystem::SomeFunction)
#define Subscribe(eventType, eventListener, callbackFn)					CreateSubscription<eventType>(std::bind(&callbackFn, eventListener, std::placeholders::_1), 0)
	
	//Subscribes an object class to an event with a callback member function to call when event happens, with set priority
	//eg usage: SubscribeWPrio(QuitEvent, EngineSystem::GetInstance().get(), EngineSystem::SomeFunction, 5);
	//Higher priority will handle the event first. Higher number = higher prio
#define SubscribeWPrio(eventType, eventListener, callbackFn, priority)	CreateSubscription<eventType>(std::bind(&callbackFn, eventListener, std::placeholders::_1), priority)

	/*
		eventType: Event class (not the enum) that wants to be kept track of
		eventListener: Raw Pointer to ISystem that wants to keep track of an event
		callbackFn: The member function to call when the event happens, signature must be void(IEvent&)
		priority: Higher number means higher priority, higher priority gets to handle the event first
	*/

#define EventMgr EventDispatcher::GetInstance()
}

#include "EventDispatcher.hpp"

#endif
