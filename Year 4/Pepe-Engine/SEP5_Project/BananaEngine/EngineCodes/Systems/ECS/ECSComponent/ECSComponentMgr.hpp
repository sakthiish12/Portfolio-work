#pragma once

namespace BE
{
	template <typename ComponentType>
	void ECSComponentMgr::AddComponent(EntityID _eid, ComponentType&& _data)
	{
		GetComponentArray<ComponentType>()->InsertData(_eid, std::move(_data));
	}

	//Deleting component data from the array
	template <typename ComponentType>
	void ECSComponentMgr::RemoveComponent(EntityID _eid)
	{
		GetComponentArray<ComponentType>()->RemoveData(_eid);
	}

	template <typename ComponentType>
	ComponentType& ECSComponentMgr::GetComponent(EntityID _eid)
	{
		return GetComponentArray<ComponentType>()->GetData(_eid);
	}

	template <typename ComponentType>
	ComponentID ECSComponentMgr::RegisterComponent()
	{
		ComponentID compID = GetComponentID<ComponentType>();

		AssertCore(m_ComponentList.count(compID) == 0, "Component \"%s\" was already registered", typeid(ComponentType).name());
		
		//Truncating the namespace and class/struct from the type name
		std::string compName{ typeid(ComponentType).name() };
		size_t i = compName.find_last_of(" :");
		
		//Store the component ID - component name, and component ID - data array mapping
		m_ComponentList.emplace(compID, compName.substr(i + 1, compName.size() - i - 1));
		m_compName_to_compID.emplace(compName.substr(i + 1, compName.size() - i - 1), compID);
		m_ComponentData.emplace(compID, new ECSCompArray<ComponentType> );

		return compID;
	}

	template <typename ComponentType>
	ComponentID ECSComponentMgr::GetComponentID() const
	{
		static ComponentID newID = m_nextID++;
		return newID;
	}

	template <typename ComponentType>
	ECSCompArray<ComponentType>* ECSComponentMgr::GetComponentArray()
	{
		ComponentID compID = GetComponentID<ComponentType>();
		AssertCore(m_ComponentList.count(compID) != 0, "Component \"%s\" was not registered", typeid(ComponentType).name());
		return dynamic_cast<ECSCompArray<ComponentType>*>(m_ComponentData.at(compID));
	}
}