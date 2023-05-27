#pragma once

namespace BE
{
	//Registers a new tracker for use to track entities with required components
	template <typename TrackerName, typename... Args>
	TrackerID ECSTrackerMgr::RegisterTracker(const CompBitset& _trackedComponents, Args&& ...args)
	{
		TrackerID sysID = GetTrackerID<TrackerName>();
		AssertCore(m_Trackers.count(sysID) == 0, "ECS Tracker \"%s\" was already registered", typeid(TrackerName).name());

		//Store the tracker ID - tracker name, and tracker ID - data array mapping
		m_Trackers.emplace(sysID, new TrackerName{ std::forward<Args>(args)... });
		m_Trackers_ComponentLookup.emplace(sysID, _trackedComponents);

		return sysID;

		static_assert(std::is_base_of<IECSTracker, TrackerName>::value, "ECS Tracker must inherit IECSTracker class");
	}

	//Get the ID to this tracker
	template <typename TrackerName>
	TrackerID ECSTrackerMgr::GetTrackerID()
	{
		static TrackerID newID = m_nextID++;
		return newID;

		static_assert(std::is_base_of<IECSTracker, TrackerName>::value, "ECS Tracker must inherit IECSTracker class");
	}

	//Get access to the tracker itself - contains the set of entities with required components
	template <typename TrackerName>
	TrackerName* const ECSTrackerMgr::GetTracker()
	{
		TrackerID sysID = GetTrackerID<TrackerName>();
		AssertCore(m_Trackers.at(sysID) != 0, "ECS Tracker \"%s\" was not registered", typeid(TrackerName).name());
		return dynamic_cast<TrackerName*>(m_Trackers.at(sysID));

		static_assert(std::is_base_of<IECSTracker, TrackerName>::value, "ECS Tracker must inherit IECSTracker class");
	}

	//Gets a reference to all the tracked entities by the tracker
	template <typename TrackerName>
	auto& ECSTrackerMgr::GetTrackedEntities()
	{
		TrackerID sysID = GetTrackerID<TrackerName>();
		AssertCore(m_Trackers.at(sysID) != 0, "ECS Tracker \"%s\" was not registered", typeid(TrackerName).name());
		return m_Trackers.at(sysID)->GetTrackedEntities();

		static_assert(std::is_base_of<IECSTracker, TrackerName>::value, "ECS Tracker must inherit IECSTracker class");
	}
}