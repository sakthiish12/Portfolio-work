// /******************************************************************************/
// /*!
// \file         GraphicsSettings.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special Functions for Entity 
// \lines        9 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Implementation of serialisation and deserialisation of
//preset entities with JSON file.

#include "../../pch.h"
#include "PresetEntitySerializer.h"
#include "../SerializeManager.h"

//TODO NO GOOD IMPLEMENTATION YET. DON'T CALL
//This is called when saving a new preset entity to a JSON file
bool presetEntitySerializer::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	static_cast<void>(writer);//unreferenced variable 
	return false;
}

//Loading a preset entity
