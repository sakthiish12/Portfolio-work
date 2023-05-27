// /******************************************************************************/
// /*!
// \file         ObserverMessgeg.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        30 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "ObserverMessage.h"

//Need to recheck if this actually get overriden or it will still run
IObserver::~IObserver()
{
	////might be buggy, please inform if any issue
	//for (auto& s : registeredSubjects)
	//{
	//	s->RemoveObserver(std::make_shared<IObserver>(this));
	//}
}

void IObserver::AddSubjects(std::shared_ptr<ISubject> subject)
{
	registeredSubjects.push_back(subject);
}

void IObserver::RemoveSubjects(std::shared_ptr<ISubject> subject)
{
	registeredSubjects.remove(subject);
}

void IObserver::AttachHandler(const MessageID& messageId, MESSAGE_HANDLER handler)
{
	m_handlers.insert(std::pair<MessageID, MESSAGE_HANDLER>(messageId, handler));
}

ISubject::ISubject() = default;
ISubject::~ISubject() = default;
//add something so that subject will update it via Notify()
void ISubject::AddObserver(std::shared_ptr<IObserver> observer)
{
	list_observer_.push_back(observer);
}

void ISubject::RemoveObserver(std::shared_ptr<IObserver> observer)
{
	list_observer_.remove(observer);
}

int ISubject::GetObserversSize()
{
	return (int)list_observer_.size();
}