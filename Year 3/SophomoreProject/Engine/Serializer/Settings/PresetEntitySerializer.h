// /******************************************************************************/
// /*!
// \file         GraphicsSettings.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Special Functions for Entity 
// \lines        10 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Serialized data of an entity

#pragma once
#ifndef PRESETENTITYSERIALIZER_H
#define PRESETENTITYSERIALIZER_H

#include "../ISerializable.h"

struct presetEntitySerializer : public ISerializable
{
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& obj) override;


};

#endif