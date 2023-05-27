#pragma once
#ifndef	LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <BananaEngine.h>
#include "../../Application/GameConfig.h"
#include "../Character/CharacterPather.h"
#include "../Camera/CameraController.h"
#include "LevelEvents.h"

namespace TH
{
	using namespace BE;

	class LevelScript : public IScript
	{
		std::vector<LevelEvent> Events;
	public:

		void Awake() {
			Events.reserve(10);
		}
		void Update(double dt)
		{
			for (size_t i = 0; i < Events.size(); ++i)
			{
				if (Events[i].eResolution->HasCompleted) continue;
				if (Events[i].eResolution->rCanResolve == true) {
					Events[i].eResolution->ActivateResolution();
					Events[i].eResolution->HasCompleted = true;
				}
			}
		}
		void AddEvent(LevelEvent const& new_event) {
			Events.emplace_back(new_event);
		}

		void UpdateEventTrigger(EntityID in, EventTriggerType type)
		{
			for (size_t i = 0; i < Events.size(); ++i)
			{
				if (Events[i].eResolution->HasCompleted) continue;
				if (Events[i].eType == type) {
					Events[i].CheckEntity(in);
					if (Events[i].IsFufilled()) {
						Events[i].eResolution->rCanResolve = true;
					}
				}
			}
		}

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class LevelManager : public IScript
	{
		std::set<EntityID> mCharacters;
		Vec3f Gravity = Vec3f(0.f, -20.f, 0.f);
		size_t mWallCount;
		int mPlayerInAreaZone = 0;
		bool GameOverIsShowing = false;
		bool LevelCompleteIsShowing = false;
		bool AbilityMenuIsShowing = false;

		//UI Canvases
		BE::CanvasID gameoverCanvas = BE::NULL_ENTITY;
		BE::CanvasID levelCompleteCanvas = BE::NULL_ENTITY;
		BE::CanvasID abilityMenuCanvas = BE::NULL_ENTITY;

	public:
		inline bool GetGameOverIsShowing() const { return GameOverIsShowing; }

		void ShowGameOver();
		void ShowLevelComplete();

		void ToggleAbilityMenu(bool on);

		void Awake();

		void Init();

		void Update(double dt);

		LevelScript* GetCurrentLevel()
		{
			if (ECS->HasComponent<GameLogic>(id)) {
				auto ptr = ECS->GetComponent<GameLogic>(id).GetScript<LevelScript>();
				if (ptr) {
					LevelScript* temp = dynamic_cast<LevelScript*>(ptr.get());
					if (temp)
						return temp;
				}
			}
			return nullptr;
		}
		void CharacterDied(EntityID in) {
			auto* levelptr = GetCurrentLevel();
			if(levelptr)
				levelptr->UpdateEventTrigger(in, EventTriggerType::CharacterDied);
		}
		void ObjectInteracted(EntityID in) {
			auto* levelptr = GetCurrentLevel();
			if (levelptr)
				levelptr->UpdateEventTrigger(in, EventTriggerType::Interacted);
		}
		void ObjectInteractedFirstTime(EntityID in) {
			auto* levelptr = GetCurrentLevel();
			if (levelptr)
				levelptr->UpdateEventTrigger(in, EventTriggerType::InteractedFirstTime);
		}

		inline void SetPlayerZone(int zone_number) {
			mPlayerInAreaZone = zone_number;
		}
		inline int GetPlayerZone() const {
			return mPlayerInAreaZone;
		}

		inline void Retry() 		 { GameOverIsShowing = false; }


		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


}

#endif