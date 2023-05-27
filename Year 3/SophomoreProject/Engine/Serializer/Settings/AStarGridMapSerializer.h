// /******************************************************************************/
// /*!
// \file         
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
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
#include "../ISerializable.h"

struct AStarGridMapSettings : public ISerializable
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;

	//member variables
	Mathf::vec2 m_GridPos;//position of the grid map
	float m_Radius;//radius of the grid map 
	Mathf::vec2 m_Scale; //scale of the grid map
};
