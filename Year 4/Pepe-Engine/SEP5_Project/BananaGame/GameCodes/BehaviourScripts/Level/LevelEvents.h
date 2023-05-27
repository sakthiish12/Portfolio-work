#pragma once
#ifndef	LEVEL_EVENT_H
#define LEVEL_EVENT_H

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;
	enum class EventTriggerType
	{
		CharacterDied = 0,
		Interacted,
		InteractedFirstTime,
		TotalEventTypes
	};

	struct EventTrigger
	{
		std::map<EntityID, bool> eTriggerTargets;
		EventTriggerType eType;

		void CheckEntity(EntityID in) {
			auto itr = eTriggerTargets.find(in);
			if (itr != eTriggerTargets.end())
				itr->second = true;
		}

		void UncheckEntity(EntityID in) {
			auto itr = eTriggerTargets.find(in);
			if (itr != eTriggerTargets.end())
				itr->second = true;
		}

		bool IsFufilled() {
			for (auto& pair : eTriggerTargets) {
				if (pair.second == false)
					return false;
			}
			return true;
		}
	};

	struct EventResolution
	{
		std::set<EntityID> tTargetIDs;
		bool rCanResolve = false;
		bool HasCompleted = false;
		virtual void ActivateResolution() = 0;
	};

	struct LevelEvent : public EventTrigger
	{
		std::shared_ptr<EventResolution> eResolution;


	};
	/*------------------------------------------------------------------------------
		Derived Events
	------------------------------------------------------------------------------*/

	struct SpawnRobot : public EventResolution
	{
		uint8_t SpawnsPerLocation = 1;
		std::vector<Vec3f> Locations ;

		void ActivateResolution() override;
	};

	struct UnlockDoors : public EventResolution
	{
		void ActivateResolution() override;
	};

	struct ObjectiveEvent : public EventResolution
	{
		size_t m_Index;
		void ActivateResolution() override;
	};

	struct UnlockDoorsAndObjective : public EventResolution
	{
		size_t m_Index;
		void ActivateResolution() override;
	};

	struct UnlockDoorsAndDialogue : public EventResolution
	{
		size_t m_Index;
		void ActivateResolution() override;
	};
	struct CustomCallback : public EventResolution
	{
		std::function<void()> callback;
		void ActivateResolution()
		{
			callback();
		};
	};
}

#endif