#pragma once
#ifndef	DOOR_H
#define DOOR_H

#include <BananaEngine.h>
#include "../Misc/Timers.h"

namespace TH
{
	using namespace BE;

	enum class DoorType
	{
		Sliding,
		Garage,
		Pivoted
	};

	/*
		Door Script is applied on the parent entity, which contains
		the trigger collider to control opening and closing of the doors.

		The actual door entities are the children of the script entity.
	*/
	class DoorScript : public IScript
	{
		DoorType dDoor_type = DoorType::Sliding;
		std::string dTag_Activation = "Friendly";
		Vec3f dOrignal_Center;
		Vec3f leftstartpoint;
		Vec3f rightstartpoint;
		float dDoor_duration = 1.0f;
		float dCurrent_duration = 1.0f;
		float dDoor_timer = 0.f;
		bool dOpenOnce = false;
		bool dIsOpening = false;
		bool dIsClosing = false;
		bool dLocked = true;
		bool dTargetIsCollided = false;
		bool dTargetIsInside = false;
		bool dQueueDoorClose = false;
		bool dQueueDoorOpen = false;
		EntityID Door1 = NULL_ENTITY; // Left
		EntityID Door2 = NULL_ENTITY; // Right

		bool dAllowTriggerOpen = true;
		Timer DoorCloseBack;

		enum class DoorState
		{
			Open,
			Close
		} 
		dDoor_state = DoorState::Close;

	public:
		void SetDoorLock(bool in);
		inline void SetAllowTriggerOpen(bool in) { dAllowTriggerOpen = in; }
		inline bool GetDoorLock() const { return dLocked; }
		inline void SetTagActivation(std::string const& tag) { dTag_Activation = tag; }
		inline std::string const& GetTagActivation() const { return dTag_Activation; }

		void Init();
		void Update(double);

		inline float GetDoorOpenCloseDuration() const { return dDoor_duration;  }
		inline void SetDoorOpenCloseDuration(float in) { dDoor_duration = in; }
		inline void StopDoors();
		void OpenDoors();
		void OpenDoorsAndCloseAfter(float open_duration);
		void CloseDoors();
		void OnCollision(EntityID collidingID);

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}

#endif
