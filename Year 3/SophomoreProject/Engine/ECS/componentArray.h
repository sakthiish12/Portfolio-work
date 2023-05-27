// /******************************************************************************/
// /*!
// \file         ComponentArray.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        This file contains the declaration for the component array
// \lines        36 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Component arrays to store components for all entities.

#ifndef COMPONENTARRAY_H
#define COMPONENTARRAY_H
#pragma once

#include <vector>
#include "component.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include "component.h"

using entityID = uint32_t; //4 294 967 296 entities. More than enough.

class componentVectorBase
{
public:
	//Pure virtual destructor for empty base class
	//https://stackoverflow.com/a/13989626
	virtual ~componentVectorBase() = 0
	{
	}

	//Virtual functions
	virtual void reserve(size_t amount) = 0;

	virtual void removeComponent(entityID fromEntity) = 0;

	virtual void duplicateComponent(entityID from, entityID to) = 0;

	virtual void clearArray() = 0;
};

template <typename componentType>
class componentVector : public componentVectorBase
{
	//Vector containing entityIDs concerning
	//which entities the components belong to
	std::vector<entityID> ids;

	//Vector containing components. To follow the order of ids vector
	std::vector<componentType> components;

public:
	//Reserve these many elements in the vectors
	//Action is only taken if the new amount exceeds current capacities
	//Best to call before first use
	//TODO private it
	void reserve(size_t amount) override;

	//ADD COMPONENTS
	//Add componentType containing specified data for specified entity
	//TODO private it
	void addComponent(entityID toEntity, componentType const& withData);

	//Duplicates component data of entity ofEntity to entity toEntity
	//TODO private it
	void duplicateComponent(entityID ofEntity, entityID toEntity);

	//REMOVE COMPONENTS
	//Remove componentType from a specified entity
	//TODO private it
	void removeComponent(entityID fromEntity) override;

	//Remove all components from the array
	//TODO private it
	void clearArray() override;

	//READ COMPONENTS
	//Get the component of an entity
	componentType& getComponent(entityID ofEntity);
};

//Function template definitions here
#include "componentArray.tpp"

#endif //COMPONENTARRAY_H