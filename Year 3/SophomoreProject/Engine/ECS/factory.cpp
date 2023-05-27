// /******************************************************************************/
// /*!
// \file         factory.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Definitions of ECS factory functions to ease entity creation
// \lines        33 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Definitions of helper functions

#include "factory.h"

entityID newBaseEntity
(float posX, float posY, float rot, float scaleX, float scaleY)
{
	//TODO subobtimal initialisation used
	//transform2D t{ Mathf::vec2{posX, posY}, rot, Mathf::vec2{scaleX, scaleY} };
	transform2D t;
	t.pos = Mathf::vec3(posX, posY,1.f);
	t.rotation = rot;
	t.scale = Mathf::vec2(scaleX, scaleY);

	return ecs.addComponent<transform2D>(t);
}

entityID newBaseEntity
(transform2D const& t)
{
	return ecs.addComponent<transform2D>(t);
}

entityID newSpriteEntity
(spriteCom const& s, float posX, float posY, float rot, float scaleX, float scaleY)
{
	entityID newID = ecs.addComponent<spriteCom>(s);
	//transform2D t{ Mathf::vec2{posX, posY}, rot, Mathf::vec2{scaleX, scaleY} };
	transform2D t;
	t.pos = Mathf::vec3(posX, posY,1.f);
	t.rotation = rot;
	t.scale = Mathf::vec2(scaleX, scaleY);
	ecs.addComponent<transform2D>(newID, t);
	return newID;
}

entityID newSpriteEntity
(spriteCom const& s, transform2D const& t)
{
	entityID newID = ecs.addComponent<spriteCom>(s);
	ecs.addComponent<transform2D>(newID, t);
	return newID;
}