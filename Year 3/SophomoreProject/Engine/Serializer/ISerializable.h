// /******************************************************************************/
// /*!
// \file         ISerializable.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        ISerializable
// \lines        24 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include "../../Dependencies/rapidjson/prettywriter.h"
#include "../../Dependencies/rapidjson/document.h"
#include <string>

class ISerializable
{
public:
	virtual ~ISerializable() = default;

	//overridden by derived components to serialize the data members into a json file
	virtual bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* _writer) const = 0;

	//override function by derived components to deserialize the data from json file back into the data members
	virtual bool OnDeserialize(const rapidjson::Value& _obj) = 0;
};

class componentSerializable : public ISerializable
{
public:
	//Add deserialized component data into ECS
	virtual void addComponent(uint32_t toEntity) = 0;
};

//temporary derived class to be used in the fnc DeserializeFromFile
class SerializeDerived final : public ISerializable
{
public:
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* _writer) const override;

	bool OnDeserialize(const rapidjson::Value& _obj) override;

	//called by SerializeManager's DeserializeFromFile if initDocument returns true
	//that is to say if the parsing of string is successful
	bool Deserialize(const std::string& _s, ISerializable& _component);

	//parses the string into the document, returns true if valid
	bool InitDocument(const std::string& _s, rapidjson::Document& _doc);
};