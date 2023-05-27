// /******************************************************************************/
// /*!
// \file         GameObjectSerializer.cpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        GameobjectSerializer changed to in component.cpp
// \lines        115 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../pch.h"
#include"GameObjectSerializer.h"
#include "../SerializeManager.h"
using namespace std;
//
bool GameObjectSerializer::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	//SerializeManager serializeManager;
	
	////window height and width
	//serializeManager.SerializeVector("m_windowSize", m_windowSize, writer);
	////window title
	//serializeManager.SerializeString("m_windowTitle", m_windowTitle.c_str(), writer);
	////window configurations
	//WriterWrapper<std::unordered_map, std::string, int> map;
	//map.WriteContainer(writer, m_WindowConfigurations, "m_WindowConfigurations");
	//writer->EndObject();
	//SerializeManager serializeManager;
	//writer->StartArray();
	//serializeManager.SerializeInt("id", id, writer);
	//serializeManager.SerializeString("name", name.c_str(), writer);
	//serializeManager.SerializeString("m_texture", m_texture.c_str(), writer);
	//serializeManager.SerializeString("m_shader", m_shader.c_str(), writer);
	//writer->EndArray();
	//writer->StartArray();
	//serializeManager.SerializeUInt("Key", entityIds, writer);
	//serializeManager.registerEntity(entityIds);
	//writer->EndArray();
	//writer->EndObject();
	//serializeManager.SerializeString();

	writer->StartObject();
	writer->Key("People");
	writer->StartArray();
	for (int i = 0; i < 3; i++)
	{
		writer->StartObject();
		writer->Key("name");
		writer->String("Player");
		writer->Key("m_texture");
		writer->String("transform");
		writer->Key("sex");
		writer->String("Basic.shader");
		writer->EndObject();
	}
	writer->EndArray();

	writer->EndObject();


	return true;
}
//
//
//typedef unsigned SizeType;
bool GameObjectSerializer::OnDeserialize(const rapidjson::Value& obj)
{
	SerializeManager serializeManager;

	if (obj.HasMember("entities") && obj["entities"].IsObject())
	{
		const rapidjson::Value& object = obj["entities"];
		if (object.HasMember("id") && object["id"].IsInt())
		{
			cout << "Object.id = " << object["id"].GetInt() << endl;
		}

		if (object.HasMember("transform2D") && object["transform2D"].IsObject())
		{
			const rapidjson::Value& TransformDetails = object["transform2D"];
			if (TransformDetails.HasMember("pos") && TransformDetails["pos"].IsObject())
			{
				const rapidjson::Value& position = TransformDetails["pos"];
				serializeManager.DeserializeVector("pos", pos, position);
			}
			if (TransformDetails.HasMember("rotation") && TransformDetails["rotation"].IsDouble())
			{
				cout << "Object.rotation = " << TransformDetails["rotation"].GetDouble() << endl;
				serializeManager.DeserializeDouble("rotation", rot, object);
			}
			if (TransformDetails.HasMember("scale") && TransformDetails["scale"].IsObject())
			{
				const rapidjson::Value& scale = TransformDetails["scale"];
				serializeManager.DeserializeVector("scale", scal, scale);
			}
		}


		if (object.HasMember("rigidBody2D") && object["rigidBody2D"].IsObject())
		{
			const rapidjson::Value& Transform = object["rigidBody2D"];
			if (Transform.HasMember("previousTransform") && Transform["previousTransform"].IsObject())
			{
				const rapidjson::Value& TransformDetails = object["transform2D"];
				if (TransformDetails.HasMember("pos") && TransformDetails["pos"].IsObject())
				{
					const rapidjson::Value& position = TransformDetails["pos"];
					serializeManager.DeserializeVector("pos", pos, position);
				}
				if (TransformDetails.HasMember("rotation") && TransformDetails["rotation"].IsDouble())
				{
					cout << "Object.rotation = " << TransformDetails["rotation"].GetDouble() << endl;
					serializeManager.DeserializeDouble("rotation", rot, object);
				}
				if (TransformDetails.HasMember("scale") && TransformDetails["scale"].IsObject())
				{
					const rapidjson::Value& scale = TransformDetails["scale"];
					serializeManager.DeserializeVector("scale", scal, scale);
				}
			}
		}

		if (object.HasMember("rigidBody2D") && object["rigidBody2D"].IsObject())
		{
			const rapidjson::Value& Transform2 = object["rigidBody2D"];

			if (Transform2.HasMember("vel") && Transform2["vel"].IsObject())
			{
				serializeManager.DeserializeVector("vel", vel, Transform2);
			}
			if (Transform2.HasMember("acceleration") && Transform2["acceleration"].IsObject())
			{
				serializeManager.DeserializeVector("acceleration", acceleration, Transform2);
			}
			if (Transform2.HasMember("Force") && Transform2["Force"].IsObject())
			{
				serializeManager.DeserializeVector("Force", Force, Transform2);
			}

			if (Transform2.HasMember("Mass") && Transform2["Mass"].IsDouble())
			{
				cout << "Mass = " << Transform2["Mass"].GetDouble() << endl;
			}
			if (Transform2.HasMember("InvMass") && Transform2["InvMass"].IsDouble())
			{
				cout << "InvMass = " << Transform2["InvMass"].GetDouble() << endl;
			}
			if (Transform2.HasMember("Restitution") && Transform2["Restitution"].IsDouble())
			{
				cout << "Restitution = " << Transform2["Restitution"].GetDouble() << endl;
			}
			if (Transform2.HasMember("Friction") && Transform2["Friction"].IsDouble())
			{
				cout << "Friction = " << Transform2["Friction"].GetDouble() << endl;
			}
			if (Transform2.HasMember("isGravityOn") && Transform2["isGravityOn"].IsBool())
			{
				cout << "isGravityOn = " << Transform2["isGravityOn"].GetBool() << endl;
			}
		}
		}
	return true;
	}


//void GameObjectSerializer::BuildAndSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer)
//{
//
//	
// if (obj.HasMember("GameObjects") && obj["GameObjects"].IsArray())
//	{
//		const rapidjson::Value& array = obj["GameObjects"];
//		size_t len = array.Size();
//		for (size_t i = 0; i < len; i++)
//		{
//			const rapidjson::Value& object = array[i];
//			//In order to prevent type mismatch, type verification is generally added
//			if (object.IsObject())
//			{
//				cout << "ObjectArray[" << i << "]: ";
//				if (object.HasMember("Key") && object["Key"].IsInt())
//				{
//					cout << "Key=" << object["Key"].GetInt();
//					//id[i]= object["id"].GetInt();
//					//serializeManager.DeserializeInt("Key", key, obj);
//				}
//				if (object.HasMember("Value") && object["Value"].IsObject())
//				{
//					//cout << "Value=" << object["Value"].GetString();
//					//id[i]= object["id"].GetInt();
//					//serializeManager.DeserializeInt("Key", key, obj);
//					const rapidjson::Value& object2 = object["Value"];
//					if (object2.HasMember("CurrentAnimation") && object2["CurrentAnimation"].IsObject())
//					{
//						cout << "CurrentAnimation=" << object2["CurrentAnimation"].GetString();
//						//id[i]= object["id"].GetInt();
//						//serializeManager.DeserializeInt("Key", key, obj);
//
//						const rapidjson::Value& object3 = object3["CurrentAnimation"];
//
//						if (object3.HasMember("Name") && object3["Name"].IsString())
//						{
//							cout << "Key=" << object3["Key"].GetString();
//							
//						}
//
//
//
//
//
//					}
//					const rapidjson::Value& object4 = object4["CurrentAnimation"];
//					if (object2.HasMember("POS") && object2["POS"].IsObject())
//					{
//						const rapidjson::Value& object4 = object4["POS"];
//						if (object4.HasMember("POS") && object4["POS"].IsString())
//						{
//							cout << "Key=" << object4["x"].GetString();
//							//cout << "Key=" << object3["POS"].GetString();
//
//						}
//					
//					}
//
//					const rapidjson::Value& object5 = object5["CurrentAnimation"];
//					if (object2.HasMember("FrameSize") && object2["FrameSize"].IsObject())
//					{
//						const rapidjson::Value& object5 = object2["FrameSize"];
//
//						if (object5.HasMember("x") && object5["x"].IsString())
//						{
//							//cout << "Key=" << object4["x"].GetString();
//							//cout << "Key=" << object3["POS"].GetString();
//						}
//
//						//cout << "FrameSize=" << object2["FrameSize"].GetString();
//						//id[i]= object["id"].GetInt();
//						//serializeManager.DeserializeInt("Key", key, obj);
//					}
//					if (object2.HasMember("ColumnCount") && object2["ColumnCount"].IsString())
//					{
//						cout << "ColumnCount = " << object2["ColumnCount"].GetInt() << endl;
//					}
//					if (object2.HasMember("RowCount") && object2["RowCount"].IsString())
//					{
//						cout << "RowCount = " << object["RowCount"].GetInt() << endl;
//					}
//				}
//			
//			/*	if (object.HasMember("name") && object["name"].IsString())
//				{
//					cout << ", name=" << object["name"].GetString();
//					
//				}
//				if (object.HasMember("m_texture") && object["m_texture"].IsString())
//				{
//					cout << "m_texture=" << object["m_texture"].GetString();
//					
//				}
//				if (object.HasMember("m_shader") && object["m_shader"].IsString())
//				{
//					cout << "m_shader=" << object["m_shader"].GetString();
//					
//				}
//					if (object.HasMember("Value") && object["Value"].IsObject())
//				{
//					const rapidjson::Value& Value = object["Value"];
//					if (Value.HasMember("CurrentAnimation") && Value["CurrentAnimation"].IsObject())
//					{
//						if (Value.HasMember("Name") && Value["Name"].IsString())
//						{
//							cout << "Value.Name = " << Value["Name"].GetString() << endl;
//						//serializeManager.DeserializeString("Name", NameArray, obj);
//						}
//						if (Value.HasMember("Frames") && Value["Frames"].IsInt())
//						{
//							cout << "Value.Frames = " << Value["Frames"].GetInt() << endl;
//						}
//					}
//					if (Value.HasMember("age") && Value["age"].IsInt())
//					{
//						cout << "Value.age = " << Value["age"].GetInt() << endl;
//					}
//				}*/
//			}
//		}
//	}
//	
////
//
//}
