#pragma once

namespace BE
{
	template <typename ComponentType>
	void ECSCoordinator::AddComponent(EntityID _eid, ComponentType&& _data)
	{
		//Update the entity's component bitset
		m_entityMgr->AddComponent(_eid, m_componentMgr->GetComponentID<ComponentType>());

		//Put the actual data into the component array
		m_componentMgr->AddComponent<ComponentType>(_eid, std::move(_data));

		//Update all the trackers to keep track of this entity
		m_trackerMgr->OnEntityComponentChanged(_eid, m_entityMgr->GetEntityComponentBitset(_eid));
	}

	template <typename ComponentType>
	void ECSCoordinator::RemoveComponent(EntityID _eid)
	{
		if constexpr (std::is_same<ComponentType, Transform>::value)
		{
			LogCoreError("Cannot remove Transform component");
		}
		else if constexpr (std::is_same<ComponentType, EntityInfo>::value)
		{
			LogCoreError("Cannot remove EntityInfo component");
		}
		else
		{
			//Update the entity's component bitset
			m_entityMgr->RemoveComponent(_eid, m_componentMgr->GetComponentID<ComponentType>());

			//Remove the actual data from the component array
			m_componentMgr->RemoveComponent<ComponentType>(_eid);

			//Update all the trackers to keep track of this entity
			m_trackerMgr->OnEntityComponentChanged(_eid, m_entityMgr->GetEntityComponentBitset(_eid));
		}
	}

	template <typename ComponentType>
	EntityID ECSCoordinator::FindOwnerOfComponentData(const ComponentType& _componentData) const
	{
		//Get the array of this component type
		ECSCompArray<ComponentType>* container = m_componentMgr->GetComponentArray<ComponentType>();

		size_t idx = (&_componentData - &(container->m_componentData.front()));

		AssertCore(idx < container->m_trackedEntitiesCount, "Given component data does not belong to any entity, \
			make sure you are not getting data from a local copied variable");

		return container->m_indexToIDMap.at(idx);
	}
}