#pragma once
#ifndef	ECS_TRACKER_MGR_H
#define ECS_TRACKER_MGR_H

#include "ECSTracker.h"

namespace BE
{
	class ECSTrackerMgr : public Singleton<ECSTrackerMgr>
	{
		/*
			ECS Entities tracking
		*/
		//Stores the set of entities that have the required components in ComponentLookup
		std::unordered_map<TrackerID, IECSTracker*> m_Trackers{};

		/*
			ECS Tracker information
		*/
		//Stores the set of component that each entity requires in order to process the entities
		std::unordered_map<TrackerID, CompBitset> m_Trackers_ComponentLookup{};

		//For registration of systems
		TrackerID m_nextID = 0;





		/*
			Constructors and Destructors
		*/
		ECSTrackerMgr() = default;
		~ECSTrackerMgr();

		/*
			Tracker Manager functions
		*/

		//Registers a new tracker for use to track entities with required components. Args is ctor param for SystemName
		template <typename TrackerName, typename... Args>
		TrackerID RegisterTracker(const CompBitset& _trackedComponents, Args&& ...args);

		//Get the ID to this tracker
		template <typename TrackerName>
		TrackerID GetTrackerID();

		//Get access to the tracker itself - contains the set of entities with required components
		template <typename TrackerName>
		TrackerName* const GetTracker();

		//Gets a reference to all the tracked entities by the tracker
		template <typename TrackerName>
		auto& GetTrackedEntities();

		/*
			Tracker Manager update functions
		*/
		//Removes entity from tracking when entity is destroyed
		void OnEntityDestroyed(EntityID _eid);

		//Update the tracker's tracking when entity's component changes
		void OnEntityComponentChanged(EntityID _eid, const CompBitset& _newBitset);



		//Only ECS coordinator can access this class
		friend class ECSCoordinator;
		friend class Singleton<ECSTrackerMgr>;
	};
}

#include "ECSTrackerMgr.hpp"
#include "AudioListenerTracker.h"
#include "PhysicsTracker.h"
#include "AnimationComponentTracker.h"
#include "GameLogicTracker.h"
#include "RendererTracker.h"
#include "lightComponentTracker.h"
#include "ParticleSystemTracker.h"
//Include all the different types of trackers here

#endif
