#pragma once
#ifndef	ECS_COMPONENT_MGR_H
#define ECS_COMPONENT_MGR_H

#include "ECSComponentArray.h"

namespace BE
{
	class ECSComponentMgr : public Singleton<ECSComponentMgr>
	{
		/*
			Component data storage
		*/

		//The name of the components that are loaded into the ECS
		std::unordered_map<ComponentID, std::string> m_ComponentList{};

		//Maps component name to component ID
		std::unordered_map<std::string, ComponentID> m_compName_to_compID{};

		//The actual place where component data are stored
		std::unordered_map<ComponentID, ICompArray*> m_ComponentData{};

		//For issuing out component IDs, as each component is registered, this value will go up
		mutable ComponentID m_nextID = 0;
	




		/*
			Constructor and Destructor
		*/

		ECSComponentMgr() = default;
		~ECSComponentMgr();

		/*
			Component Array functions
		*/

		//Adding component data into the array
		template <typename ComponentType>
		void AddComponent(EntityID _eid, ComponentType&& _data);

		//Deleting component data from the array
		template <typename ComponentType>
		void RemoveComponent(EntityID _eid);

		//Getting component from the array
		template <typename ComponentType>
		ComponentType& GetComponent(EntityID _eid);

		/*
			Non-templated version of the above functions
		*/
		//Adding component data into the array (non-templated)
		void AddComponentNT(const std::string& _component, EntityID _eid);

		//Deleting component data from the array (non-templated)
		void RemoveComponentNT(const std::string& _component, EntityID _eid);

		//Getting component from the array (non-templated)
		IECSComponent& GetComponentNT(const std::string& _component, EntityID _eid);


		/*
			Component Manager functions
		*/

		//Registering a new component for use
		template <typename ComponentType>
		ComponentID RegisterComponent();

		//Get the ID of the component
		template <typename ComponentType>
		ComponentID GetComponentID() const;

		//Removes component data from all arrays when an entity is to be destroyed
		void OnEntityDestroyed(EntityID _eid);

		//Get the ID of the component - non templated
		ComponentID GetComponentID(const std::string& _componentName) const;

		/*
			Getters
		*/

		//Get all the components that are registered
		inline const auto& GetAllComponentTypes() const { return m_ComponentList; }

		//Helper function for Accessing the derived class component array
		template <typename ComponentType>
		ECSCompArray<ComponentType>* GetComponentArray();



		//Only ECS coordinator can access this class
		friend class ECSCoordinator;
		friend class Singleton<ECSComponentMgr>;
	};
}

#include "ECSComponentMgr.hpp"

//Include all the different types of components here
#include <Components/Transform.h>
#include <Components/EntityInfo.h>
#include <Components/AudioSource.h>
#include <Components/AudioListener.h>
#include <Components/Physics.h>
#include <Components/GameLogic.h>
#include <Components/Renderer.h>
#include <Components/CameraComponent.h>
#endif