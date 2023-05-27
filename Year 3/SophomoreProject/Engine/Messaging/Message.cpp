// /******************************************************************************/
// /*!
// \file         Message.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        12 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Message.h"

IMessage::IMessage() = default;
IMessage::IMessage(MessageID id, std::string msg) : 
	_id(id), 
	_message(msg) 
{
}
IMessage::~IMessage() = default;

void IMessage::SetMessage(std::string msg) 
{ 
	_message = msg; 
}
