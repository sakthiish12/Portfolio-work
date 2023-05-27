// /******************************************************************************/
// /*!
// \file         Broadcast.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        21 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BROADCAST_H_
#define _BROADCAST_H_

#include <list>
#include <memory>
#include "Message.h"

class IBroadcastReceiver
{
	virtual ~IBroadcastReceiver() = default;
	virtual void HandleMessage(IMessage* msg) = 0;
};

class IBroadcastSender
{
protected:
	std::list<std::shared_ptr<IBroadcastReceiver>> _receiverList;
public:
	virtual ~IBroadcastSender() = default;
	virtual void BroadcastMessage(IMessage* msg) = 0;

	void AddToBroadcastList(std::shared_ptr<IBroadcastReceiver> receiver);
	void RemovefromBroadcastList(std::shared_ptr<IBroadcastReceiver> receiver);
};



#endif