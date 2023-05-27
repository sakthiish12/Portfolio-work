#pragma once
#ifndef	ECS_CONFIG_H
#define ECS_CONFIG_H

#include <Precompiled/pch.h>
#include <Debugger/Debugger.h>
#include <Utilities/BEID/BEID.h>
#include <Utilities/Singleton/Singleton.h>

namespace BE
{
	//Entities constants and typedefs - used to keep track of entities that are active in the scene
	constexpr size_t MAX_ENTITIES = 4096;
	using EntityID = BEID;
	using TagID = BEID;
	const EntityID NULL_ENTITY = EntityID(0);
	const TagID NULL_TAG = TagID(0);
	

	//Components constants and typedefs - used to keep track of component data and what components an entity has
	constexpr size_t MAX_COMPONENTS = 32;
	using ComponentID = std::uint32_t;
	using CompBitset = std::bitset<MAX_COMPONENTS>;


	//Trackers constants and typedefs - used to keep track of entities with a specific set of components
	using TrackerID = std::uint32_t;
}

#endif