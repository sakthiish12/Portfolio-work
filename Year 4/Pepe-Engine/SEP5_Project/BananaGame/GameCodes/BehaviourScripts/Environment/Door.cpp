
#include "Door.h"

namespace TH
{
	using namespace BE;
	const AssetID doorOpenSFX = AssetID(17642160463763851988);
	const AssetID doorCloseSFX = AssetID(2028999274763248375);
	void DoorScript::Init()
	{
		//if (ECS->GetComponent<EntityInfo>(id).HasTag("GarageDoor")) {
		//	dDoor_type = DoorType::Garage;
		//}
		if (ECS->HasComponent<AudioSource>(id)) {
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			as.AddAudio(doorOpenSFX);
			as.SetVolume(0.5f, doorOpenSFX);
			as.AddAudio(doorCloseSFX);
			as.SetVolume(0.5f, doorCloseSFX);
		}
		
		auto children = ECS->GetComponent<Transform>(id).GetChildren();
		if (dDoor_type == DoorType::Sliding && children.size() >= 2) {
			Door1 = children[0];
			Door2 = children[1];
		}
		else if (dDoor_type == DoorType::Garage && children.size() >= 1) {
			Door1 = children[0];
		}
		dOrignal_Center = ECS->GetComponent<Transform>(Door1).GetPosition();
		
	}

	void DoorScript::Update(double dt)
	{
		DoorCloseBack.update((float)dt);

		if (dIsOpening || dIsClosing)
		{
			int numberOfSteps = Engine->GetPhysSteps();

			//Loop used in systems that have time-based formula
			for (int step = 0; step < numberOfSteps; ++step)
			{
				dDoor_timer += static_cast<float>(Engine->GetFixedDeltaTime() * EngineSpeed->GetEngineSpeedMultiplier());
			}
			if (dDoor_timer >= dCurrent_duration)
			{
				dDoor_timer = 0.f;
				StopDoors();
			}
		}
		if (dQueueDoorOpen) {
			OpenDoors();
			dQueueDoorOpen = false;
		}

		// when target leaves door sensor area
		if (dTargetIsInside && (PhySys->HasUpdatedThisFrame() && !dTargetIsCollided))
		{
			//std::cout << "LOL\n";
			dTargetIsInside = false;
			dQueueDoorClose = true;
		}
		if (dTargetIsInside)
			dQueueDoorClose = false;
		if (dQueueDoorClose) {
			CloseDoors();
		}
		dTargetIsCollided = false;

		if (DoorCloseBack.IsTimerEnded()) {
			CloseDoors();
		}
	}

	inline void DoorScript::StopDoors()
	{
		if (Door1 != NULL_ENTITY) {
			ECS->GetComponent<Physics>(Door1).SetLinearVelocity(Vec3f());
			if (dIsClosing) {
				if(dDoor_type == DoorType::Garage) ECS->GetComponent<Transform>(Door1).SetPosition(dOrignal_Center);
				else if(dDoor_type == DoorType::Sliding) ECS->GetComponent<Transform>(Door1).SetPosition(rightstartpoint);
			}
		}
		if (Door2 != NULL_ENTITY) {
			ECS->GetComponent<Physics>(Door2).SetLinearVelocity(Vec3f());
			if (dIsClosing) {
				if (dDoor_type == DoorType::Sliding) ECS->GetComponent<Transform>(Door2).SetPosition(leftstartpoint);
			}
		}
		dIsOpening = false;
		dIsClosing = false;
	}

	void DoorScript::OpenDoors()
	{
		if (dLocked || dIsOpening) return;
		if (dDoor_state == DoorState::Open) return;

		if (ECS->HasComponent<AudioSource>(id)) {
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			if (as.IsAudioPlaying(doorCloseSFX))
				as.Stop(doorCloseSFX);
			as.Play(doorOpenSFX);
		}
		switch (dDoor_type) {
		case DoorType::Sliding:
		{
			if (Door1 != NULL_ENTITY && Door2 != NULL_ENTITY)
			{
				Transform transf1 = ECS->GetComponent<Transform>(Door1);
				Transform transf2 = ECS->GetComponent<Transform>(Door2);
				Transform transMain = ECS->GetComponent<Transform>(id);
				Mat3f rotMatrix;
				rotMatrix.SetRotation(transMain.GetRotation());

				Vec3f center = transMain.GetPosition();

				float doorscale = ECS->GetComponent<Physics>(Door1).GetShapeSize().x;
				Vec3f left_endpoint = center + rotMatrix * Vec3f::Left() * doorscale * 1.5f;
				Vec3f right_endpoint = center + rotMatrix * Vec3f::Right() * doorscale * 1.5f;
				float time_ratio = (left_endpoint - transf2.GetPosition()).Mag() / doorscale;
				dCurrent_duration = time_ratio * dDoor_duration;
				ECS->GetComponent<Physics>(Door2).MoveKinematic(left_endpoint, transf2.GetRotation(), dCurrent_duration);
				ECS->GetComponent<Physics>(Door1).MoveKinematic(right_endpoint, transf1.GetRotation(), dCurrent_duration);

			}
			break;
		}
		case DoorType::Garage: {
			if (Door1 != NULL_ENTITY)
			{
				Transform transf1 = ECS->GetComponent<Transform>(Door1);
				Transform transMain = ECS->GetComponent<Transform>(id);

				/*Mat3f rotMatrix;
				rotMatrix.SetRotation(transMain.GetRotation());*/

				Vec3f center = transf1.GetPosition();

				float doorscale = ECS->GetComponent<Physics>(Door1).GetShapeSize().y;
				Vec3f endpoint = center + /*rotMatrix * */Vec3f::Up() * doorscale;
				float time_ratio = (endpoint - transf1.GetPosition()).Mag() / doorscale;
				dCurrent_duration = time_ratio * dDoor_duration;
				ECS->GetComponent<Physics>(Door1).MoveKinematic(endpoint, transf1.GetRotation(), dCurrent_duration);
			}
			break;
		}
		case DoorType::Pivoted: {
			break;
		}
		}
		dIsOpening = true;
		dDoor_state = DoorState::Open;
		dDoor_timer = 0.f;
	}

	void DoorScript::OpenDoorsAndCloseAfter(float open_duration)
	{
		OpenDoors();
		DoorCloseBack.duration = open_duration;
		DoorCloseBack.StartTimer();
	}

	void DoorScript::CloseDoors()
	{
		if (dLocked || dIsOpening || dIsClosing) return;
		if (dDoor_state == DoorState::Close) return;

		if (ECS->HasComponent<AudioSource>(id)) {
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			if (as.IsAudioPlaying(doorOpenSFX))
				as.Stop(doorOpenSFX);
			as.Play(doorCloseSFX);
		}
		switch (dDoor_type) {
		case DoorType::Sliding:
		{
			if (Door1 != NULL_ENTITY && Door2 != NULL_ENTITY)
			{
				Transform transf1 = ECS->GetComponent<Transform>(Door1);
				Transform transf2 = ECS->GetComponent<Transform>(Door2);
				Transform transMain = ECS->GetComponent<Transform>(id);
				Mat3f rotMatrix;
				rotMatrix.SetRotation(transMain.GetRotation());

				Vec3f center = transMain.GetPosition();

				float doorscale = ECS->GetComponent<Physics>(Door1).GetShapeSize().x;
				Vec3f left_startpoint = center + rotMatrix * Vec3f::Left() * doorscale * 0.5f;
				Vec3f right_startpoint = center + rotMatrix * Vec3f::Right() * doorscale * 0.5f;
				dCurrent_duration = dDoor_duration;
				leftstartpoint = left_startpoint;
				rightstartpoint = right_startpoint;
				ECS->GetComponent<Physics>(Door2).MoveKinematic(left_startpoint, transf2.GetRotation(), dCurrent_duration);
				ECS->GetComponent<Physics>(Door1).MoveKinematic(right_startpoint, transf1.GetRotation(), dCurrent_duration);
			}
			break;
		}
		case DoorType::Garage: {
			if (Door1 != NULL_ENTITY)
			{
				Transform transf1 = ECS->GetComponent<Transform>(Door1);
				Transform transMain = ECS->GetComponent<Transform>(id);

				float doorscale = ECS->GetComponent<Physics>(Door1).GetShapeSize().y;
				Vec3f endpoint = transf1.GetPosition() + /*rotMatrix * */Vec3f::Down() * doorscale;

				dCurrent_duration = dDoor_duration;
				ECS->GetComponent<Physics>(Door1).MoveKinematic(dOrignal_Center, transf1.GetRotation(), dCurrent_duration);
			}
			break;
		}
		case DoorType::Pivoted: {
			break;
		}
		}
		dIsClosing = true;
		dDoor_state = DoorState::Close;
		dQueueDoorClose = false;
		dDoor_timer = 0.f;
	}

	void DoorScript::OnCollision(EntityID collidingID)
	{
		if (dAllowTriggerOpen && ECS->GetComponent<EntityInfo>(collidingID).HasTag(dTag_Activation))
		{
			dQueueDoorOpen = true;
			if(!dTargetIsInside) dTargetIsInside = true;
			dTargetIsCollided = true;
		}
	}

	void DoorScript::SetDoorLock(bool in)
	{
		dLocked = in;
		if (!dLocked) {
			for (auto cid : ECS->GetComponent<Transform>(id).GetChildren()) {
				if (ECS->HasComponent<lightComponent>(cid)) {
					ECS->GetComponent<lightComponent>(cid).setColor({ 0.f, 1.f, 0.f });
				}
			}
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	rttr::registration::enumeration<DoorType>(RegistrationNameCreator<DoorType>())
	(
		value("Sliding", DoorType::Sliding),
		value("Garage", DoorType::Garage),
		value("Pivoted", DoorType::Pivoted)
	);
	registration::class_<DoorScript>(RegistrationNameCreator<DoorScript>())
		.constructor()
		.property("Door Type", &DoorScript::dDoor_type)
		.property("Door Activation Tag", &DoorScript::dTag_Activation)
		.property("Door Duration", &DoorScript::dDoor_duration)
		.property("Door Open Once", &DoorScript::dOpenOnce)
		.property("Door Locked", &DoorScript::dLocked)
		.property("Allow Trigger Open Door", &DoorScript::dAllowTriggerOpen);
}