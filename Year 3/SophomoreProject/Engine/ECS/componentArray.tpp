// /******************************************************************************/
// /*!
// \file         componentArray.tpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        This file contains the function template definitions for the component array
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Definitions of componentArray functions

#define ECSDEBUG 0

template <typename componentType>
void componentVector<componentType>::reserve(size_t newCapacity)
{
	ids.reserve(newCapacity);
	components.reserve(newCapacity);
#if ECSDEBUG
	std::cout << "[ECS] Component vector reserved for "
		<< newCapacity << " components" << std::endl;
#endif
}

template <typename componentType>
void componentVector<componentType>::addComponent
(entityID toEntity, componentType const& withData)
{
	//If the vectors are empty, add without checking
	if (ids.empty() && components.empty())
	{
		ids.insert(ids.begin(), toEntity);
		components.insert(components.begin(), withData);
#if ECSDEBUG
		std::cout << "[ECS] Component added to entity " << toEntity <<
			std::endl;
#endif
	}
	else
	{
		//Should the vectors have contents in them, do this check
		//Ensure that the entity doesn't already have the component
		//TODO attempt to optimise this function since it's causing a bottleneck
		//The way this is implemented is likely O(log2(n))
		auto foundPair = std::equal_range(ids.begin(), ids.end(), toEntity);
		if (foundPair.first == ids.end() ||
			foundPair.first == foundPair.second)
		{
			//The entity does not have this component yet.
			//Add it at the sorted location of the ids and components vectors.
			ptrdiff_t insertionIndex = foundPair.first - ids.begin();
			ids.insert(ids.begin() + insertionIndex, toEntity);
			components.insert(components.begin() + insertionIndex, withData);
#if ECSDEBUG
			std::cout << "[ECS] Component added to entity " << toEntity <<
				std::endl;
#endif
		}
		else
		{
			//Do nothing should the entity already have this component
#if ECSDEBUG
			std::cout << "[ECS] Entity " << toEntity << " presently has " <<
				"component" << std::endl;
#endif
		}
	}
}

template <typename componentType>
void componentVector<componentType>::duplicateComponent
(entityID fromEntity, entityID toEntity)
{
	//Attempt to duplicate component
#if ECSDEBUG
	std::cout << "[ECS] Duplicating component of entity " << fromEntity <<
		" to entity " << toEntity << std::endl;
#endif
	componentType toDuplicate = getComponent(fromEntity);
	addComponent(toEntity, toDuplicate);
}

//TODO return by pointer instead
template <typename componentType>
componentType& componentVector<componentType>::getComponent
(entityID ofEntity)
{
#if ECSDEBUG
	std::cout << "[ECS] Getting component of entity " << ofEntity << std::endl;
#endif

	//TODO attempt to optimise this function since it's causing a bottleneck
	//The way this is implemented is likely O(log2(n))
	auto foundPair = std::equal_range(ids.begin(), ids.end(), ofEntity);
	if (foundPair.first != ids.end() && foundPair.first != foundPair.second)
	{
		//Component belonging to input entity found
		//Return reference to found component so that other code can perform
		//changes to found entity
		ptrdiff_t foundIndex = foundPair.first - ids.begin();
		return components[foundIndex];
	}
	else
	{
		//Unable to find the component matching the required entity
		//What to return???
		//Cannot be *nullptr
		//Cannot be a throwaway object of componentType created in this block
		//because reference will be dead.
		//Throw an exception because the user should not randomly access
		//entities and their components.

		//Chun Ren suggested on 5th Dec to return a pointer instead
		//so that instead of exception, it will be a nullptr
		throw std::runtime_error("Accessing non-existent component");
	}
}

template <typename componentType>
void componentVector<componentType>::removeComponent
(entityID fromEntity)
{
	//TODO attempt to optimise this function since it's causing a bottleneck
		//The way this is implemented is likely O(log2(n))
	auto foundPair = std::equal_range(ids.begin(), ids.end(), fromEntity);
	if (foundPair.first != ids.end() && foundPair.first != foundPair.second)
	{
		//Component found. Remove
		ptrdiff_t removalIndex = foundPair.first - ids.begin();
		ids.erase(ids.begin() + removalIndex);
		components.erase(components.begin() + removalIndex);
#if ECSDEBUG
		std::cout << "[ECS] Removed component from entity " <<
			fromEntity << std::endl;
#endif
	}
	//Component not found. Do nothing
#if ECSDEBUG
	std::cout << "[ECS] Cannot find component of entity " <<
		fromEntity << " to delete" << std::endl;
#endif
}

template <typename componentType>
void componentVector<componentType>::clearArray()
{
	ids.clear();
	components.clear();
#if ECSDEBUG
	std::cout << "[ECS] Component vector cleared" << std::endl;
#endif
}