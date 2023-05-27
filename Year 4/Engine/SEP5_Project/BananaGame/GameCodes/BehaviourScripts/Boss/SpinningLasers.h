#pragma once

#include <BananaEngine.h>
#include "../Character/BaseSkill.h"

namespace TH
{
	using namespace BE;

	class SpinningLasers : public IScript
	{
		std::vector<ChargeBeamSkill> sLasersAttacks;
		std::vector<EntityID> sLaserSources;
		std::vector<EntityID> sLaserBlasts;
		std::vector<Vec3f> sLasers_LocalPositions;
		Timer sStartEndSpinning = Timer(3.f);
		Timer sActualSpinning = Timer(5.f);
		Vec3f sSpinAxis= Vec3f().Up();
		Vec3f sTopAngularVelocity;
		Vec3f sSavedEndPos ;
		Vec3f sFixedRotPos ;
		Vec3f SpawnPos;
		float sSpinForce = 40000.f;
		float sVolume;
		float sBase_Attack = 10.f;
		float sChargeSizeM = 1.15f;
		bool isSlowingDown = false;
		bool sCanStartSpinning = false;
		bool sCanStartLasering = false;

		// movement
		Vec3f sMoveVelocity;
		Vec3f sRotationVelocity;
		Timer sMoveTimer = Timer();
		
		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		void UpdateTimers(float dt);
		void LasersUpdate(float dt);
		void SpinningUpdate(float dt);
		void MovementUpdate(float dt);
		inline void StartSpinning() { sCanStartSpinning = true; }
	public:
		void Respawn();
		void MoveAndSpinToPosition(Vec3f const& pos, Vec3f const& rotSpeed, float duration);
		void CancelLasers();
		inline size_t GetLaserCount() { return sLasersAttacks.size(); }
		inline void StartLasering() { sCanStartLasering = true; }
		inline void SetBaseAttack(float in) { sBase_Attack = in; }
		inline bool HasStoppedSpinning() const { return !sStartEndSpinning.IsTimerRunning() && !sActualSpinning.IsTimerRunning() && !isSlowingDown; }

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}