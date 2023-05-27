#pragma once
#ifndef	PREFABASSET_H
#define PREFABASSET_H

#include "IAsset.h"
#include <Systems/ECS/ECS.h>


namespace BE
{
	struct PrefabAssetInitializer
	{
		std::string m_filePath{};
		PrefabAssetInitializer() = default;
		PrefabAssetInitializer(const PrefabAssetInitializer& in) = default;
		PrefabAssetInitializer(const std::string& filePath) : m_filePath{ filePath } {}
		RTTR_ENABLE()
	};

	class PrefabAsset : public IAsset
	{
		//Stores component ID and the associated data
#if 0
		std::unordered_map<ComponentID, std::shared_ptr<IECSComponent>> m_prefabComponents{};
#endif
		std::string m_fileName{};

	public:
		/*
			Constructions
		*/
		PrefabAsset() = default;
		PrefabAsset(const std::string& _file);
		PrefabAsset(PrefabAsset&& _rhs) noexcept;
		PrefabAsset& operator=(PrefabAsset&& _rhs) noexcept;
		~PrefabAsset() = default;

		//Construct the data for non-templated load asset
		void Construct(const PrefabAssetInitializer& _data);
		void ConstructFromPtr(PrefabAssetInitializer* _data);
		PrefabAssetInitializer Serialise();



#if 0
		/*
			Prefab data adding/overwriting
		*/
		//If component doesn't exist, add this component. IF component exists, overwrite the data
		template <typename ComponentType>
		void UpdateComponentData(const ComponentType& _data);

		//If component doesn't exist, add this component. IF component exists, overwrite the data - non templated
		void UpdateComponentData(ComponentID _cid, const IECSComponent& _data);
		void UpdateComponentData(const std::string& _componentName, const IECSComponent& _data);



		/*
			Prefab data removal
		*/
		//Remove component data from this prefab
		template <typename ComponentType>
		void RemoveComponentData();

		//Remove component data from this prefab - non templated
		void RemoveComponentData(ComponentID _cid);
		void RemoveComponentData(const std::string& _componentName);


		/*
			Prefab data retrival
		*/
		//Getting all prefab's component data, can be used for modifying
		inline const auto& GetPrefabComponents() const { return m_prefabComponents; }

		//Check if entity has the prefab data
		template <typename ComponentType>
		bool HasPrefabComponent() const;

		//Check if entity has the prefab data - non templated
		bool HasPrefabComponent(ComponentID _cid) const;
		bool HasPrefabComponent(const std::string& _componentName) const;
		
		//Retrieve prefab data for given component - will return empty ptr if component doesn't exist (check with .expired())
		template <typename ComponentType>
		std::weak_ptr<ComponentType> GetPrefabComponent() const;

		//Retrieve prefab data for given component - will return empty ptr if component doesn't exist (check with .expired())
		std::weak_ptr<IECSComponent> GetPrefabComponent(ComponentID _cid) const;
		std::weak_ptr<IECSComponent> GetPrefabComponent(const std::string& _componentName) const;
	
#endif

		/*
			Updating instances - will be done in ECS side
		*/
	
		RTTR_ENABLE(IAsset)
	};

}

#endif