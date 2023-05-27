#include "pch.h"
#include "ECSComponentMgr.h"

namespace BE
{
	ECSComponentMgr::~ECSComponentMgr()
	{
		//Free the memory of the data arrays
		for (auto& pair : m_ComponentData)
			delete pair.second;
	}

	
	void ECSComponentMgr::AddComponentNT(const std::string& _component, EntityID _eid)
	{
		m_ComponentData.at(m_compName_to_compID.at(_component))->InsertDataNT(_eid);
	}

	void ECSComponentMgr::RemoveComponentNT(const std::string& _component, EntityID _eid)
	{
		m_ComponentData.at(m_compName_to_compID.at(_component))->RemoveDataNT(_eid);
	}

	IECSComponent& ECSComponentMgr::GetComponentNT(const std::string& _component, EntityID _eid)
	{
		return m_ComponentData.at(m_compName_to_compID.at(_component))->GetDataNT(_eid);
	}

	ComponentID ECSComponentMgr::GetComponentID(const std::string& _componentName) const
	{
		AssertCore(m_compName_to_compID.contains(_componentName), "Component \"%s\" not registered", _componentName.c_str());
		return m_compName_to_compID.at(_componentName);
	}


	void ECSComponentMgr::OnEntityDestroyed(EntityID _eid)
	{
		//Go through each component array and remove _eid's data from them
		for (auto& pair : m_ComponentData)
			pair.second->OnEntityDestroyed(_eid);
	}
}
