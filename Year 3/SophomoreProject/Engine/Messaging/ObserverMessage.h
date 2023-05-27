// /******************************************************************************/
// /*!
// \file         ObserverMessgeg.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        45 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _OBSERVER_MESSAGE_H_
#define _OBSERVER_MESSAGE_H_

#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <map>
#include "Message.h"

/*
* Class in this file:
* IObserver
* ISubject
* 
* Fn Callback:
* MESSAGE_HANDLER
*/

/*
* Steps to using:
* 1) Add to enum class MessageID in Message.h
* 2) Create your own message public inheriting IMessage, define pure fn if any
* 3) Add public inheritance IObserver to your observer (the one receiving), define pure fn if any
*	3a) Define HandleMessage if your observer is going to receive the message
* 4) Add public inheritance ISubject to your observerable (the one sending) , define pure fn if any
*	4a) Define ProcessMessage if your subject is going to call the handler (fn call back) in observer
* 5) Create a MESSAGE_HANDLER function call back to handle the message 
* 
* Send your message via 3a or automatically via 4a
* 
* NOTE:
* Observer to unregister any registered in the destructor
*/

//MESSAGE_HANDLER is a function pointer that is meant to be called by subject to process message on observer behalf
typedef void(*MESSAGE_HANDLER)(IMessage*);

class ISubject;
// listener / subscriber / receiver etc
class IObserver
{
protected:
	std::map<MessageID, MESSAGE_HANDLER> m_handlers; // depending on the message id, use this callback function
	std::string m_id; //maybe the observer "name"
	std::list<std::shared_ptr<ISubject>> registeredSubjects;
public:
	//In each derived class, have to removeobserver in subjects.
	virtual ~IObserver();

	void AddSubjects(std::shared_ptr<ISubject> subject);

	void RemoveSubjects(std::shared_ptr<ISubject> subject);

	//this is for without callback fn
	virtual void HandleMessage(std::shared_ptr<IMessage> message) = 0;

	void AttachHandler(const MessageID& messageId, MESSAGE_HANDLER handler);

	inline MESSAGE_HANDLER GetHandler(const MessageID& messageId) noexcept
	{
		return m_handlers[messageId];
	}

	inline std::string GetID() const noexcept
	{
		return m_id;
	}
};

//subject / publisher / sender
class ISubject
{
protected:
	std::list<std::shared_ptr<IObserver>> list_observer_; //list of things listening to this class
public:
	ISubject();
	virtual ~ISubject();
	//add something so that subject will update it via Notify()
	void AddObserver(std::shared_ptr<IObserver> observer);

	void RemoveObserver(std::shared_ptr<IObserver> observer);

	int GetObserversSize();

	//Will loop through list_observer_, then get its handler to fn callback on the message
	virtual void ProcessMessage(IMessage* message) = 0;

	//Will loop through list_observer_ send message without using fn callback
	virtual void Notify() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///*
//* SAMPLE
//* Inherited message
//*/
//class CollideMessage : public IMessage
//{
//public:
//	int _objectID_1 = 0;
//	int _objectID_2 = 0;
//	CollideMessage() = default;
//	CollideMessage(int id1, int id2) : IMessage(MessageID::MSG_COLLIDED, "Collided"), _objectID_1(id1), _objectID_2(id2) {}
//
//	//user can define or add more stuff, to be handled in the callback
//	int GetData1() { return _objectID_1; }
//	int GetData2() { return _objectID_2; }
//};
//
///*
//* SAMPLE
//* Function callback for processing message
//*/
//void PrintCollidedObjectsID(CollideMessage* message)
//{
//	std::cout << "Processing CollideMessage object with _objectID_1 = " << message->GetData1() << " and _objectID_2 = " << message->GetData2() << std::endl;
//};
//
////Sample usage
///*
//* Example of subjects: Objects that sends out message, system that sends out message
//*/
//class SampleSubject : public ISubject {
//public:
//	SampleSubject() = default;
//	virtual ~SampleSubject() = default;
//
//	/**
//	 * Normal sending of message from subjects without using fn callback
//	 */
//	void Notify() override {
//		std::list<IObserver*>::iterator iterator = list_observer_.begin();
//		while (iterator != list_observer_.end()) {
//			(*iterator)->HandleMessage(static_cast<std::shared_ptr<IMessage>>(&_message));
//			++iterator;
//		}
//	}
//
//	void ProcessMessage(IMessage* message)
//	{
//		std::list<IObserver*>::iterator iterator = list_observer_.begin();
//		while (iterator != list_observer_.end()) {
//			(*iterator)->GetHandler(message->GetID())(message);
//			++iterator;
//		}
//	}
//
//	void CreateMessage(CollideMessage msg)
//	{
//		//not using anymore
//		_message = msg;
//	}
//private:
//	CollideMessage _message{};
//};
//
////SampleObserver
///*
//* Example of Observer: Systems that receives updates
//*/
//class SampleObserverOne : public IObserver {
//public:
//	SampleObserverOne(std::shared_ptr<SampleSubject> subject) : subject_(subject)
//	{
//		this->subject_->AddObserver(this);
//		std::cout << "Hi, I'm the Observer \"" << ++SampleObserverOne::static_number_ << "\".\n";
//		this->number_ = SampleObserverOne::static_number_;
//	}
//
//	virtual ~SampleObserverOne()
//	{
//		std::cout << "Goodbye, I was the Observer \"" << this->number_ << "\".\n";
//	}
//
//	void HandleMessage(std::shared_ptr<IMessage> message) override
//	{
//		message_from_subject_ = message->GetMessage();
//		PrintInfo();
//	}
//
//	void RemoveMeFromTheList()
//	{
//		subject_->RemoveObserver(this);
//		std::cout << "Observer \"" << number_ << "\" removed from the list.\n";
//	}
//
//	void PrintInfo()
//	{
//		std::cout << "Observer \"" << this->number_ << "\": a new message is available --> " << this->message_from_subject_ << "\n";
//	}
//
//private:
//	std::string message_from_subject_; // could be processed immediately or modify other variable
//	std::shared_ptr<SampleSubject> subject_; // could be in container
//	static int static_number_;
//	int number_;
//};
//
//int SampleObserverOne::static_number_ = 0;
//
//void ClientCode() {
//	SampleSubject* subject = new SampleSubject;
//	SampleObserverOne* observer1 = new SampleObserverOne(std::shared_ptr<SampleSubject>(subject));
//	SampleObserverOne* observer2 = new SampleObserverOne(std::shared_ptr<SampleSubject>(subject));
//	SampleObserverOne* observer3 = new SampleObserverOne(std::shared_ptr<SampleSubject>(subject));
//	SampleObserverOne* observer4;
//	SampleObserverOne* observer5;
//	CollideMessage sampleMessage(1, 5);
//
//	subject->CreateMessage(sampleMessage);
//	observer3->RemoveMeFromTheList();
//
//	subject->CreateMessage(sampleMessage);
//	observer4 = new SampleObserverOne(std::shared_ptr<SampleSubject>(subject));
//
//	observer2->RemoveMeFromTheList();
//	observer5 = new SampleObserverOne(std::shared_ptr<SampleSubject>(subject));
//
//	subject->CreateMessage(sampleMessage);
//	observer5->RemoveMeFromTheList();
//
//	observer4->RemoveMeFromTheList();
//	observer1->RemoveMeFromTheList();
//
//	delete observer5;
//	delete observer4;
//	delete observer3;
//	delete observer2;
//	delete observer1;
//	delete subject;
//}
//
//int TestObserver() {
//	ClientCode();
//	return 0;
//}

namespace Messaging  //UNFINISHED! - TODO - "Disconnect",...
{
	template<typename... T_ARGS> class Event;

	template<typename... T_ARGS>
	class AbstractDelegate //acts like a delegate node in a linked list of delegate nodes
	{
	protected:
		using EventType = Event<T_ARGS...>;

		constexpr AbstractDelegate() noexcept = default;
		__forceinline constexpr AbstractDelegate(EventType& eve) noexcept : m_event(&eve) {}

		__forceinline void Attach(EventType& eve) noexcept { if (m_event != nullptr) m_event = &eve; }
		__forceinline void Detach(EventType& eve) noexcept { if ((m_event != nullptr) && (m_event == &eve)) m_event = nullptr; m_next = m_previous = nullptr; }
		virtual void Call(T_ARGS... args) const noexcept = 0;

		//data
		EventType* m_event{ nullptr };
		AbstractDelegate<T_ARGS...>* m_next{ nullptr };
		AbstractDelegate<T_ARGS...>* m_previous{ nullptr };


		template<typename...> friend class Event;
	};



	template<typename T_CLASS, typename... T_ARGS>
	class Delegate : public AbstractDelegate<T_ARGS...>//concrete delegate
	{
	public:
		using Callback = void(T_CLASS::*)(T_ARGS...);//member class function pointer
		using EventType = Event<T_ARGS...>;
		using Parent = AbstractDelegate<T_ARGS...>;


		__forceinline constexpr Delegate(Callback fct) noexcept : m_function(fct), m_class(nullptr) {};
		__forceinline constexpr Delegate(T_CLASS& cls, Callback fct) noexcept : m_class(&cls), m_function(fct) {}
		Delegate(const Delegate&) = delete;
		void operator = (const Delegate&) = delete;

		virtual void Call(T_ARGS... args) const noexcept override final { (m_class->*m_function)(args...); }

		__forceinline void Connect(EventType& e) noexcept { e.Append(*this); this->Attach(e); }

	protected:
		T_CLASS* m_class{ nullptr };
		Callback m_function{ nullptr };
	};






	template<typename... T_ARGS>
	class Event
	{
	public:
		using AbstractDelegateClass = AbstractDelegate<T_ARGS...>;

		Event() noexcept = default;
		__forceinline ~Event() noexcept {}
		__forceinline void Notify(T_ARGS... args) noexcept
		{
			for (auto pdelegate = m_head; pdelegate; pdelegate = pdelegate->m_next)
				pdelegate->Call(std::forward<T_ARGS>(args)...);
		}


	protected:
		__forceinline void Append(AbstractDelegateClass& d) noexcept
		{
			d.m_next = m_head;
			if (m_head)
				m_head->m_previous = &d;
			m_head = &d;
		}

		__forceinline void Connect(AbstractDelegateClass& d) noexcept { Append(d); d.Attach(*this); }

	private:
		Event(const Event&) = delete;
		void operator = (const Event&) = delete;

	private:
		AbstractDelegateClass* m_head{ nullptr };

	protected:
		template< typename, typename... > friend class Delegate;
	};
}

#endif
