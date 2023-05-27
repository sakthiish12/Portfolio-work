// /******************************************************************************/
// /*!
// \file         Message.h
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
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>

enum class MessageID
{
	/*
	* Message ID enum to identify
	*/
	MSG_SAMPLE = 0,
	MSG_COLLIDED,
	MSG_NONE
};

class IMessage
{
protected:
	MessageID _id = MessageID::MSG_NONE;
	std::string _message = "MESSAGE_NONE";
	/*
	* Inherited IMessage class can add more data as you need
	*/
public:
	IMessage();
	IMessage(MessageID id, std::string msg);
	virtual ~IMessage();
	inline MessageID GetID() const noexcept { return _id; }
	inline std::string GetMessage() const noexcept { return _message; }
	void SetMessage(std::string msg);
};

#endif