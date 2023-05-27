#pragma once
#ifndef	ECS_TRACKER_H
#define ECS_TRACKER_H

#include "../ECSConfig.h"

namespace BE
{
	//Stores all entities that has specified components that the tracker needs

	class IECSTracker
	{
	protected:
		//Stores all entities with the required components that the tracker needs
		std::vector<EntityID> m_trackedEntities{};

		//Constructor and destructor
		IECSTracker() = default;
		virtual ~IECSTracker() = default;

	public:
		//Get all entities tracked by this tracker, which has the required components for data processing
		inline auto& GetTrackedEntities()						{ return m_trackedEntities; }

		//Whether a certain entity is being tracked. _where returns an iterator to _eid in the container m_trackedEntities.
		bool IsEntityTracked(EntityID _eid, const std::optional<std::vector<EntityID>::const_iterator*>& _where = std::nullopt) const;

		//Additional updating needed, if any
		virtual void AdditionalUpdateOnAddition(EntityID _eid)	{ UNUSED(_eid); };	//Additional things to do when adding entity for tracking
		virtual void AdditionalUpdateOnRemoval(EntityID _eid)	{ UNUSED(_eid); };	//Additional things to do when removing entity for tracking

		//Disabling copy constructor/assignment
		IECSTracker& operator=(const IECSTracker&) = delete;
		IECSTracker(const IECSTracker&) = delete;

		friend class ECSTrackerMgr;
	};
}

#endif