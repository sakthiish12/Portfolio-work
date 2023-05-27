// /******************************************************************************/
// /*!
// \file         system.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         08th Oct, 2021
// \brief        Definition of base system for ECS architecture
// \lines        161 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//ECS Architecture - System Base Class
//This header file serves to define the base system that
//every system should inherit directly from.
//TODO Put definitions in a CPP
//TODO Make the base system functions more secure (restrict access)

//POINTERS ON SYSTEM DESIGN
//1) Please write your system's implementation in a separate file
//   within the Systems folder for organisation's sake.
//2) All systems should directly inherit from systemBase
//3) Systems serve to manipulate component data of compatible entities,
//   usually on a continuous frame-by-frame basis.
//4) This ECS architecture is defined such that two kinds of inclusion policies
//   may be adopted by systems when they consider which entities they would
//   consider for data manipulation. These are:
//   a) Entities only need all the components the system requires for the
//      system to consider the entity for data manipulation. This is default.
//   b) Entities must ONLY contain the components the system requires for the
//      system to consider the entity for data manipulation.
//5) For components that are included in many entities (like Transform,
//   which should be included in every entity), it is expected that many
//   systems require the entity contain such a component for the system to
//   consider the entity.
//6) All systems should have a function of signature 'void update(float dt)'
//   that iterates through all appropriate entities and their needed components
//   The systems may contain other class variables and unique functions

#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <algorithm>
#include "componentArray.h"

class systemBase
{
private:
	//If TRUE, the entity must have the exact same list of components as what
	//the system needs in order for the entity to be considered for the system.
	//If FALSE, the entity only needs the components required by the system
	//in order for the entity to be considered for the system.
	//Default to be FALSE.
	bool requireMatchingComponentList;

	//Modification of the components bitset is done through member functions
	std::vector<bool> componentsNeeded;

	//Make it public for traversal in game functions
	std::vector<entityID> entitiesRegistered;
public:
	//Constructor
	//initialComponents - initial number of components for system to consider
	//initialEntities - initial number of entities for system to consider
	//requireComponentMatch - whether EXACT match of component set is required
	//for entity to be registered by system
	systemBase(bool requireComponentMatch = false) :
		requireMatchingComponentList{ requireComponentMatch },
		componentsNeeded{ std::vector<bool>(32) },
		entitiesRegistered{ std::vector<entityID>() }
	{

	}

	//Check compatibility between a given bitset (of an entity)
	//and the system's component bitset given the system's conditions
	bool checkCompatibility(std::vector<bool>& entityBitset)
	{
#if 0
		std::cout << "[ECS] Comparing bitsets: " << std::endl;
		std::cout << "ENTITY BITSET: ";
		for (bool b : entityBitset)
			std::cout << b;
		std::cout << std::endl;

		std::cout << "SYSTEM BITSET: ";
		for (bool b : componentsNeeded)
			std::cout << b;
		std::cout << std::endl;
#endif

		//How much to compare between the two bitsets
		//Should be the length of the shorter of the two
		size_t comparisonLength;
		if (entityBitset.size() < componentsNeeded.size())
			comparisonLength = entityBitset.size();
		else
			comparisonLength = componentsNeeded.size();

		//Compare
		for (size_t i = 0; i < comparisonLength; ++i)
		{
			if (requireMatchingComponentList)
			{
				if (entityBitset[i] != componentsNeeded[i])
					return false;
			}
			else
			{
				if (componentsNeeded[i] == true &&
					entityBitset[i] == false)
					return false;
			}
		}

		//Check remaining bits of the longer bitset
		if (componentsNeeded.size() > entityBitset.size())
		{
			for (size_t i = comparisonLength;
				i < componentsNeeded.size();
				++i)
				if (componentsNeeded[i] == true)
					return false;
		}
		else if (entityBitset.size() > componentsNeeded.size() &&
			requireMatchingComponentList)
		{
			for (size_t i = comparisonLength;
				i < entityBitset.size();
				++i)
				if (entityBitset[i] == true)
					return false;
		}

		//All checks passed. Return true.
		return true;
	}

	//COMPONENT BITSET RELATED

	//Make it such that system requires this component to include
	//TODO private this out and turn it into a friend
	void setComponentBit(size_t targetBit)
	{
#if 0
		std::cout << "[ECS] Setting component bit " <<
			targetBit << " of system" << std::endl;
#endif
		//Resize bitset if targetBit is beyond current range
		if (targetBit >= componentsNeeded.size())
			componentsNeeded.resize(targetBit + 1);
		componentsNeeded[targetBit] = true;
	}

	//Make it such that system does not require this component to include
	//TODO private this out and turn it into a friend
	void resetComponentBit(size_t targetBit)
	{
#if 0
		std::cout << "[ECS] Resetting component bit " <<
			targetBit << " of system" << std::endl;
#endif
		//Resize bitset if targetBit is beyond current range
		if (targetBit >= componentsNeeded.size())
			componentsNeeded.resize(targetBit + 1);
		componentsNeeded[targetBit] = false;
	}

	//ENTITY REGISTRATION RELATED

	//Register an entity into the vector of entities considered
	//TODO private this out
	void registerEntity(entityID entityToRegister)
	{
		//If the vector is empty, add without checking
		if (entitiesRegistered.empty())
			entitiesRegistered.push_back(entityToRegister);
		else
		{
			//Should the vector have contents in it, do this check
			//Ensure that the entity is not already registered
			auto foundPair = std::equal_range(
				entitiesRegistered.begin(),
				entitiesRegistered.end(),
				entityToRegister);
			if (foundPair.first == entitiesRegistered.end() ||
				foundPair.first == foundPair.second)
			{
				//The entity is not registered yet.
				//Add it at sorted location
				ptrdiff_t insertionIndex =
					foundPair.first - entitiesRegistered.begin();
				entitiesRegistered.insert(
					entitiesRegistered.begin() + insertionIndex,
					entityToRegister);
#if 0
				std::cout << "[ECS] Registered entity " << entityToRegister <<
					" to system" << std::endl;
#endif
			}
			else
			{
#if 0
				//Do nothing if the entity is already registered.
				std::cout << "[ECS] Entity " << entityToRegister <<
					" already registered to system" << std::endl;
#endif
			}
		}
	}

	//TODO private this out
	void unregisterEntity(entityID targetEntity)
	{
		auto foundPair = std::equal_range(
			entitiesRegistered.begin(),
			entitiesRegistered.end(),
			targetEntity);
		if (foundPair.first != entitiesRegistered.end() &&
			foundPair.first != foundPair.second)
		{
			//Entity found. Remove
			ptrdiff_t removalIndex =
				foundPair.first - entitiesRegistered.begin();
			entitiesRegistered.erase
			(entitiesRegistered.begin() + removalIndex);
#if 0
			std::cout << "[ECS] Unregistered entity " <<
				targetEntity << " from system" << std::endl;
#endif
		}
		else
		{
#if 0
			//Entity not registered. Do nothing
			std::cout << "[ECS] Entity " << targetEntity <<
				" not registered to system yet" << std::endl;
#endif
		}
		
	}

	//TODO private this out
	void unregisterAllEntities()
	{
		entitiesRegistered.clear();
#if 0
		std::cout << "[ECS] Unregistered all entities from system" << 
			std::endl;
#endif
	}

	std::vector<entityID> const& getEntitiesRegistered() const
	{
		//This function should be called for every system every update loop
		//This results in too much printing that will clog up the log
		/*std::cout << "[ECS] Entities registered in system: ";
		for (auto e : entitiesRegistered)
			std::cout << e << ", ";
		std::cout << std::endl;*/
		return entitiesRegistered;
	}
};

#endif