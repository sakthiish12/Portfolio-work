// /******************************************************************************/
// /*!
// \file         GraphicsSettings.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special Functions for Graphics Settings
// \lines        93 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//#include "pch.h"
#include <sstream>
#include "GraphicsSettings.h"
#include "../SerializeManager.h"
#include "../../Dependencies/rapidjson/reader.h"

using namespace rapidjson;
using namespace std;

bool GraphicsSettings::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	WriterWrapper<std::unordered_map, std::string, ShaderPathData> wrapper;
	WriterWrapper<std::unordered_map, std::string, uniformContainer> wrapper2;
	writer->StartObject();

	wrapper.WriteContainer(writer, m_InitShaders, "m_InitShaders");

	wrapper2.WriteContainer(writer, m_FrameBuffers, "m_FrameBuffers");

	writer->EndObject();
	return true;
}

bool GraphicsSettings::OnDeserialize(const rapidjson::Value& obj)
{
	/*ReaderWrapper<std::unordered_map, std::string, ShaderPathData> wrapper;
	ReaderWrapper<std::unordered_map, std::string, uniformContainer> wrapper2;*/

	//wrapper.ReadContainer(obj["m_InitShaders"], m_InitShaders);
	//wrapper2.ReadContainer(obj["m_FrameBuffers"], m_FrameBuffers);
	static_cast<void>(obj);//unreferenced 
	return true;
}

bool GraphicsSettings::ShaderPathData::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();
	writer->String("m_VertexPath");
	writer->String(m_VertexPath.c_str());
	writer->String("m_FragmentPath");
	writer->String(m_FragmentPath.c_str());
	writer->String("m_GeometryPath");
	writer->String(m_GeometryPath.c_str());
	writer->EndObject();

	return true;
}

bool GraphicsSettings::ShaderPathData::OnDeserialize(const rapidjson::Value& obj)
{
	m_VertexPath = obj["m_VertexPath"].GetString();
	m_FragmentPath = obj["m_FragmentPath"].GetString();
	m_GeometryPath = obj["m_GeometryPath"].GetString();

	return true;
}

GraphicsSettings::ShaderPathData::ShaderPathData(const std::string& _VertexPath, const std::string& _FragmentPath, const std::string& _GeometryPath) :
	m_VertexPath{ _VertexPath },
	m_FragmentPath{ _FragmentPath },
	m_GeometryPath{ _GeometryPath }
{}

bool Mesh::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	static_cast<void>(writer);//unreferenced 
	return false;
}

bool Mesh::OnDeserialize(const rapidjson::Value& obj)
{
	//SerializeManager serializeManager;
	
	for (auto& val : obj["Square_m_meshes"].GetArray())
	{
		const rapidjson::Value& value = val["Value"];
		std::stringstream ss;
		for (auto& valv : value["Square_m_vertices"].GetArray())
		{
			Square_m_vertices.push_back(valv.GetString());
		}
		for (auto& vali : value["Square_m_indices"].GetArray())
		{
			Square_m_indices.push_back(vali.GetString());
		}
	}

	for (auto& val : obj["Triangle_m_meshes"].GetArray())
	{
		const rapidjson::Value& value = val["Value"];
		std::stringstream ss;
		for (auto& valv : value["Triangle_m_vertices"].GetArray())
		{
			Triangle_m_vertices.push_back(valv.GetString());
		}
		for (auto& vali : value["Triangle_m_indices"].GetArray())
		{
			Triangle_m_indices.push_back(vali.GetString());
		}
	}

	for (auto& val : obj["Circle_m_meshes"].GetArray())
	{
		const rapidjson::Value& value = val["Value"];
		std::stringstream ss;
		for (auto& valv : value["Circle_m_vertices"].GetArray())
		{
			Circle_m_vertices.push_back(valv.GetString());
		}
		for (auto& vali : value["Circle_m_indices"].GetArray())
		{
			Circle_m_indices.push_back(vali.GetString());
		}

	}

	return true;
}
