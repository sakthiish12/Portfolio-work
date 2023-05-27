// /******************************************************************************/
// /*!
// \file         SerializeManager.cpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Manager for handling data
// \lines        151 Lines
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
#include "SerializeManager.h"

//Deserialize data of an entire file, calls _component.OnDeserialize()
//and writes data for _component
bool SerializeManager::DeserializeFromFile(const std::string& _filePath, ISerializable& _component)
{
	//TODO should not being able to find the file path return false instead of halting engine by assertion?
	PartyAssert(std::filesystem::exists(_filePath), "Unable to access the given file!");
	//std::cout << "Unable to access the given file!\n";
	std::ifstream f(_filePath);
	std::stringstream buffer;
	buffer << f.rdbuf();
	//returns a pointer to the stream buffer object
	f.close();

	SerializeDerived serializer;
	return (serializer.Deserialize(buffer.str(), _component));
}

//Overload to write to buffer instead
bool SerializeManager::DeserializeFromFile(const std::string& _filePath, std::stringstream& buffer)
{
	PartyAssert(std::filesystem::exists(_filePath), "Unable to access the given file!");
	std::ifstream f(_filePath);
	buffer << f.rdbuf();
	f.close();

	return true;
}

std::string SerializeManager::Serialize(const ISerializable& _component) const
{
	rapidjson::StringBuffer ss;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);

	//this is called in the derived component class itself, eg TransformX, Collider etc
	if (_component.OnSerialize(&writer) == true)
	{
		return ss.GetString();
	}

	return "";
}

bool SerializeManager::SerializeToFile(const std::string& _filePath, const ISerializable& _component)
{
	std::ofstream f(_filePath);
	std::string s = Serialize(_component);
	f << s;
	f.flush();
	f.close();

	return true;
}

bool SerializeManager::SerializeAppendToString(std::string& appendTo, const ISerializable& _component)
{
	appendTo += Serialize(_component);
	return true;
}

const char* SerializeManager::GetString(const char* _text)
{
	//marked static to fix the deserializing of strings
	static rapidjson::Document document;
	rapidjson::Value author;
	char buffer[50];
	int len = sprintf_s(buffer, "%s", _text); // dynamically created string.
	author.SetString(buffer, len, document.GetAllocator());

	return author.GetString();
}

void SerializeManager::SerializeDouble(const char* _name, double _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Double(_value);

}

void SerializeManager::SerializeFloat(const char* _name, float _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Double(static_cast<double>(_value));

}

void SerializeManager::SerializeInt64(const char* _name, int64_t _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Int64(_value);

}

void SerializeManager::SerializeInt(const char* _name, int _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Int(_value);

}

void SerializeManager::SerializeUInt64(const char* _name, size_t _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Uint64(_value);

}

void SerializeManager::SerializeUInt(const char* _name, unsigned _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Uint(_value);
}

void SerializeManager::SerializeString(const char* _name, const char* _value, PrettyWriterString _writer)
{

	_writer->String(_name);
	_writer->String(_value);

}

void SerializeManager::SerializeBool(const char* _name, bool _value, PrettyWriterString _writer)
{
	_writer->String(_name);
	_writer->Bool(_value);

}







void SerializeManager::DeserializeDouble(const char* _name, double& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetDouble();
	}
}

void SerializeManager::DeserializeFloat(const char* _name, float& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetFloat();
	}
}

void SerializeManager::DeserializeInt64(const char* _name, int64_t& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetInt64();
	}
}

void SerializeManager::DeserializeInt(const char* _name, int& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetInt();
	}
}

void SerializeManager::DeserializeUInt64(const char* _name, size_t& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetUint64();
	}
}

void SerializeManager::DeserializeUInt(const char* _name, unsigned& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetUint();
	}
}

void SerializeManager::DeserializeString(const char* _name, std::string& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetString();
	}
}

void SerializeManager::DeserializeBool(const char* _name, bool& _value, const rapidjson::Value& _obj)
{
	if (_obj.HasMember(_name))
	{
		_value = _obj[_name].GetBool();
	}
}

void SerializeManager::DeserializeArray(const char* _name, bool& _value, const rapidjson::Value& _obj) {
	static_cast<void>(_value);//unreferenced variable 
	static_cast<void>(_name);//unreferenced variable 
	for (auto& v : _obj.GetArray())
		std::cout << v.GetString();
}