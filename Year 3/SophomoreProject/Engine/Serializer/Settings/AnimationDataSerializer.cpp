// /******************************************************************************/
// /*!
// \file         
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee (50%)
//               Ryan Wang Nian Jing (50%)
// \date         18th Jan, 2022
// \brief        This file contains the serializer for the animation data
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../pch.h"
#include "AnimationDataSerializer.h"
#include "../SerializeManager.h"

bool AnimationSerializer::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
    ////position of grid map 
    //serializeManager.SerializeVector("m_GridPos", m_GridPos, writer);
    ////radius title
    //serializeManager.SerializeFloat("m_Radius", m_Radius, writer);
    ////scale of the grid map
    //serializeManager.SerializeVector("m_Scale", m_Scale, writer);

    
    writer->StartObject(); //File level braces
    {
        //UV Coordinates representing points of every 'frame' in the sprite sheet
        writer->Key("uvFramePoints");
        {
            //Start array of ALL frames
            writer->StartArray();
            {
                //Within every frame is an array of eight floats representing
                //the UV coordinates of all four points of a frame
                //Frames are represented by UV coordinates of its four corners
                //[X1, Y1, X2, Y2, X3, Y3, X4, Y4]
                //Typically,
                //Point 1 is the top left
                //Point 2 is the top right
                //Point 3 is the bottom right
                //Point 4 is the bottom left
                for (std::array<Mathf::Vector2, 4> const& frame : uvFramePoints)
                {
                    writer->StartArray();
                    for (Mathf::Vector2 const& point : frame) //4 points
                    {
                        writer->Double(static_cast<double>(point.x));
                        writer->Double(static_cast<double>(point.y));
                    }
                    writer->EndArray();
                }
            }
            writer->EndArray();
        }

        //Currently only holds frame sequences for all animations
        writer->Key("aniData");
        {
            //Container for animations
            writer->StartObject();
            for (std::pair<std::string, std::vector<size_t>> sequence : aniData)
            {
                writer->Key(sequence.first.c_str()); //Animation name
                //Frame sequence
                writer->StartArray();
                for (size_t frame : sequence.second)
                {
                    writer->Uint64(frame);
                }
                writer->EndArray();
            }
            writer->EndObject();
        }
    }
    writer->EndObject(); //File level braces

	return true;
}


bool AnimationSerializer::OnDeserialize(const rapidjson::Value& obj)
{
    //uvFramePoints
    if (obj.HasMember("uvFramePoints") && obj["uvFramePoints"].IsArray())
    {
        const rapidjson::Value& uv{ obj["uvFramePoints"] };
        //https://stackoverflow.com/a/45729000
        //For all frames in the animation
        for (const auto& frame : uv.GetArray())
        {
            float x{ 0.0f };
            float y{ 0.0f };
            std::array<Mathf::Vector2, 4> framePoints;
            //Eight points in a frame
            for (size_t point{ 0 }; point < 8; ++point)
            {
                if (point % 2 == 0) //X
                {
                    x = static_cast<float>(frame.GetArray()[static_cast<rapidjson::SizeType>(point)].GetDouble());
                }
                else //Y
                {
                    y = static_cast<float>(frame.GetArray()[static_cast<rapidjson::SizeType>(point)].GetDouble());
                    //Add point to frame
                    framePoints[point / 2].x = x;
                    framePoints[point / 2].y = y;
                }
            }
            //Add frame to the vector
            uvFramePoints.emplace_back(framePoints);
        }
    }

    //aniData
    if (obj.HasMember("aniData") && obj["aniData"].IsObject())
    {
        //Iterate through individual animations
        //https://stackoverflow.com/a/12743556
        for (auto m{ obj["aniData"].MemberBegin() };
            m != obj["aniData"].MemberEnd(); ++m)
        {
            //Animation's name
            std::string aniName{ m->name.GetString() };

            //Animation's frames
            std::vector<size_t> aniFrames;
            //Iterate through frames and populate frames
            //https://stackoverflow.com/a/45729000
            for (const auto& frame : m->value.GetArray())
            {
                aniFrames.emplace_back(frame.GetUint64());
            }

            //Load animation
            aniData.insert({ aniName, aniFrames });
        }
    }

 	//SerializeManager serializeManager;
    //if (obj.HasMember("FramesData") && obj["FramesData"].IsObject()) 
    //{
    //    const rapidjson::Value& object = obj["Value"];
    //    serializeManager.DeserializeArray("FRAMES", frames, object); //uvFramePoints
    //}
    //if (obj.HasMember("AnimationData") && obj["AnimationData"].IsObject())
    //{
    //    const rapidjson::Value& detail = obj["AnimationData"];
    //    serializeManager.DeserializeString("ANIMATION", ANIMATION NAME, detail); //ANIMATION
    //    if (obj.IsArray()) 
    //    {
    //        serializeManager.DeserializeArray("ANIMATION NAME", ANIMATION NAME, obj); // ANIMATIONNAME
    //    }
    //}


    //ReaderWrapper<std::vector, Mathf::Vector2> Frames;
    //Frames.ReadContainer(obj["FRAMES"], uvFramePoints);

    //ReaderWrapper<std::unordered_map, std::string, int> map;
   // map.ReadContainer(obj["AnimationData"], aniData);
  /*  ReaderWrapper<std::vector, Mathf::Vector2> Frames;
    Frames.ReadContainer(obj["FRAMES"], uvFramePoints);
    
    ReaderWrapper<std::unordered_map, std::string, animationData> aniData;
    aniData.ReadContainer(obj["AnimationData"], animationDataContainer);*/

	return true;
}

//if (data.HasMember("struct Poly") &&
//	data["struct Poly"].IsObject())
//{
//	const rapidjson::Value& detail = data["struct Poly"];
//	sm.DeserializeVector("offset_pos", offset_pos, detail);
//	unsigned int size;
//	sm.DeserializeUInt("model_size", size, detail);
//	models.clear();
//	for (unsigned int i = 0; i < size; ++i)
//	{
//		vec2 newModel{};
//		std::string name{};
//		name += "modelPoint";
//		name += std::to_string(i);
//		sm.DeserializeVector(name.c_str(), newModel, detail);
//		models.push_back(newModel);
//	}
//	points.resize(size);
//	return true;
//}
//return false; //Cannot retrieve component from file