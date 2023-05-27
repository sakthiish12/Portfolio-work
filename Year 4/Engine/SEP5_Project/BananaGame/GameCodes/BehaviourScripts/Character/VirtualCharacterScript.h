
#pragma once
#ifndef	VIRTUAL_CHARACTER_H
#define VIRTUAL_CHARACTER_H

#include <BananaEngine.h>
namespace TH
{
	using namespace BE;

	class VirtualCharacter : public IScript
	{
		// Character size
		float	cCharacterHeightStanding = 2.0f;
		float	cCharacterRadiusStanding = 0.6f;
		float	cMinStepForward = 0.15f;
		Vec3f cStepUpHeight = Vec3f(0.0f, 0.4f, 0.0f);
		// settings
		static inline float		cMaxSlopeAngle = BE::Math::DegToRad<float>(60.0f);
		static inline float		cMaxStrength = 100.0f;
		static inline float		cCharacterPadding = 0.02f;
		static inline float		cPenetrationRecoverySpeed = 1.0f;
		static inline float		cPredictiveContactDistance = 0.1f;
		static inline bool		cEnableWalkStairs = true;

		BECharacterListener cListener;
		Vec3f cPushDir;
		bool cPushChar = false;
	public:	

		VirtualCharacter() = default;
		VirtualCharacter(const VirtualCharacter& in) = default;
		// The 'player' character
		JPH::Ref<BECharacterVirtual>	cCharacter;
		// Character collider to check
		//JPH::Ref<JPH::Character>		cChar_collider;
		JPH::RefConst<JPH::Shape>		cStandingShape;

		inline void PushCharacter(Vec3f const& dir)
		{
			cPushDir = dir;
			cPushChar = true;
		}

		inline void SetShape(JPH::Shape const* inShape)
		{
			float maxPene = 1.5f * PhySys->GetJoltSystem()->GetPhysicsSettings().mPenetrationSlop;
			cCharacter->SetShape(inShape, maxPene,
				PhySys->GetJoltSystem()->GetDefaultBroadPhaseLayerFilter(PhysicLayers::MOVING), PhySys->GetJoltSystem()->GetDefaultLayerFilter(PhysicLayers::MOVING),
				{ }, PhySys->GetAllocator());
			//PhySys->GetBodyInterface().SetShape(ECS->GetComponent<Physics>(id).GetBodyID(), inShape, false, JPH::EActivation::Activate);
		}

		inline Vec3f GetColliderFrontalPos(float forward_dist = 0.f)
		{
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f frontal = ECS->GetComponent<Transform>(id).GetPosition() + rotMatrix * (Vec3f::Forward() * (forward_dist + cCharacter->GetShape()->GetLocalBounds().GetExtent().GetZ()));
			return frontal;
		}

	private:

		void Awake();
		

		void Update(double dt);
		
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}
#endif