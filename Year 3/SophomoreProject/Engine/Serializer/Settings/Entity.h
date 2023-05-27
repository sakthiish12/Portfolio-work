// /******************************************************************************/
// /*!
// \file         Entity.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Handling the entity (used for testing Not fully functional)
// \lines        23 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <string>
#include <utility>
//#include "IComponent.h"
#include "../SerializeManager.h"
#include"../../ECS/componentArray.h"

struct EntityInfo : public ISerializable//, componentVector
{
	//std::vector<entityID>;
	//std::string name;
	std::vector<std::string> tags;
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override
	{
		SerializeManager serializeManager;
		WriterWrapper<std::vector, std::string> containerWriter;

		writer->StartObject();

		writer->EndObject();
		return true;
	}

	bool OnDeserialize(const rapidjson::Value& obj) override
	{
		ReaderWrapper<std::vector, std::string> containerReader;

		//name = obj["name"].GetString();
		containerReader.ReadContainer(obj["tags"], tags);
		return true;
	}

};