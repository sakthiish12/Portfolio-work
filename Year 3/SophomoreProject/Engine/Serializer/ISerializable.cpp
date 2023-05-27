// /******************************************************************************/
// /*!
// \file         ISerializable.cpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        ISerializable 
// \lines        33 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../pch.h"
#include "ISerializable.h"

bool SerializeDerived::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* _writer) const
{
	(void)_writer;
	return true;
}

bool SerializeDerived::OnDeserialize(const rapidjson::Value& _obj)
{
	(void)_obj;
	return true;
}

bool SerializeDerived::Deserialize(const std::string& _s, ISerializable& _component)
{
	rapidjson::Document doc;
	if (!InitDocument(_s, doc))
	{
		PartyAssert(false, "Json file format is incorrect!");
		//std::cout << "Json file format is incorrect!\n";
		return false;
	}

	_component.OnDeserialize(doc);

	return true;
}

bool SerializeDerived::InitDocument(const std::string& _s, rapidjson::Document& _doc)
{
	if (_s.empty())
	{
		return false;
	}

	std::string validJson(_s);

	return !_doc.Parse(validJson.c_str()).HasParseError() ? true : false;
}
