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
#include "../../pch.h"
#include "AStarGridMapSerializer.h"//
#include "../SerializeManager.h"

bool AStarGridMapSettings::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	SerializeManager serializeManager;

	writer->StartObject();
	//position of grid map 
	serializeManager.SerializeVector("m_GridPos", m_GridPos, writer);
	//radius title
	serializeManager.SerializeFloat("m_Radius",m_Radius, writer);
	//scale of the grid map
	serializeManager.SerializeVector("m_Scale", m_Scale, writer);
	writer->EndObject();
	return true;
}


bool AStarGridMapSettings::OnDeserialize(const rapidjson::Value& obj)
{
	SerializeManager serializeManager;
	serializeManager.DeserializeVector("m_GridPos", m_GridPos, obj);
	serializeManager.DeserializeFloat("m_Radius", m_Radius, obj);
	serializeManager.DeserializeVector("m_Scale", m_Scale, obj);
	return true;
}