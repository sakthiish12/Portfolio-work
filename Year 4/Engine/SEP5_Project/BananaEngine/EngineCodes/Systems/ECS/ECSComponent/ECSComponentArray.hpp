#pragma once

namespace BE
{
	template <typename ComponentType> 
	void ECSCompArray<ComponentType>::InsertData(EntityID _eid, ComponentType&& _data)
	{
		//If the array already contains the data for this entity, then just override the data
		if (m_IDToIndexMap.count(_eid) != 0)
		{
			LogCoreError("Entity %llu already has \"%s\" component. Replacing data instead.", *_eid, typeid(ComponentType).name());
			m_componentData[m_IDToIndexMap.at(_eid)] = std::move(_data);
			return;
		}

		//Update the map on where the insertion will happen
		m_IDToIndexMap[_eid] = m_trackedEntitiesCount;
		m_indexToIDMap[m_trackedEntitiesCount] = _eid;
		
		//Move the data into the array
		m_componentData[m_trackedEntitiesCount] = std::move(_data);
		
		//Update number of entities being tracked for this array
		++m_trackedEntitiesCount;
	}

	template <typename ComponentType> 
	void ECSCompArray<ComponentType>::RemoveData(EntityID _eid)
	{
		AssertCore(m_IDToIndexMap.count(_eid) != 0, "Entity %llu does not have \"%s\" component", *_eid, typeid(ComponentType).name());

		//Determine the last entity that will take over the memory position of the entity to be destroyed
		size_t indexToReplace = m_IDToIndexMap.at(_eid);
		size_t indexToLast = m_trackedEntitiesCount - 1;
		EntityID lastEntity = m_indexToIDMap.at(indexToLast);

		//Transfer the component data for the last entity to replace the one to be destroyed
		m_componentData[indexToReplace] = std::move(m_componentData[indexToLast]);
		m_componentData[indexToLast] = std::move(ComponentType{});

		//Update the mapping
		m_IDToIndexMap[lastEntity] = indexToReplace;
		m_indexToIDMap[indexToReplace] = lastEntity;

		//Destroy the old entity mapping
		m_IDToIndexMap.erase(_eid);
		m_indexToIDMap.erase(indexToLast);

		//Update number of entities being tracked for this array
		--m_trackedEntitiesCount;
	}

	template <typename ComponentType> 
	ComponentType& ECSCompArray<ComponentType>::GetData(EntityID _eid)
	{
		AssertCore(m_IDToIndexMap.count(_eid) != 0, "Entity %llu does not have \"%s\" component", *_eid, typeid(ComponentType).name());
		return m_componentData[m_IDToIndexMap[_eid]];
	}

	template <typename ComponentType>
	void ECSCompArray<ComponentType>::InsertDataNT(EntityID _eid)
	{
		this->InsertData(_eid, ComponentType{});
	}

	template <typename ComponentType>
	void ECSCompArray<ComponentType>::RemoveDataNT(EntityID _eid)
	{
		this->RemoveData(_eid);
	}

	template <typename ComponentType>
	IECSComponent& ECSCompArray<ComponentType>::GetDataNT(EntityID _eid)
	{
		return this->GetData(_eid);
	}

	template <typename ComponentType> 
	void ECSCompArray<ComponentType>::OnEntityDestroyed(EntityID _eid)
	{
		//If entity has this component, destroy it
		if (m_IDToIndexMap.count(_eid) != 0)
			this->RemoveData(_eid);
	}
}
