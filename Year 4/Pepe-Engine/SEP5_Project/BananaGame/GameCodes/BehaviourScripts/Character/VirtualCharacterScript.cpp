#include "VirtualCharacterScript.h"

namespace TH
{
	using namespace BE;
	void TH::VirtualCharacter::Awake()
	{
		cStandingShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), JPH::Quat::sIdentity(), new JPH::CapsuleShape(0.5f * cCharacterHeightStanding, cCharacterRadiusStanding)).Create().Get();
		// Create character collider
		/*JPH::Ref<JPH::CharacterSettings> csettings = new JPH::CharacterSettings();
		csettings->mMaxSlopeAngle = Math::DegToRad(45.0f);
		csettings->mLayer = PhysicLayers::MOVING;
		csettings->mShape = cStandingShape;
		csettings->mGravityFactor = 0.f;
		csettings->mFriction = 0.f;
		cChar_collider = new JPH::Character(csettings, JPH::Vec3::sZero(), JPH::Quat::sIdentity(), 0, PhySys->GetJoltSystem());
		cChar_collider->AddToPhysicsSystem(JPH::EActivation::Activate);*/

		// Create 'player' character
		JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
		settings->mMaxSlopeAngle = cMaxSlopeAngle;
		settings->mMaxStrength = cMaxStrength;
		settings->mShape = cStandingShape;
		settings->mCharacterPadding = cCharacterPadding;
		settings->mPenetrationRecoverySpeed = cPenetrationRecoverySpeed;
		settings->mPredictiveContactDistance = cPredictiveContactDistance;
		Transform& transf = ECS->GetComponent<Transform>(id);
		cCharacter = new BECharacterVirtual(settings, transf.GetPosition(), transf.GetRotation());
		cCharacter->SetListener(&cListener);
		cCharacter->id = id;
	
		//PhySys->GetBodyInterface().SetShape(ECS->GetComponent<Physics>(id).GetBodyID(), cStandingShape, false, JPH::EActivation::Activate);
		ECS->GetComponent<Physics>(id).SetGravityFactor(0.f);
		ECS->GetComponent<Physics>(id).SetFriction(0.f);
		ECS->GetComponent<Physics>(id).SetIsTrigger(true);
		ECS->GetComponent<Physics>(id).SetIsVirtualCharacter(true);
	}

	void VirtualCharacter::Update(double dt)
	{
		// update physics tranform
		Transform& transf = ECS->GetComponent<Transform>(id);
		Vec3f position_temp = transf.GetPosition();
		position_temp.y -= cCharacter->GetShape()->GetLocalBounds().GetExtent().GetY();
		cCharacter->SetPosition(JPHtoBE(position_temp));
		cCharacter->SetRotation(JPHtoBE(transf.GetRotation()));

		// Viutual Character's physics update
		int numberOfSteps = Engine->GetPhysSteps();
		for (int step = 0; step < numberOfSteps; ++step)
		{
			if (cPushChar) {
				cPushChar = false;
				cCharacter->SetLinearVelocity(cCharacter->GetLinearVelocity() + JPHtoBE(cPushDir));
			}
			// Remember old position
			JPH::Vec3 old_position = cCharacter->GetPosition();
			// Update the character position (instant, do not have to wait for physics update)

			JPH::PhysicsSystem const* joltsys = PhySys->GetJoltSystem();
			//std::cout << EngineSpeed->GetEngineSpeedMultiplier() << std::endl;
			cCharacter->Update(static_cast<float>(Engine->GetFixedDeltaTime()) * EngineSpeed->GetEngineSpeedMultiplier()
				, joltsys->GetGravity(), joltsys->GetDefaultBroadPhaseLayerFilter(PhysicLayers::MOVING), joltsys->GetDefaultLayerFilter(PhysicLayers::MOVING), { }, PhySys->GetAllocator());

			// Allow user to turn off walk stairs algorithm
			if (cEnableWalkStairs)
			{
				// Calculate how much we wanted to move horizontally
				JPH::Vec3 desired_horizontal_step = cCharacter->GetLinearVelocity() * static_cast<float>(Engine->GetFixedDeltaTime());
				desired_horizontal_step.SetY(0);
				float desired_horizontal_step_len = desired_horizontal_step.Length();

				// Calculate how much we moved horizontally
				JPH::Vec3 achieved_horizontal_step = cCharacter->GetPosition() - old_position;
				achieved_horizontal_step.SetY(0);
				float achieved_horizontal_step_len = achieved_horizontal_step.Length();

				// If we didn't move as far as we wanted and we're against a slope that's too steep
				if (achieved_horizontal_step_len + 1.0e-4f < desired_horizontal_step_len
					&& cCharacter->CanWalkStairs())
				{
					// Calculate how much we should step forward
					JPH::Vec3 step_forward_normalized = desired_horizontal_step / desired_horizontal_step_len;
					JPH::Vec3 step_forward = step_forward_normalized * (desired_horizontal_step_len - achieved_horizontal_step_len);

					// Calculate how far to scan ahead for a floor
					JPH::Vec3 step_forward_test = step_forward_normalized * cMinStepForward;

					cCharacter->WalkStairs(static_cast<float>(Engine->GetFixedDeltaTime()), joltsys->GetGravity() * 1.5f, JPHtoBE(cStepUpHeight), step_forward, step_forward_test, JPH::Vec3::sZero(), joltsys->GetDefaultBroadPhaseLayerFilter(PhysicLayers::MOVING), joltsys->GetDefaultLayerFilter(PhysicLayers::MOVING), { }, PhySys->GetAllocator());
				}
			}
		}
		// character collider
		// cChar_collider->SetPositionAndRotation(cCharacter->GetPosition(), cCharacter->GetRotation());

		// update tranform
		Vec3f position = JPHtoBE(cCharacter->GetPosition());
		position.y += cCharacter->GetShape()->GetLocalBounds().GetExtent().GetY();
		transf.SetPosition(position);
		transf.SetRotation(JPHtoBE(cCharacter->GetRotation()));

	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<VirtualCharacter>(RegistrationNameCreator<VirtualCharacter>())
		.constructor()
		.property("Character Height Standing", &VirtualCharacter::cCharacterHeightStanding)
		.property("Character Radius Standing", &VirtualCharacter::cCharacterRadiusStanding)
		.property("Min Step Forward", &VirtualCharacter::cMinStepForward)
		.property("Step Up Height", &VirtualCharacter::cStepUpHeight);
}