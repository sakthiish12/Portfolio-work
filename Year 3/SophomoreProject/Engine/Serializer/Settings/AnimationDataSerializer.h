// /******************************************************************************/
// /*!
// \file         
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations for the animation data serializer 
// \lines        
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
#include "../../Math/Vector/Vector2.h"
#include "../ISerializable.h"

struct AnimationSerializer : public ISerializable
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;

	//member variables
	//storing of the uv points
	//Suggestion to do it as std::vector<std::array<Mathf::Vector2, 4>> instead
	std::vector<std::array<Mathf::Vector2, 4>> uvFramePoints;

	//string contains the action name
	//vector contains the sequence of frame indices
	std::unordered_map<std::string, std::vector<size_t>> aniData;
	//storing of the animation name and the animation frame points 
	//using animationData = std::unordered_map<std::string, std::vector<int>>;
	//
	//using animationDataContainer = std::unordered_map<std::string, animationData>;
	//
	//animationDataContainer animationDataContainer;

	const std::string animationResourcePath = "Resources/Animations/";//getting the animations path 
	
};
