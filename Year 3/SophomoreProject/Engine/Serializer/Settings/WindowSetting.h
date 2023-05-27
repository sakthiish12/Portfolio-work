// /******************************************************************************/
// /*!
// \file         WindowSettings.h
// \project name Engine
// \author(s)    Sakthiish s/o Vijaydass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special function for WindowSettings
// \lines        12 Lines
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

struct WindowSettings : public ISerializable
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;

	//member variables
	glm::ivec2 m_windowSize;
	std::string m_windowTitle;
	std::unordered_map<std::string, int> m_WindowConfigurations;
};
