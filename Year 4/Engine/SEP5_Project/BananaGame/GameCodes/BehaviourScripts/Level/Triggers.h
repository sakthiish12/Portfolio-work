#pragma once
#ifndef	TRIGGERS_H
#define TRIGGERS_H

#include <BananaEngine.h>
#include "../Character/VirtualCharacterScript.h"
#include "../Camera/CameraController.h"
#include "LevelManager.h"
#include "../Environment/Door.h"
namespace TH
{
	using namespace BE;

	class Teleporter : public IScript
	{

		Vec3f EndLocation = { 0.9f, 7.5f, -27.6f };
		Vec3f EndRotation = { 0.f, 90.f, 0.f };

		void OnCollision(EntityID collidingID);
		size_t CutsceneIndex = 1;
		bool ShownCutscene = false;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class DoorUnlockTrigger : public IScript
	{
		std::set<EntityID> doors;
		bool ShownDialogue = false;
		void Init()
		{
			const auto doors1 = ECS->FindEntitiesByTag("DoorUnlock2");
			for (auto eid : doors1) {
				doors.insert(eid);
			}
		}

		void OnCollision(EntityID collidingID);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class SuperChargeStress : public IScript
	{
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class EndPoint : public IScript
	{
		bool once = false;

		void OnCollision(EntityID collidingID);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class AggroPlayerTrigger : public IScript
	{
		std::string tagGroup;

		void OnCollision(EntityID collidingID);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	class BossRoomTrigger : public IScript
	{
		EntityID boss = NULL_ENTITY;
		bool HasPlayed = false;
		void Init() override;
		void OnCollision(EntityID collidingID) override;


	public: 
		void ResetTrigger() { HasPlayed = false; }
		RTTR_ENABLE(IScript)
			RTTR_REGISTRATION_FRIEND
	};
}
#endif
