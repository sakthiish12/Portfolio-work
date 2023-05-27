// /******************************************************************************/
// /*!
// \file         factory.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Declarations of ECS factory functions to ease entity creation
// \lines        13 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Helper functions to call the ECS to create various preset objects quickly

#ifndef FACTORY_H
#define FACTORY_H

#include "ECS.h"
#include "component.h"

//Create a new entity with transform
entityID newBaseEntity(float posX = 0.0f, float posY = 0.0f, 
	float rot = 0.0f, 
	float scaleX = 1.0f, float scaleY = 1.0f);

//Overload newBaseEntity() with reference to transform2D passed in instead
entityID newBaseEntity(transform2D const& t);

//Base entity with sprite
entityID newSpriteEntity(spriteCom& s,
	float posX = 0.0f, float posY = 0.0f,
	float rot = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f);

//Overrides above
entityID newSpriteEntity(spriteCom const& s, transform2D const& t);

#endif