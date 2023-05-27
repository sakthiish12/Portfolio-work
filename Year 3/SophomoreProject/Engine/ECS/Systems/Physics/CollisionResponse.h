 /******************************************************************************/
 /*!
 \file         CollisionResponse.h
 \project name Engine
 \author(s)    Ho Zi Xing
 \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
 \brief        Collision response function declarations
 \lines        22 Lines

 All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
 reserved.
 Copyright (C) 2021 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the
 prior written consent of DigiPen Institute of Technology is prohibited.
  */
  /******************************************************************************/

#pragma once
#include "../../component.h"
#include "../../../Math/Vector/Vector2.h"
#include "../../Systems/Physics/ImplicitGrid.h"

void CollisionResponse_CirclePillar(const Vec2& normal,
									const float& interTime,
									const Vec2& ptStart,
									const Vec2& ptInter,
									Vec2& ptEnd,
									Vec2& reflectedVectorNormalized);
void CollisionResponse_CircleCircle(Vec2& normal,
									const float interTime,
									Vec2& velA,
									const float& massA,
									Vec2& interPtA,
									Vec2& velB,
									const float& massB,
									Vec2& interPtB,
									Vec2& reflectedVectorA,
									Vec2& ptEndA,
									Vec2& reflectedVectorB,
									Vec2& ptEndB);

//void CollisionResponseRectRect(std::shared_ptr<collidablePairID> m)
//{
//	//Check if static
//	
//	//check if is trigger event kind
//
//	//calculate relative velocity (rv)
//
//	// calculate rv in terms of collision normal dir (dir in which impulse will be applied)
//	float vel_along_norm = GMath::DotProduct(rv, m->normal);
//	if (vel_along_norm > 0.0f) // ensure colliders are moving towards each other
//	{
//		// velocities are separating; no need to resolve collision
//		return;
//	}
//
//	float e = 0.5f;
//
//	float imp_scalar = -(1 + e) * vel_along_norm;
//	float sum_inv_mass = rb_a->GetInvMass() + rb_b->GetInvMass();
//	if (sum_inv_mass == 0.0f)
//	{
//		return;
//	}
//	imp_scalar /= (sum_inv_mass);
//
//	// apply impulse
//	GMath::Vec2 impulse = imp_scalar * m->normal;
//	float sum_mass = rb_a->GetMass() + rb_b->GetMass();
//	// return if total mass is 0
//	if (sum_mass == 0.0f)
//	{
//		return;
//	}
//	float ratio = rb_a->GetMass() / sum_mass;
//	if (!a_is_static && ((bc_a->GetIsTrigger() && b_unphasable) || !bc_a->GetIsTrigger()))
//	{
//		rb_a->SetVelocity(
//			rb_a->GetVelocity() - ratio * impulse
//		);
//	}
//	if (!b_is_static && ((bc_b->GetIsTrigger() && a_unphasable) || !bc_b->GetIsTrigger()))
//	{
//		rb_b->SetVelocity(
//			rb_b->GetVelocity() + ratio * impulse
//		);
//	}
//
//	// if both are not static objects, allow the bouncing correction to take place
//	// if one is static, will do the basic response only!
//	if (!a_is_static && !b_is_static)
//	{
//		// Handle positional error (sinking objects and jittering) after the impulse is applied
//		// -- move each object along collision normal by a % of pen depth
//		const float percent = 0.3f; // typically 20-80 %
//		// -- prevent jittering by having some slack; only perform correction if pen above a slop (arbitrary threshold)
//		const float slop = 0.05f; // typically 0.01 to 0.1
//		// note: scale pen depth by total mass => gives correction proportional to how much mass dealing with
//		//GMath::Vec2 correction = m->pen / sum_inv_mass * percent * m->normal;
//		GMath::Vec2 correction = GMath::Max(m->pen - slop, 0.0f) / sum_inv_mass * percent * m->normal;
//		//std::cout << "[CR] correction: " << correction.x() << "," << correction.y() << std::endl;
//		//std::cout << "[CR] change: " << rb_a->GetInvMass() * correction.x() << "," << rb_a->GetInvMass() * correction.y() << std::endl;
//		// apply correction:
//		// + ensure transform is overwritten by these changes as well
//		rb_a->SetPosition(rb_a->GetPosition() - rb_a->GetInvMass() * correction);
//		t_a->SetPosition(rb_a->GetPosition());
//		rb_b->SetPosition(rb_b->GetPosition() + rb_b->GetInvMass() * correction);
//		t_b->SetPosition(rb_b->GetPosition());
//	}
//}


