// /******************************************************************************/
// /*!
// \file         WindowSettings.cpp
// \project name Engine
// \author(s)    Sakthiish s/o Vijaydass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special function for WindowSettings
// \lines        23 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../pch.h"
#include"WindowSetting.h"
#include "../SerializeManager.h"

bool WindowSettings::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	SerializeManager serializeManager;

	writer->StartObject();

	//window height and width
	serializeManager.SerializeVector("m_windowSize", m_windowSize, writer);
	//window title
	serializeManager.SerializeString("m_windowTitle", m_windowTitle.c_str(), writer);
	//window configurations
	WriterWrapper<std::unordered_map, std::string, int> map;
	map.WriteContainer(writer, m_WindowConfigurations, "m_WindowConfigurations");

	writer->EndObject();
	return true;
}


bool WindowSettings::OnDeserialize(const rapidjson::Value& obj)
{
	SerializeManager serializeManager;
	serializeManager.DeserializeVector("m_windowSize", m_windowSize, obj);
	m_windowTitle = obj["m_windowTitle"].GetString();

	//initialize window configurations
	ReaderWrapper<std::unordered_map, std::string, int> map;
	map.ReadContainer(obj["m_WindowConfigurations"], m_WindowConfigurations);

	return true;
}