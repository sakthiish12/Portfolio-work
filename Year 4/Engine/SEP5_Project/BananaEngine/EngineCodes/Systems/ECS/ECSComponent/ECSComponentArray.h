#pragma once
#ifndef ECS_COMPONENT_ARRAY_H
#define ECS_COMPONENT_ARRAY_H

#include <Components/ECSComponent.h>

namespace BE
{
	//Base class for every component arrays
	class ICompArray
	{
	protected:
		ICompArray() = default;
		virtual ~ICompArray() = default;

		//Removing data from the array for the entity that is going to be destroyed
		virtual void OnEntityDestroyed(EntityID e) = 0;

		//Non-templated version of add/remove/get data - NT = non-templated
		virtual void InsertDataNT(EntityID _eid) = 0;
		virtual void RemoveDataNT(EntityID _eid) = 0;
		virtual IECSComponent& GetDataNT(EntityID _eid) = 0;

		//Only ECS Component manager can access this class
		friend class ECSComponentMgr;
		friend class ECSCoordinator;
	};


	//Each component will have its own data array
	template <typename ComponentType>
	class ECSCompArray : public ICompArray
	{
		/*
			Component data tracking
		*/
		//The actual component data will be stored here
		std::array<ComponentType, MAX_ENTITIES> m_componentData{};

		/*
			Entity ID - Component data array index mapping
		*/

		//Maps from entity ID to index of component data array
		std::unordered_map<EntityID, size_t> m_IDToIndexMap{};

		//Maps from index of component data array to entity ID
		std::unordered_map<size_t, EntityID> m_indexToIDMap{};

		//How many entities are being tracked
		size_t m_trackedEntitiesCount = 0;





		/*
			Constructor and Destructors	
		*/
		ECSCompArray<ComponentType>() = default;
		~ECSCompArray<ComponentType>() = default;

		/*
			Data handling
		*/

		//Putting data into the arrays
		void InsertData(EntityID _eid, ComponentType&& _data);

		//Removing data from the array
		void RemoveData(EntityID _eid);

		//Retrieves data for the specified entity
		ComponentType& GetData(EntityID _eid);

		/*
			Non-templated version of data handling
		*/
		//Non-templated version of add/remove/get data - NT = non-templated
		void InsertDataNT(EntityID _eid) override;
		void RemoveDataNT(EntityID _eid) override;
		IECSComponent& GetDataNT(EntityID _eid) override;

		/*
			Misc functions
		*/
		//When entity is to be destroyed, remove its data
		void OnEntityDestroyed(EntityID _eid) override;





		//Disabling copy constructor/assignment
		ECSCompArray& operator=(const ECSCompArray&) = delete;
		ECSCompArray(const ECSCompArray&) = delete;

		//Only ECS Component manager can access this class
		friend class ECSComponentMgr;
		friend class ECSCoordinator;

		//Static assertions
		static_assert(std::is_base_of<IECSComponent, ComponentType>::value, 
					  "Static Assertion failed: ComponentType must inherit IECSComponent class");
		static_assert(std::is_default_constructible<ComponentType>::value,
					  "Static Assertion failed: ComponentType must have default constructor");
	};
}

#include "ECSComponentArray.hpp"

#endif