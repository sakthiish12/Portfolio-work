// /******************************************************************************/
// /*!
// \file         CollisionSystem.cpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         13rd Apr, 2022
// \brief        Collision system, also handle resolution
// \lines        77 Lines to 500 (episode)
// \lines        500 Lines to 531 (gold)
// 
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "CollisionSystem.h"
#include "../../Debugging/PhysicsDebugging.h"
#include "CollisionDetection.h"
#include "../Physics/physicsSystem.h"
#include "Manifold.h"

std::shared_ptr<CollisionSystem> collisionSystemsPtr;
std::shared_ptr<CollisionPolygonSystem> collisionPolySystemsPtr;
using std::cout;
using std::endl;

//
//void ApplyAngularImpulse(rigidBody2D& rb, const Vector3& force)
//{
//	rb.vel.x += rb.InvMass * force.x;
//	rb.vel.y += rb.InvMass * force.y;
//	//angularVelocity += inverseInteriaTensor * force;
//}

void ApplyLinearImpulse(rigidBody2D& rb, const Vector3& force)
{
	rb.vel.x += rb.InvMass * force.x;
	rb.vel.y += rb.InvMass * force.y;
	//linearVelocity += force * inverseMass;
}

//void ImpulseResolveCollision(GameObject& a, GameObject&b, CollisionDetection::ContactPoint& p) const 
void ImpulseResolveCollision(collidablePairID& m) 
{
	transform2D& transformA = ecs.getComponent<transform2D>(m.first);
	transform2D& transformB = ecs.getComponent<transform2D>(m.second);
	rigidBody2D *rb1 = nullptr, *rb2 = nullptr;

	//To see which object is rotatable (by right should use rb to check, but now get it work first)
	Poly *pp1 = nullptr, *pp2 = nullptr; 
	if (ecs.entityHas<Poly>(m.first))
		pp1 = &ecs.getComponent<Poly>(m.first);
	if (ecs.entityHas<Poly>(m.second))
		pp2 = &ecs.getComponent<Poly>(m.second);

	//Only do collision response if it is not static
	//Static means no RB or (have RB but static)
	if (m.static1 == false && m.hasRB1 == true)
		rb1 = &ecs.getComponent<rigidBody2D>(m.first);
	if (m.static2 == false && m.hasRB2 == true)
		rb2 = &ecs.getComponent<rigidBody2D>(m.second);

	float InvMass1 = 0.f, InvMass2 = 0.f, totalMass = 0.f;
	if (rb1)
	{
		InvMass1 = rb1->InvMass;
		totalMass += rb1->InvMass;
	}
	if (rb2)
	{
		InvMass2 = rb2->InvMass;
		totalMass += rb2->InvMass;
	}

	//if (!rb1 && rb2)
	//	m.collisionNormal *= -1;

	// Separate them out using projection
	transformA.pos = transformA.pos - Vector3{ (m.collisionNormal * m.penetration * (InvMass1 / totalMass)), 0.f };
	transformB.pos = transformB.pos + Vector3{ (m.collisionNormal * m.penetration * (InvMass2 / totalMass)), 0.f };

	//cout << "Entity: " << m.first << " vs " << m.second << endl;
	//cout << "static-ness: " << m.static1 << " vs " << m.static1 << endl;
	//cout << "Penetration: " << m.penetration << endl;
	//cout << "CollisionNormal" << m.collisionNormal.x << ", " << m.collisionNormal.y << endl;
	//cout << endl;
	//cout << endl;
	//cout << endl;

	Vector3 fullVelocityA{ 0 };// + angVelocityA;
	if (rb1)
		fullVelocityA = { rb1->vel, 0.f };
	Vector3 fullVelocityB{ 0 };// + angVelocityB;
	if (rb2)
		fullVelocityB = { rb2->vel, 0.f };

	//std::cout << "Reso vel1: " << fullVelocityA.x << ", " << fullVelocityA.y << "Reso vel1: " << fullVelocityB.x << ", " << fullVelocityB.y << std::endl;
	
	Vector3 contactVelocity = fullVelocityB - fullVelocityA;
	//if (rb1 && !rb2 || !rb1 && rb2)
		//if (contactVelocity)

	//float impulseForce = Vector3::Dot(contactVelocity, p.normal);
	float impulseForce = Mathf::Dot(contactVelocity.getVec2(), m.collisionNormal);

	//rotation related
	 // now to work out the effect of inertia ....
	//Vector3 inertiaA = Mathf::Cross(physA - > GetInertiaTensor() * Mathf::Cross(relativeA, m.collisionNormal), relativeA);
	//Vector3 inertiaB = Mathf::Cross(physB - > GetInertiaTensor() * Mathf::Cross(relativeB, m.collisionNormal), relativeB);
	//float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);
	
	//float cRestitution = 0.66f; // disperse some kinectic energy
	float cRestitution = 0.06f; // disperse some kinectic energy
	
	//original
	//float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);
	
	//modified assumption for no rotation,  assume angularEffect at 0 while the rest remains
	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass);
	
	Vector3 fullImpulse = Vector3{ m.collisionNormal, 0.f } * j;

	//if (!rb1 && rb2)
	//	ApplyLinearImpulse(*rb2,-fullImpulse);
	//else
	{
		if (rb1)
			ApplyLinearImpulse(*rb1, -fullImpulse);

		if (rb2)
			ApplyLinearImpulse(*rb2, fullImpulse);
	}
	//physA - > ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	//physB - > ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));
}











//fn declar
//void ResolveCollision(unsigned int A, unsigned int B, collidablePairID m);


//impluse generated by vel when they collide
//youtube: Math for Game Developers: Collision Resolution in Game Physics
void resolutionImpluseMethod(collidablePairID& m, bool& static1, bool& static2)
{
	//relative vel
	//vel rel = Vel a - Vel b

	//GOAL: apply impluse along the normal of collision, and relative vel along normal
	//rel vel alaong normal = dot product of vel rel and collision normal HAT

	//rel vel alaong normal DOT collision normal Hat == -elasticity(old rel vel DOT collision normal Hat)
	//Elasticity is also the epilson, also the coefficient of resititution
	//betweek 0 to 1, where 0 means wont bounce as much than 1
	//static or non rigidbody should be 0 since cant move

	//difference of momentum of A
	//diff of momentum = mass of A * new vel A - mass of A * old vel of A 
	//	Since impluse is the diff of momentum
	//J DOT collision normal Hat = mass of A * new vel A - mass of A * old vel of A 
	//reordering:
	//new vel = old vel + (J DOT N) / mass of A
	//where J Stand for Impluse along the normal

	//difference of momentum of B
	//diff of momentum = mass of B * new vel B - mass of B * old vel of B
	//	Since impluse is the diff of momentum
	//J DOT collision normal Hat = mass of B * new vel B - mass of B * old vel of B 
	//reordering:
	//new vel = old vel - (J DOT N) / mass of B

	//new relative vel = (vel a  - vel b) + (J DOT N) / mass A + (J DOT N) / mass B
	//N DOT new relative vel = N DOT (vel a  - vel b) + ((J DOT N) / mass A + (J DOT N) / mass B) DOT N

	//-Elasticity (old relative vel Dot N) = old RV Dot N + J Dot N(1/mass a + 1/mass b) Dot N
	//===============================
	//Goal J:
	//J = [-elasticity(old RV Dot N) - (old RV Dot N)] / [1/mass a + 1/ mass b dont N dot N]
	//J = [-(1+E)(old rv dot n) ]/ (invmass a + inv mass b)

	rigidBody2D* rb1 = nullptr;
	rigidBody2D* rb2 = nullptr;
	float restitution1 = 0.0f;
	float restitution2 = 0.0f;
	float invMass1 = 0.f;
	float invMass2 = 0.f;
	vec2 vel1{ 0.f,0.f };
	vec2 vel2{ 0.f,0.f };

	if (ecs.entityHas<rigidBody2D>(m.first))
	{
		rb1 = &ecs.getComponent<rigidBody2D>(m.first);
		restitution1 = rb1->restitution;
		invMass1 = rb1->InvMass;
		vel1 = rb1->vel;
		if (rb1->Type != rigidBody2D::Types::Static)
			static1 = false;
	}
	if (ecs.entityHas<rigidBody2D>(m.second))
	{
		rb2 = &ecs.getComponent<rigidBody2D>(m.second);
		restitution2 = rb2->restitution;
		invMass2 = rb2->InvMass;
		vel2 = rb2->vel;
		if (rb2->Type != rigidBody2D::Types::Static)
			static2 = false;
	}
	float e = min(restitution1, restitution2);

	vec2 relative_velocity = vel1 - vel2;

	float impluseMag = -(1 + e) * Dot(relative_velocity, m.collisionNormal) / (invMass1 + invMass2);
	vec2 impulseDir = m.collisionNormal;
	vec2 jn = impulseDir * impluseMag;
	/*if (ecs.entityHas<rigidBody2D>(m.first))
	{
		rb1->vel += jn * invMass1;
	}
	if (ecs.entityHas<rigidBody2D>(m.second))
	{
		rb2->vel -= jn * invMass2;
	}*/

	if (!static1 && !static2)
	{
		rb1->vel += jn * invMass1;
		rb2->vel -= jn * invMass2;
	}
	else if (static1 && !static2)
	{
		rb2->vel -= jn * invMass2;
	}
	else if (!static1 && static2)
	{
		rb1->vel += jn * invMass1;
	}

}

void positionalCorrection(collidablePairID& m, bool& static1, bool& static2)
{
	//randygual method from tutorial points or something
	transform2D* t1 = &ecs.getComponent<transform2D>(m.first);
	transform2D* t2 = &ecs.getComponent<transform2D>(m.second);
	rigidBody2D* rb1 = nullptr;
	rigidBody2D* rb2 = nullptr;
	float invMass1 = 0.f;
	float invMass2 = 0.f;
	if (ecs.entityHas<rigidBody2D>(m.first))
	{
		rb1 = &ecs.getComponent<rigidBody2D>(m.first);
		invMass1 = rb1->InvMass;
	}
	if (ecs.entityHas<rigidBody2D>(m.second))
	{
		rb2 = &ecs.getComponent<rigidBody2D>(m.second);
		invMass2 = rb2->InvMass;
	}

	//percent should be how strong the snaping is
	//float percent = 0.05f; // usually 20% to 80%
	float percent = 0.5f; // usually 20% to 80%
	//if (static1 || static2)
	//{
	//	percent = 0.01f; // for static collision, probably no need to move that much
	//}
	//slop is so that there is a bit of penetration allowed if not will always jitter
	const float slop = 0.01f; // usually 0.01 to 0.1
	
	//original correction
	//Vec2 correction = max(abs(m.penetration) - slop, 0.0f) / (invMass1 + invMass2) * percent* m.collisionNormal;
	
	//modified correction via scale testing
	Vec2 correction = max(abs(m.penetration) - slop, 0.0f) /* /(invMass1 + invMass2) */ * percent * m.collisionNormal;
	
	/////////
	//snaping style
	/////////

	vec2 posCorr1 = invMass1 * correction;
	vec2 posCorr2 = invMass2 * correction;
	if (!static1 && !static2)
	{ 
		//t1->pos.x -= posCorr1.x;
		//t1->pos.y -= posCorr1.y;
		//t2->pos.x += posCorr2.x;
		//t2->pos.y += posCorr2.y;

		if (t1->pos.x < t2->pos.x)
		{
			t1->pos.x -= posCorr1.x;
			t2->pos.x += posCorr2.x;
		}
		else
		{
			t1->pos.x += posCorr1.x;
			t2->pos.x -= posCorr2.x;
		}
		if (t1->pos.y < t2->pos.y)
		{
			t1->pos.y -= posCorr1.y;
			t2->pos.y += posCorr2.y;
		}
		else
		{
			t1->pos.y += posCorr1.y;
			t2->pos.y -= posCorr2.y;
		}
	}
	else if (static1 && !static2)
	{
		/*t2->pos.x += posCorr2.x;
		t2->pos.y += posCorr2.y;*/
		if (t2->pos.x < t1->pos.x)
			t2->pos.x -= posCorr2.x;
		else
			t2->pos.x += posCorr2.x;
		if (t2->pos.y < t1->pos.y)
			t2->pos.y -= posCorr2.y;
		else
			t2->pos.y += posCorr2.y;
	}
	else if (!static1 && static2)
	{
		/*t1->pos.x -= posCorr1.x;
		t1->pos.y -= posCorr1.y;*/
		if (t1->pos.x < t2->pos.x)
			t1->pos.x -= posCorr1.x;
		else
			t1->pos.x += posCorr1.x;
		if (t1->pos.y < t2->pos.y)
			t1->pos.y -= posCorr1.y;
		else
			t1->pos.y += posCorr1.y;
	}
}

void CollisionSystem::narrowPhase(float dt)
{
	for (auto it = currentGrid.collidablePairsIDContainer.begin(); it < currentGrid.collidablePairsIDContainer.end();)
	{
		rigidBody2D* rb1 = nullptr, *rb2 = nullptr;
		
		if (ecs.entityHas<rigidBody2D>(it->first))
		{
			rb1 = &ecs.getComponent<rigidBody2D>(it->first);
			it->hasRB1 = true;
			if (rb1->Type != rigidBody2D::Types::Static)
				it->static1 = false;
		}
		else
		{
			it->hasRB1 = false;
			it->static1 = true;
		}
		if (ecs.entityHas<rigidBody2D>(it->second))
		{
			rb2 = &ecs.getComponent<rigidBody2D>(it->second);
			it->hasRB2 = true;
			if (rb2->Type != rigidBody2D::Types::Static)
				it->static2 = false;
		}
		else
		{
			it->hasRB2 = false;
			it->static2 = true;
		}
		// END  ///////////////Setting up collidablePairsIDContainer element

		//continue if both static
		if (it->static1 && it->static2)
		{	
			++it;
			continue;
		}

		//entity 1
		unsigned int e1 = it->first;
		//auto rb1 = ecs.getComponent<rigidBody2D>(e1);
		AABB* bb1 = nullptr;
		if (ecs.entityHas<AABB>(e1))
		{
			bb1 = &ecs.getComponent<AABB>(e1);
		}
		Poly* pp1 = nullptr;
		if (ecs.entityHas<Poly>(e1))
		{
			pp1 = &ecs.getComponent<Poly>(e1);
		}

		//entity 2
		unsigned int e2 = it->second;
		//auto rb2 = ecs.getComponent<rigidBody2D>(e2);
		AABB* bb2 = nullptr;
		if (ecs.entityHas<AABB>(e2))
		{
			bb2 = &ecs.getComponent<AABB>(e2);
		}
		Poly* pp2 = nullptr;
		if (ecs.entityHas<Poly>(e2))
		{
			pp2 = &ecs.getComponent<Poly>(e2);
		}

		vec2 vel1{ 0.f, 0.f };
		vec2 vel2{ 0.f, 0.f };
		if (rb1)
		{
			//e1 have rigidbody
			vel1 = rb1->vel;
		}
		if (rb2)
		{
			//e2 have rigidbody
			vel2 = rb2->vel;
		}

		//AABB vs AABB
		if (bb1 != nullptr && bb2 != nullptr)
		{
			if (CollisionIntersection_RectRect(*bb1, vel1, *bb2, vel2, dt, *it))
			{
				//std::cout << "AABB IS COLLIDING" << std::endl;
				bb1->isOverlap += 1;
				bb2->isOverlap += 1;
				it->collided = true;
				++it;
				//NEED CALL CALLBACK HERE??

				if (printDebugInfo)
					std::cout << "NarrowPhase() collided: " << e1 << " and " << e2 << std::endl;
			}
			else
			{
				//std::cout << "AABB IS NOT COLLIDING" << std::endl;
				it = currentGrid.collidablePairsIDContainer.erase(it);
			}
		}

		//OBB vs OBB
		else if (pp1 != nullptr && pp2 != nullptr)
		{
			//if (CollisionIntersection_DynamicSAT2(*pp1, *pp2, *it))
			if (CollisionIntersection_DynamicSAT(*pp1, vel1, *pp2, vel2, *it))
			{
				it->collided = true;
				if (printDebugInfo)
					std::cout << "NarrowPhase() SAT collided: " << e1 << " and " << e2 << std::endl;
				//std::cout << "SAT IS COLLIDING" << std::endl;
				++it;
			}
			else
			{
				//std::cout << "SAT IS NOT COLLIDING" << std::endl;
				it = currentGrid.collidablePairsIDContainer.erase(it);
			}
		}
		//cases like OBB vs AABB etc
		else
		{
			//Dont handle for now
			it = currentGrid.collidablePairsIDContainer.erase(it);
		}
	}
}


void CollisionSystem::collisionResolution(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	for (collidablePairID& x : currentGrid.collidablePairsIDContainer)
	{
		if (x.collided == true)
		{
			//Skips if both static
			//Static here is either non-present rigidbody, or rigidBody2D::Types::Static
			//Data of static is set in narrowphase
			if (x.static1 && x.static2) 
				continue;

			rigidBody2D* rb1 = nullptr;
			rigidBody2D* rb2 = nullptr;

			//Updating the static-ness
			if (x.hasRB1)
			{
				rb1 = &ecs.getComponent<rigidBody2D>(x.first);
			}
			if (x.hasRB2)
			{
				rb2 = &ecs.getComponent<rigidBody2D>(x.second);
			}

			//std::cout << x.first << " is colliding with " << x.second << std::endl;

			//Skip checks(or special collision resolution) only if both have component type
			if (ecs.entityHas<Com_type>(x.first) && ecs.entityHas<Com_type>(x.second))
			{
				 Com_type* ct1 = &ecs.getComponent<Com_type>(x.first);
				 Com_type* ct2 = &ecs.getComponent<Com_type>(x.second);
				 
				 //std::cout << ct1->type << " type check " << ct2->type << std::endl;


				 //Skip collision for some type against some type
				 //player, enemies and enemy bullets pass spawners
				 if (ct1->type == Com_type::type::enemy && ct2->type == Com_type::type::spawner ||
					 ct1->type == Com_type::type::spawner && ct2->type == Com_type::type::enemy ||
					 ct1->type == Com_type::type::spawner && ct2->type == Com_type::type::enemyrange ||
					 ct1->type == Com_type::type::enemyrange && ct2->type == Com_type::type::spawner ||
					 ct1->type == Com_type::type::spawner && ct2->type == Com_type::type::player ||
					 ct1->type == Com_type::type::player && ct2->type == Com_type::type::spawner ||
					 ct1->type == Com_type::type::enemyBullet && ct2->type == Com_type::type::spawner ||
					 ct1->type == Com_type::type::spawner && ct2->type == Com_type::type::enemyBullet)
					 continue;

				//if bullet and bullet have collided, SKIP resolution
				if (ct1->type == Com_type::type::bullet && ct2->type == Com_type::type::bullet ||
					ct1->type == Com_type::type::bullet && ct2->type == Com_type::type::enemyBullet || 
					ct1->type == Com_type::type::enemyBullet && ct2->type == Com_type::type::bullet ||
					ct1->type == Com_type::type::enemyBullet && ct2->type == Com_type::type::enemyBullet)
					continue;
				//player and bullet
				if (ct1->type == Com_type::type::player && ct2->type == Com_type::type::bullet ||
					ct1->type == Com_type::type::bullet && ct2->type == Com_type::type::player)
					continue;
				//bullet and tree
				if (ct1->type == Com_type::type::obstacles && ct2->type == Com_type::type::bullet ||
					ct1->type == Com_type::type::bullet && ct2->type == Com_type::type::obstacles)
					continue;
				//Enemy Bullet and enemies
				if (ct1->type == Com_type::type::enemyBullet && ct2->type == Com_type::type::enemy ||
					ct1->type == Com_type::type::enemy && ct2->type == Com_type::type::enemyBullet ||
					ct1->type == Com_type::type::enemyBullet && ct2->type == Com_type::type::enemyrange ||
					ct1->type == Com_type::type::enemyrange && ct2->type == Com_type::type::enemyBullet)
					continue;
			}
			
			//resolutionImpluseMethod(x, x.static1, x.static2);
			//positionalCorrection(x, x.static1, x.static2);
			ImpulseResolveCollision(x);
		}
	}
}

std::vector<unsigned int> CollisionSystem::getEntityCollidedWith(unsigned int id)
{
	std::vector<unsigned int> result;

	for (auto& x : currentGrid.collidablePairsIDContainer)
	{
		if (x.first == id)
			result.push_back(x.second);
		else if (x.second == id)
			result.push_back(x.first);
	}
	return result;
}

bool CollisionSystem::checkCollisionBetween(unsigned int id1, unsigned int id2)
{
	for (auto& x : currentGrid.collidablePairsIDContainer)
	{
		if (x.first == id1)
		{
			if (x.second == id2)
			{
				return true;
			}
		}
		else if (x.second == id1)
		{
			if (x.first == id2)
			{
				return true;
			}
		}

	}

	return false;
}
//
//void ResolveCollision(unsigned int A, unsigned int B, collidablePairID m)
//{
//	rigidBody2D& rb1 = ecs.getComponent<rigidBody2D>(A);
//	rigidBody2D& rb2 = ecs.getComponent<rigidBody2D>(B);
//
//
//	// Calculate relative velocity
//	Vec2 rv = rb2.vel - rb1.vel;
//
//	// Calculate relative velocity in terms of the normal direction
//	float velAlongNormal = Mathf::Dot(rv, m.collisionNormal);
//
//	// Do not resolve if velocities are separating
//	if (velAlongNormal > 0)
//		return;
//
//	// Calculate restitution
//	float e;
//	(rb1.restitution < rb2.restitution) ? e = rb1.restitution : e = rb2.restitution;
//	// Calculate impulse scalar
//	float j = -(1 + e) * velAlongNormal;
//	j /= 1 / rb1.Mass + 1 / rb2.Mass;
//
//	// Apply impulse
//	Vec2 impulse = j * m.collisionNormal;
//	rb1.vel-= 1 / rb1.Mass * impulse;
//	rb2.vel += 1 / rb2.Mass * impulse;
//}
//
////Positional correction is: 
////move each object along the collision normal \(n\) by a percentage of the penetration depth
//void PositionalCorrection(unsigned int A, unsigned int B)
//{
//	auto& t1 = ecs.getComponent<transform2D>(A);
//	auto& t2 = ecs.getComponent<transform2D>(B);
//
//	auto& r1 = ecs.getComponent<rigidBody2D>(A);
//	auto& r2 = ecs.getComponent<rigidBody2D>(B);
//
//	const float percent = 0.2; // usually 20% to 80%
//	const float slop = 0.01; // usually 0.01 to 0.1
//
//	Vec2 correction = max(penetration - k_slop, 0.0f) / (r1.InvMass + r2.InvMass) * percent* n;
//	
//	t1.pos -= r1.InvMass * correction;
//	t2.pos += r2.InvMass * correction;
//}

void CollisionSystem::collidersDebugDraw(bool isEditorMode)
{
	//for (auto const& e : getEntitiesRegistered())
	for (auto const& e : AllColliderEntity)
	{
		if (ecs.entityHas<Com_Health>(e))
		{
			if (ecs.getComponent<Com_Health>(e).isActive == false)
				continue;
		}

		if (ecs.entityHas<transform2D>(e))
		{
			auto& tc = ecs.getComponent<transform2D>(e);

			//draw AABB
			if (ecs.entityHas<AABB>(e))
			{
				PhysicsDebugging::drawDebugAABB(tc, ecs.getComponent<AABB>(e), isEditorMode);
			}

			//draw polygon
			if (ecs.entityHas<Poly>(e))
			{
				PhysicsDebugging::drawDebugPolygon(tc, ecs.getComponent<Poly>(e), isEditorMode);
			}
		}
	}
	currentGrid.drawGridLine();
}

CollisionSystem::CollisionSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{
}
//CollisionSystem::CollisionSystem(CollisionSystem& lhs) {}
CollisionSystem::~CollisionSystem() {}

void CollisionSystem::combineColliderRegisteredEntity()
{
	//NOTE!! Need to watch out for duplicates, when one entity have both aabb and poly, not handled yet

	//Save poly registered entity
	RegisteredEntityWithPoly.clear();
	//RegisteredEntityWithPoly = collisionPolySysInstance->getEntitiesRegistered();
	auto& t1 = collisionPolySystemsPtr->getEntitiesRegistered();
	RegisteredEntityWithPoly.insert(RegisteredEntityWithPoly.end(), t1.begin(), t1.end());
	
	//Combining all collider entity
	AllColliderEntity.clear();
	//AllColliderEntity = collisionSysInstance->getAllRegisteredEntity();
	auto& t2 = collisionSysInstance->getEntitiesRegistered();
	AllColliderEntity.insert(AllColliderEntity.end(), t2.begin(), t2.end());
	AllColliderEntity.insert(AllColliderEntity.end(), t1.begin(), t1.end());
}

std::vector<entityID>& CollisionSystem::getAllRegisteredEntity()
{
	return AllColliderEntity;
}

void CollisionSystem::update(float dt)
{
	//probabbly not used 
	_Unreferenced_parameter_(dt);
	//for (auto const& e : getEntitiesRegistered())
	for (auto const& e : AllColliderEntity) 
	{
		std::cout << "ICollider Works entity: " << e << std::endl;
	}
}

CollisionSystem& CollisionSystem::GetInstance()
{
	static CollisionSystem instance;
	return instance;
}






















void CollisionSystem::updateAABB(const transform2D& trans, AABB& aabb)
{
	const float bBoxSize = 0.5;
	float absScaleX = bBoxSize * trans.scale.x * aabb.scale.x;
	float absScaleY = bBoxSize * trans.scale.y * aabb.scale.y;
	aabb.max.x = (absScaleX)+trans.pos.x + aabb.offset_pos.x;
	aabb.max.y = (absScaleY)+trans.pos.y + aabb.offset_pos.y;

	aabb.min.x = trans.pos.x + aabb.offset_pos.x - (absScaleX);
	aabb.min.y = trans.pos.y + aabb.offset_pos.y - (absScaleY);

}

Poly CollisionSystem::AABBToPoly(AABB& aabb)
{
	Poly polygon;
	polygon.models.push_back({ 0.5f, 0.5f });
	polygon.models.push_back({ -0.5f, 0.5f });
	polygon.models.push_back({ -0.5f, -0.5f });
	polygon.models.push_back({ 0.5f, -0.5f });

	polygon.points.push_back({ aabb.max.x, aabb.max.y });
	polygon.points.push_back({ aabb.min.x, aabb.max.y });
	polygon.points.push_back({ aabb.min.x, aabb.min.y });
	polygon.points.push_back({ aabb.min.x, aabb.min.y });
	return polygon;
}

void CollisionSystem::UpdatePOLY_WithRotation(const transform2D& trans, Poly& poly)
{
	float cosRot = cos(trans.rotation + poly.angle_offset);
	float sinRot = sin(trans.rotation + poly.angle_offset);

	Matrix3 scale{
		poly.scale_ratio.x * trans.scale.x, 0.f, 0.f,
		0.f, poly.scale_ratio.y * trans.scale.y, 0.f,
		0.f, 0.f, 1.f
	};

	Matrix3 rotate{
		cosRot, -sinRot, 0.f,
		sinRot, cosRot, 0.f,
		0.f, 0.f, 1.f
	};

	Matrix3 translate{
		1.f, 0.f, poly.offset_pos.x + trans.pos.x,
		0.f, 1.f, poly.offset_pos.y + trans.pos.y,
		0.f, 0.f, 1.f
	};
	Matrix3 RT = rotate * scale;
	Matrix3 SRT = translate * RT;
	//Matrix3 SRT = translate * rotate * scale;

	//INPUT: VECTOR OF POINTS, rotation value
	for (size_t i = 0; i < poly.points.size(); ++i)
	{
		poly.points[i] = SRT * Vector<3, float>{poly.models[i], 1.f};

		//leaving it here in case matrix fails for now
		/*float sx = trans.scale.x * poly.models[i].x;
		float sy = trans.scale.y * poly.models[i].y;
		float srx = cosRot * sx + -sinRot * sy;
		float sry = sinRot * sx + cosRot * sy;
		poly.points[i].x = srx + trans.pos.x;
		poly.points[i].y = sry + trans.pos.y;*/
	}
}

CollisionSystem::CollisionSystem()
{
}

std::shared_ptr<CollisionSystem> CollisionSystem::getInstance()
{
	if (collisionSystemsPtr == nullptr)
	{
		collisionSystemsPtr = ecs.registerSystem<CollisionSystem>();
		ecs.setSystemComponentRequirement<CollisionSystem, transform2D>();
		ecs.setSystemComponentRequirement<CollisionSystem, AABB>();
		CollisionPolygonSystem::getInstance();
		return collisionSystemsPtr;
	}
	else
		return collisionSystemsPtr;
}

void CollisionSystem::init()
{
	ecs.setSystemComponentRequirement<CollisionSystem, transform2D>();
	ecs.setSystemComponentRequirement<CollisionSystem, AABB>();
	CollisionPolygonSystem::getInstance();
}


void CollisionSystem::broadPhase()
{
	//currentGrid.printDebugInfoToConsole = true;
	//currentGrid.setupGrid(getEntitiesRegistered());
	currentGrid.setupGrid(AllColliderEntity);
	currentGrid.checkCollidables();
}

void CollisionSystem::updateAllColliders()
{
	//for (auto const& e : getEntitiesRegistered())
	for (auto const& e : AllColliderEntity)
	{
		if (ecs.entityHas<transform2D>(e))
		{
			if (ecs.entityHas<AABB>(e))
			{
				CollisionSystem::updateAABB(ecs.getComponent<transform2D>(e), ecs.getComponent<AABB>(e));
			}

			// update for poly 
			if (ecs.entityHas<Poly>(e))
			{
				CollisionSystem::UpdatePOLY_WithRotation(ecs.getComponent<transform2D>(e), ecs.getComponent<Poly>(e));
			}
		}
	}
}

CollisionPolygonSystem::CollisionPolygonSystem(bool requireComponentMatch) : 
	systemBase(requireComponentMatch)
{
}

std::shared_ptr<CollisionPolygonSystem> CollisionPolygonSystem::getInstance()
{
	if (collisionPolySystemsPtr == nullptr)
	{
		collisionPolySystemsPtr = ecs.registerSystem<CollisionPolygonSystem>();
		ecs.registerComponentType<Poly>();
		ecs.setSystemComponentRequirement<CollisionPolygonSystem, transform2D>();
		ecs.setSystemComponentRequirement<CollisionPolygonSystem, Poly>();
		//Test 
		//AABB aabb;
		//ecs.addComponent<AABB>(aabb, "E Test");
		//Poly p;
		//ecs.addComponent<Poly>(p, "E Test");
		return collisionPolySystemsPtr;
	}
	else
		return collisionPolySystemsPtr;
}
