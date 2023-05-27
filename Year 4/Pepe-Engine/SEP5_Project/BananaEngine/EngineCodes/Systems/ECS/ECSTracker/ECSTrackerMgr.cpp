#include "pch.h"
#include "ECSTrackerMgr.h"

namespace BE
{
	ECSTrackerMgr::~ECSTrackerMgr()
	{
		for (auto& pair : m_Trackers)
			delete pair.second;
	}

	void ECSTrackerMgr::OnEntityDestroyed(EntityID _eid)
	{
		//Remove this entity from tracking
		for (const auto& pair : m_Trackers)
		{
			//An iterator to where _eid is stored in the tracker's tracked entities container
			std::remove_reference<decltype(pair.second->GetTrackedEntities())>::type::const_iterator it;

			//If this tracker is tracking the given entity, remove it
			if (pair.second->IsEntityTracked(_eid, &it))
			{
				pair.second->GetTrackedEntities().erase(it);
				pair.second->AdditionalUpdateOnRemoval(_eid);
			}

		}
	}

	void ECSTrackerMgr::OnEntityComponentChanged(EntityID _eid, const CompBitset& _newBitset)
	{
		for (const auto& pair : m_Trackers)
		{
			const CompBitset& sysBitset = m_Trackers_ComponentLookup.at(pair.first);

			//If the entity has the required components for this tracker, add this entity for tracking by this tracker
			if ((sysBitset & _newBitset) == sysBitset)
			{
				//If this tracker does not track the given entity, add it
				if (!pair.second->IsEntityTracked(_eid))
				{
					pair.second->GetTrackedEntities().push_back(_eid);
					pair.second->AdditionalUpdateOnAddition(_eid);
				}
			}
			//If the entity no longer has the required components for this tracker, then remove this entity for tracking from this tracker
			else
			{
				//An iterator to where _eid is stored in the tracker's tracked entities container
				std::remove_reference<decltype(pair.second->GetTrackedEntities())>::type::const_iterator it;

				//If this tracker is tracking the given entity, remove it
				if (pair.second->IsEntityTracked(_eid, &it))
				{
					pair.second->GetTrackedEntities().erase(it);
					pair.second->AdditionalUpdateOnRemoval(_eid);
				}
			}

			//No need to check for entity validity since the entities are stored in a set
		}
	}
}