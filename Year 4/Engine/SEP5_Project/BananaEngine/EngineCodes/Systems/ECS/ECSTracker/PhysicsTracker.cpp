#include "pch.h"
#include "PhysicsTracker.h"
#include "../../PhysicsSystem/sPhysicsSystem.h"
#include "../ECS.h"
namespace BE
{
	void PhysicsTracker::AdditionalUpdateOnAddition(EntityID eid)
	{
		if (PhySys->GetHadInit()) {
			Physics& rPhysics = ECS->GetComponent<Physics>(eid);
			Transform& rTransform = ECS->GetComponent<Transform>(eid);

			PhySys->CreateBody(eid, rPhysics, rTransform.GetPosition() + rPhysics.GetOffset(), rTransform.GetRotation());
			PhySys->GetBodyInterface().AddBody(rPhysics.GetBodyID(), JPH::EActivation::Activate);
			if (rPhysics.GetIsScaleBinded()) {
				rPhysics.SetShapeSize(rTransform.GetScale() /** 0.5f*/);
			}
		}		
	}

	void PhysicsTracker::AdditionalUpdateOnRemoval(EntityID eid)
	{
		PhySys->DeleteBody(eid);
	}

}