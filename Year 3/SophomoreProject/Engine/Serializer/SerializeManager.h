// /******************************************************************************/
// /*!
// \file         SerializeManager.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Manager for handling data
// \lines        551 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#ifndef SERIALIZEMANAGER_H
#define SERIALIZEMANAGER_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../Math/Vector/Vector2.h"
#include "../Math/Vector/Vector3.h"
#include <iostream>
#include <memory>
#include <vector>

//#include "../ECS/ECS.h" DO NOT INCLUDE
#include "ISerializable.h"
#include "../Serializer/SerializeManager.h"

#include "../Serializer/Settings/GraphicsSettings.h"
#include "../Math/Vector/Vector4.h"

using PrettyWriterString = rapidjson::PrettyWriter<rapidjson::StringBuffer>*;

//forward declaration
template<typename T>
struct SerializeArray;
template<typename T>
struct DeserializeArray;

class SerializeManager// : public systemBase
{
public:
	//function to be called to deserialize the json file to initialize all entities, components
	//and systems
	bool DeserializeFromFile(const std::string& _filePath, ISerializable& _component);

	//Overload. Writes directly to the stringstream instead for further processing
	bool DeserializeFromFile(const std::string& _filePath, std::stringstream& buffer);

	//returns a string but actual serialization is called based on the derived type component
	std::string Serialize(const ISerializable& _component) const;
	//function to be called to serialize all entities, components and systems into a json file
	bool SerializeToFile(const std::string& _filePath, const ISerializable& _component);

	//Append JSON-compatible data to an existing string based off an ISerializable's data
	bool SerializeAppendToString(std::string& appendTo, const ISerializable& _component);

	const char* GetString(const char* _text);

	//serialize a Mathf::Vector3 vector into a json file. default type T is float
	template<typename T = float>
	void SerializeVector(const char* _name, const Mathf::Vector<3, T>& _coord, PrettyWriterString _writer)
	{
		if (_name != nullptr)
		{
			_writer->String(_name);
			_writer->StartObject();
		}

		//serialize for floating point vectors by checking the type
		if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
		{
			_writer->String("x");
			_writer->Double(_coord.x);
			_writer->String("y");
			_writer->Double(_coord.y);
			_writer->String("z");
			_writer->Double(_coord.z);
		}

		else
		{
			_writer->String("x");
			_writer->Int(static_cast<int>(_coord.x));
			_writer->String("y");
			_writer->Int(static_cast<int>(_coord.y));
			_writer->String("z");
			_writer->Int(static_cast<int>(_coord.z));
		}

		if (_name != nullptr)
		{
			_writer->EndObject();
		}
	}

	//serialize a glm::vec3 vector into a json file. default type T is float
	template<typename T = float>
	void SerializeVector(const char* _name, const glm::vec<3, T, glm::defaultp>& _coord, PrettyWriterString _writer)
	{
		if (_name != nullptr)
		{
			_writer->String(_name);
			_writer->StartObject();
		}

		//serialize for floating point vectors by checking the type
		if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
		{
			_writer->String("x");
			_writer->Double(_coord.x);
			_writer->String("y");
			_writer->Double(_coord.y);
			_writer->String("z");
			_writer->Double(_coord.z);
		}

		else
		{
			_writer->String("x");
			_writer->Int(static_cast<int>(_coord.x));
			_writer->String("y");
			_writer->Int(static_cast<int>(_coord.y));
			_writer->String("z");
			_writer->Int(static_cast<int>(_coord.z));
		}

		if (_name != nullptr)
		{
			_writer->EndObject();
		}
	}
	//TODO: change to paramter pack to optimize for all serialize vecs
	template<typename T = float>
	void SerializeVector(const char* _name, const glm::vec<4, T, glm::defaultp>& _coord, PrettyWriterString _writer)
	{
		if (_name != nullptr)
		{
			_writer->String(_name);
			_writer->StartObject();
		}

		//serialize for floating point vectors by checking the type
		if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
		{
			_writer->String("x");
			_writer->Double(_coord.x);
			_writer->String("y");
			_writer->Double(_coord.y);
			_writer->String("z");
			_writer->Double(_coord.z);
			_writer->String("w");
			_writer->Double(_coord.w);
		}

		else
		{
			_writer->String("x");
			_writer->Int(static_cast<int>(_coord.x));
			_writer->String("y");
			_writer->Int(static_cast<int>(_coord.y));
			_writer->String("z");
			_writer->Int(static_cast<int>(_coord.z));
			_writer->String("w");
			_writer->Int(static_cast<int>(_coord.w));
		}

		if (_name != nullptr)
		{
			_writer->EndObject();
		}
	}

	//overload for Mathf::Vector2. default would be float if not specified
	template <typename T = float>
	void SerializeVector(const char* _name, const Mathf::Vector<2, T>& _coord, PrettyWriterString _writer)
	{
		if (_name != nullptr)
		{
			_writer->String(_name);
			_writer->StartObject();
		}

		//serialize for floating point vectors by checking the type
		if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
		{
			_writer->String("x");
			_writer->Double(_coord.x);
			_writer->String("y");
			_writer->Double(_coord.y);
		}

		//serialize for int vectors
		else
		{
			_writer->String("x");
			_writer->Int(static_cast<int>(_coord.x));
			_writer->String("y");
			_writer->Int(static_cast<int>(_coord.y));
		}

		if (_name != nullptr)
		{
			_writer->EndObject();
		}
	}

	template <typename T = float>
	void SerializeVector(const char* _name, const glm::vec<2, T, glm::defaultp>& _coord, PrettyWriterString _writer)
	{
		if (_name != nullptr)
		{
			_writer->String(_name);
			_writer->StartObject();
		}
		//serialize for floating point vectors by checking the type
		if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
		{
			_writer->String("x");
			_writer->Double(_coord.x);
			_writer->String("y");
			_writer->Double(_coord.y);
		}

		//serialize for int vectors
		else
		{
			_writer->String("x");
			_writer->Int(static_cast<int>(_coord.x));
			_writer->String("y");
			_writer->Int(static_cast<int>(_coord.y));
		}

		if (_name != nullptr)
		{
			_writer->EndObject();
		}
	}

	//deserialize a Mathf::Vector3 vector from json file back into its data member
	//default type T is float
	template <typename T = float>
	void DeserializeVector(const char* _name, Mathf::Vector<3, T>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
				_coord.z = static_cast<T>(temp["z"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
				_coord.z = static_cast<T>(temp["z"].GetInt());
			}
		}
	}

	//overload for Mathf::Vector2. default type is float
	template <typename T = float>
	void DeserializeVector(const char* _name, Mathf::Vector<2, T>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
			}
		}
	}

	//deserialize a glm::vec3 vector from json file back into its data member
	//default type T is float
	template <typename T = float>
	void DeserializeVector(const char* _name, glm::vec<3, T, glm::defaultp>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
				_coord.z = static_cast<T>(temp["z"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
				_coord.z = static_cast<T>(temp["z"].GetInt());
			}
		}
	}

	//overload for glm::vec2. default type is float
	template <typename T = float>
	void DeserializeVector(const char* _name, glm::vec<2, T, glm::defaultp>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
			}
		}
	}
	//TODO: change as well
	template <typename T = float>
	void DeserializeVector(const char* _name, glm::vec<4, T, glm::defaultp>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
				_coord.z = static_cast<T>(temp["z"].GetDouble());
				_coord.w = static_cast<T>(temp["w"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
				_coord.z = static_cast<T>(temp["z"].GetInt());
				_coord.w = static_cast<T>(temp["w"].GetInt());
			}
		}
	}

	//deserializing of vector 4 
	template <typename T = float>
	void DeserializeVector(const char* _name, Mathf::Vector<4, T>& _coord, const rapidjson::Value& _obj)
	{
		if (_obj.HasMember(_name))
		{
			const rapidjson::Value& temp = _obj[_name];

			if (typeid(T) == typeid(double) || typeid(T) == typeid(float))
			{
				_coord.x = static_cast<T>(temp["x"].GetDouble());
				_coord.y = static_cast<T>(temp["y"].GetDouble());
				_coord.z = static_cast<T>(temp["z"].GetDouble());
				_coord.w = static_cast<T>(temp["w"].GetDouble());
			}

			else
			{
				_coord.x = static_cast<T>(temp["x"].GetInt());
				_coord.y = static_cast<T>(temp["y"].GetInt());
				_coord.z = static_cast<T>(temp["z"].GetInt());
				_coord.w = static_cast<T>(temp["w"].GetInt());
			}
		}
	}
	//serialize a float or double into a json file
	void SerializeDouble(const char* _name, double _value, PrettyWriterString _writer);
	void SerializeFloat(const char* _name, float _value, PrettyWriterString _writer);
	//serialize an int into a json file
	void SerializeInt64(const char* _name, int64_t _value, PrettyWriterString _writer);
	void SerializeInt(const char* _name, int _value, PrettyWriterString _writer);
	//serialize an unsigned int into a json file
	void SerializeUInt64(const char* _name, size_t _value, PrettyWriterString _writer);
	void SerializeUInt(const char* _name, unsigned _value, PrettyWriterString _writer);
	//serialize a string or char* into a json file
	void SerializeString(const char* _name, const char* _value, PrettyWriterString _writer);
	//serialize a bool value into a json file
	void SerializeBool(const char* _name, bool _value, PrettyWriterString _writer);

	//deserialization
	void DeserializeDouble(const char* _name, double& _value, const rapidjson::Value& _obj);
	void DeserializeFloat(const char* _name, float& _value, const rapidjson::Value& _obj);
	void DeserializeInt64(const char* _name, int64_t& _value, const rapidjson::Value& _obj);
	void DeserializeInt(const char* _name, int& _value, const rapidjson::Value& _obj);
	void DeserializeUInt64(const char* _name, size_t& _value, const rapidjson::Value& _obj);
	void DeserializeUInt(const char* _name, unsigned& _value, const rapidjson::Value& _obj);
	void DeserializeString(const char* _name, std::string& _value, const rapidjson::Value& _obj);
	void DeserializeBool(const char* _name, bool& _value, const rapidjson::Value& _obj);
	//todo
	void DeserializeArray(const char* _name, bool& _value, const rapidjson::Value& _obj);
};

//helper to write a std::container into json file
template<template < class ... > class Container, typename Key, class ... Args>
struct WriterWrapper
{
	void WriteContainer
	(PrettyWriterString _writer, const Container<Key, Args...>& _container, const char* _name)
	{
		_writer->String(_name);
		_writer->StartArray();

		for (const auto& i : _container)
		{


			if constexpr (sizeof...(Args) != 0)
			{
				_writer->StartObject();
				_writer->String("Key");

				//if the key is string
				if constexpr (std::is_same<Key, std::string>::value)
				{
					_writer->String(i.first.c_str());
				}
				//if key is a const char*
				else if constexpr (std::is_same<Key, const char*>::value)
				{
					_writer->String(i.first);
				}
				//to be used for GraphicSetting's m_frameBuffers
				else if constexpr (std::is_same<Key, std::pair<std::string, float>>::value)
				{
					_writer->StartObject();
					_writer->String("Name");
					_writer->String(i.first.c_str());
					_writer->String("Value");
					//if the string is a threshold value, save as a double
					if (strcmp(i.first.c_str(), "Threshold") == 0)
					{
						_writer->Double(i.second);
					}
					//if not save it as a int
					else
					{
						_writer->Int(static_cast<int>(i.second));
					}
					_writer->EndObject();
				}
				else if constexpr (std::is_same<Key, Mathf::Vector<2, float>>::value)
				{
					SerializeManager serializeManager;
					serializeManager.SerializeVector(nullptr, i, _writer);
				}
				else
				{
					_writer->Int64(i.first);
				}

				//run this if the value is any type of int/size_t
				if constexpr (std::is_same<Args..., uint8_t>::value ||
					std::is_same<Args..., size_t>::value ||
					std::is_same<Args..., uint64_t>::value)
				{
					_writer->String("Value");
					_writer->Int64(i.second);
				}
				else if constexpr (std::is_same<Args..., int>::value)
				{
					_writer->String("Value");
					_writer->Int(i.second);
				}
				/*for GraphicSetting*/
				else if constexpr (std::is_same<Args..., GraphicsSettings::ShaderPathData>::value)
				{
					_writer->String("Value");
					i.second.OnSerialize(_writer);
				}
				/*for GraphicSetting*/
				else if constexpr (std::is_same<Args..., GraphicsSettings::uniformContainer>::value)
				{
					WriterWrapper<std::vector, std::pair<std::string, float>> array;
					array.WriteContainer(_writer, i.second, "Value");
				}
				
				//for Entity Manager
				else if constexpr (std::is_same<Args..., std::vector<std::string>>::value)
				{
					WriterWrapper<std::vector, std::string> array;
					array.WriteContainer(_writer, i.second, "Value");
				}
				//means its has its own serialize to run
				else
				{
					_writer->String("Value");
					i.second.OnSerialize(_writer);
				}
				_writer->EndObject();
			}

			//for all other containers besides map and unordered map
			else
			{
				if constexpr (std::is_same<Key, unsigned int>::value || std::is_same<Key, unsigned char>::value)
				{
					_writer->Uint(i);
				}
				else if constexpr (std::is_same<Key, std::string>::value)
				{
					_writer->String(i.c_str());
				}
				//to be used for GraphicSetting's m_frameBuffers
				else if constexpr (std::is_same<Key, std::pair<std::string, float>>::value)
				{
					_writer->StartObject();
					_writer->String("Name");
					_writer->String(i.first.c_str());
					_writer->String("Value");
					//if the string is a threshold value, save as a double
					if (strcmp(i.first.c_str(), "Threshold") == 0)
					{
						_writer->Double(i.second);
					}
					//if not save it as a int
					else
					{
						_writer->Int(static_cast<int>(i.second));
					}
					_writer->EndObject();
				}
				// For vector< pair<string string> >
				else if constexpr (std::is_same<Key, std::pair<std::string, std::string>>::value)
				{
					_writer->StartObject();
					_writer->String("Name");
					_writer->String(i.first.c_str());
					_writer->String("Value");
					_writer->String(i.second.c_str());
					_writer->EndObject();
				}
				//TODO: remove the glm::vec ones once all member variables are converted to mathf::vector
				else if constexpr
					(std::is_same<Key, Mathf::Vector2>::value || std::is_same<Key, Mathf::Vector3>::value
						|| std::is_same<Key, glm::vec2>::value || std::is_same<Key, glm::vec3>::value)
				{
					SerializeManager serializeManager;
					_writer->StartObject();
					serializeManager.SerializeVector(nullptr, i, _writer);
					_writer->EndObject();
				}
				//is it a container of a custom class that has its own serialize fnc to call
				else
				{
					i.OnSerialize(_writer);
				}
			}
		}

		_writer->EndArray();
	}
};

//use this specialization if the value has a shared_ptr
template<template < class ... > class Container, typename Key, typename Value>
struct WriterWrapper<Container, Key, std::shared_ptr<Value>>
{
	void WriteContainer
	(PrettyWriterString _writer, const Container<Key, std::shared_ptr<Value>>& _map, const char* _name)
	{
		_writer->String(_name);
		_writer->StartArray();
		for (const auto& i : _map)
		{
			_writer->StartObject();
			_writer->String("Key");

			//for std::String
			if constexpr (std::is_same<Key, std::string>::value)
			{
				_writer->String(i.first.c_str());
			}
			//for const char*
			else
			{
				_writer->String(i.first);
			}

			_writer->String("Value");
			_writer->StartObject();
			i.second->OnSerialize(_writer);
			_writer->EndObject();

			_writer->EndObject();
		}
		_writer->EndArray();
	}
};

template<template < class ... > class Container, typename Key, typename... Args>
struct ReaderWrapper
{
	using pairType = std::tuple<Key, Args...>;

	void ReadContainer
	(const rapidjson::Value& _obj, Container<Key, Args...>& _container)
	{
		//used specially by std::map and unordered_map
		if constexpr (sizeof...(Args) != 0)
		{

			//PartyAssert(_obj.IsArray(), "ReaderWrapper ReadContainer object has to be an array");

			for (const auto& iter : _obj.GetArray())
			{

				const rapidjson::Value& value = iter["Value"];

				//used for GraphicsSettings' m_InitShaders
				if constexpr (std::is_same<pairType, std::tuple<std::string, GraphicsSettings::ShaderPathData>>::value)
				{
					_container[static_cast<Key>(iter["Key"].GetString())].OnDeserialize(value);
				}
				//used for GraphicsSettings' m_FrameBuffers
				else if constexpr (std::is_same<std::pair<Key, Args...>, std::pair<std::string, GraphicsSettings::uniformContainer>>::value)
				{
					ReaderWrapper<std::vector, std::pair<std::string, float>> temp2;
					temp2.ReadContainer(value, _container[static_cast<Key>(iter["Key"].GetString())]);
				}
				//check if the key is a string. used for windowSettings
				else if constexpr (std::is_same<pairType, std::tuple<std::string, int>>::value)
				{
					_container[static_cast<Key>(iter["Key"].GetString())] = value.GetInt();
				}
				else if constexpr (std::is_same<pairType, std::tuple<std::string, int>>::value)
				{
					_container[static_cast<Key>(iter["Key"].GetString())] = value.GetArray();
				}
				else
				{
					_container[static_cast<Key>(iter["Key"].GetInt())].OnDeserialize(value);
				}
			}
		}

		//for all other std::containers without a key and value
		else
		{
			//Assert(_obj.IsArray(), "ReaderWrapper ReadContainer object has to be an array");

			if constexpr (std::is_same<Key, unsigned int>::value || std::is_same<Key, unsigned char>::value)
			{
				for (const auto& iter : _obj.GetArray())
				{
					_container.push_back(static_cast<Key>(iter.GetUint()));
				}
			}
			else if constexpr (std::is_same<Key, std::string>::value)
			{
				for (const auto& iter : _obj.GetArray())
				{
					_container.push_back(iter.GetString());
				}
			}
			//for GraphicsSettings' m_FrameBuffers
			else if constexpr (std::is_same<Key, std::pair<std::string, float>>::value)
			{
				for (const auto& iter : _obj.GetArray())
				{
					_container.push_back({ iter["Name"].GetString(), iter["Value"].GetFloat() });
				}
			}
			//for vector< pair<string, string> >
			else if constexpr (std::is_same<Key, std::pair<std::string, std::string>>::value)
			{
				for (const auto& iter : _obj.GetArray())
				{
					_container.push_back({ iter["Name"].GetString(), iter["Value"].GetString() });
				}
			}
			//TODO: remove the glm::vec ones once all member variables are converted to mathf::vector
			else if constexpr
				(std::is_same<Key, Mathf::Vector2>::value || std::is_same<Key, Mathf::Vector3>::value
					|| std::is_same<Key, glm::vec2>::value || std::is_same<Key, glm::vec3>::value)
			{
				for (const auto& iter : _obj.GetArray())
				{
					Key coord{};	//create a temporary glm::vec to initialize;
					coord.x = static_cast<float>(iter["x"].GetDouble());
					coord.y = static_cast<float>(iter["y"].GetDouble());

					if constexpr (std::is_same<Key, Mathf::Vector3>::value || std::is_same<Key, glm::vec3>::value)
					{
						coord.z = static_cast<float> (iter["z"].GetDouble());
					}

					_container.push_back(coord); //push into the container
				}
			}
			// has its own deserialize fnc to call
			else
			{
				//create a component based on the Key type and insert into the container
				for (const auto& iter : _obj.GetArray())
				{
					Key component;
					component.OnDeserialize(iter);
					//TODO: might nid to change to emplace to cater for more containers
					_container.push_back(component);
				}
			}
		}
	}
};
#endif