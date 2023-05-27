// /******************************************************************************/
// /*!
// \file         GraphicsSettings.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special Functions for Graphics Settings
// \lines        45 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <unordered_map>
#include <string>
#include <utility>
#include "../ISerializable.h"

struct GraphicsSettings : public ISerializable
{
	std::string name; // Name of this Entity

	std::vector<std::string> tags; // Current tags assigned to this Entity
public:
	//TODO: ShaderPathData should only be accessible by GraphicsSettings
	struct ShaderPathData :public ISerializable
	{
		ShaderPathData() = default;
		ShaderPathData(const std::string& _VertexPath, const std::string& _FragmentPath, const std::string& _GeometryPath = std::string());

		bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
		bool OnDeserialize(const rapidjson::Value& obj) override;

		std::string m_VertexPath;
		std::string m_FragmentPath;
		std::string m_GeometryPath;
	};

	//key is the shader name, value is the ShaderPathData
	using frameInitContainer = std::unordered_map<std::string, ShaderPathData>;

	using uniformContainer = std::vector<std::pair<std::string, float>>;
	//key is which shader to use, the value is the SetUniform() to set the threshold
	using frameBufferContainer = std::unordered_map<std::string, uniformContainer>;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;


	bool OnDeserialize(const rapidjson::Value& obj) override;


	//containers to store the data for shaders and frame buffer
	frameInitContainer m_InitShaders;
	frameBufferContainer m_FrameBuffers;



	//meshes 
	struct MeshesPathData :public ISerializable
	{
	
		bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
		bool OnDeserialize(const rapidjson::Value& obj) override;

		std::unordered_map<std::string, float> m_meshes;//for the meshes indices and vertices 

	};
};

struct Mesh : public ISerializable 
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;

	//member variables
	//std::vector<std::string> m_vertices;
	//ReaderWrapper<std::vector, std::string> m_vertices_array;
	//ReaderWrapper<std::vector, std::string> m_indices_array;
	//std::string shape;
	//std:: m_vertices;
	//std::unordered_map<std::string, string> m_vertices;
	//std::unordered_map<std::string, string> m_indices;

	//std::string shape;
	//std::unordered_map<std::string, double> m_vertices;
	//std::unordered_map<std::string, int> m_indices;
	std::vector<std::string>Square_m_vertices;
	std::vector<std::string>Square_m_indices;
	std::vector<std::string>Triangle_m_vertices;
	std::vector<std::string>Triangle_m_indices;
	std::vector<std::string>Circle_m_vertices;
	std::vector<std::string>Circle_m_indices;

};