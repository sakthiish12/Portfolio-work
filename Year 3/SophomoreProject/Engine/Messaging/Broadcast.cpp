// /******************************************************************************/
// /*!
// \file         Broadcast.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        9 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Broadcast.h"


void IBroadcastSender::AddToBroadcastList(std::shared_ptr<IBroadcastReceiver> receiver)
{
	_receiverList.push_back(receiver);
}

void IBroadcastSender::RemovefromBroadcastList(std::shared_ptr<IBroadcastReceiver> receiver)
{
	_receiverList.remove(receiver);
}
