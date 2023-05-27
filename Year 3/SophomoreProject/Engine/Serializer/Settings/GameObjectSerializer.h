// /******************************************************************************/
// /*!
// \file         GameObjectSerializer.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        GameobjectSerializer changed to in component.cpp
// \lines        17 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <glm/vec2.hpp>
#include <unordered_map>
#include "../ISerializable.h"
#include "../../pch.h"
//#include"../../ECS/componentArray.h"

struct GameObjectSerializer : public ISerializable//, componentVectorBase
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;
	//void BuildAndSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer);
	//member variables
	/*glm::ivec2 m_windowSize;
	std::string Name;
	std::unordered_map<std::string, int> Value;*/

	//std::vector<entityID> ids;
	std::string NameArray;
	Mathf::Vector2 pos;
	Mathf::Vector2 vel;
	Mathf::Vector2 scal;
	Mathf::Vector2 acceleration;
	Mathf::Vector2 Force;
	double rot;
	//std::list<GameObjects> GameObjects;
	//std::vector<componentType> components;
};

