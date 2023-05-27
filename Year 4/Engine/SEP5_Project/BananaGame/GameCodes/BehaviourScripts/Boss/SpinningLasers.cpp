
#include "SpinningLasers.h"

namespace TH
{
	using namespace BE;

	void SpinningLasers::Awake()
	{
		SpawnPos = ECS->GetComponent<Transform>(id).GetPosition();
	}
	void SpinningLasers::Init()
	{
		// four local positions of the laser source
		float local_offset = 0.65f;
		sLasers_LocalPositions.emplace_back(Vec3f(0.f, 0.f, local_offset));
		sLasers_LocalPositions.emplace_back(Vec3f(0.f, 0.f, -local_offset));
		sLasers_LocalPositions.emplace_back(Vec3f(local_offset, 0.f, 0.f));
		sLasers_LocalPositions.emplace_back(Vec3f(-local_offset, 0.f, 0.f));

		// initialize the laser attacks class
		for (int i = 0; i < sLasers_LocalPositions.size(); i++)
		{
			sLasersAttacks.emplace_back(ChargeBeamSkill(
				0.0f, // CD_duration
				10.0f, // Damage_Multplr
				2.0f, // Charge_duration
				300.f, // Speed
				80.f, // RayLength
				100.f, // Cast_Range
				0.1f // Radius
			));
		}

		// load laser beam and laser source from prefab or find the entities.
		auto LaserBeam_PrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/LaserBeam.prefab");
		auto LaserSource_PrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/LaserSource.prefab");
		auto name = ECS->GetComponent<EntityInfo>(id).GetName();
		float LaserBlast_duration = sStartEndSpinning.duration + sActualSpinning.duration;
		bool useSerializedID = sLaserBlasts.size() == sLasersAttacks.size() && sLaserSources.size() == sLasersAttacks.size();
		for (int i = 0; i < sLasersAttacks.size(); i++)
		{
			// Laser Beam prefab
			EntityID laserBlastID;
			if (useSerializedID) {
				laserBlastID = sLaserBlasts[i];
			}
			else {
				std::stringstream ss;
				ss << name << " LaserBeam " << i;
				auto find_entity = ECS->FindEntitiesByName(ss.str(), 1);
				if (!find_entity.empty()) laserBlastID = find_entity.front();
				else {
					laserBlastID = ECS->SpawnPrefab(LaserBeam_PrefabID);
					ECS->GetComponent<EntityInfo>(laserBlastID).SetName(ss.str());
				}
				sLaserBlasts.emplace_back(laserBlastID);
			}

			// Laser Source prefab
			EntityID laserSourceID;
			if (useSerializedID) {
				laserSourceID = sLaserSources[i];
			}
			else {
				std::stringstream ss2;
				ss2 << name << " LaserSource " << i;
				auto find_entity = ECS->FindEntitiesByName(ss2.str(), 1);
				if (!find_entity.empty()) laserSourceID = find_entity.front();
				else {
					laserSourceID = ECS->SpawnPrefab(LaserSource_PrefabID);
					ECS->GetComponent<EntityInfo>(laserSourceID).SetName(ss2.str());
				}
				sLaserSources.emplace_back(laserSourceID);
			}

			// Init Laser stats.
			sLasersAttacks[i].cCharge_size_multiplr = sChargeSizeM;
			sLasersAttacks[i].cBlast_duration = LaserBlast_duration;
			sLasersAttacks[i].InitChargeBeam(id, sLasers_LocalPositions[i], laserBlastID, laserSourceID, sBase_Attack, "Boss");
		}

		Vec3f size = ECS->GetComponent<Physics>(id).GetShapeSize();
		sFixedRotPos = ECS->GetComponent<Transform>(id).GetPosition();
		sVolume = size.x * size.y * size.z;
	}
	void SpinningLasers::Respawn()
	{
		ECS->GetComponent<Transform>(id).SetPosition(SpawnPos);

		CancelLasers();

		sStartEndSpinning.ResetTimer();
		sActualSpinning.ResetTimer();
		sMoveTimer.ResetTimer();

		ECS->GetComponent<Physics>(id).SetLinearVelocity(Vec3f());
		ECS->GetComponent<Physics>(id).SetAngularVelocity(Vec3f());

		isSlowingDown = false;
		sCanStartSpinning = false;
		sCanStartLasering = false;
	}
	void SpinningLasers::Update(double dt)
	{
		UpdateTimers((float)dt);

		//if (Input->IsKeyPressed(Key::_B)) {
		//	StartLasering();
		//}

		LasersUpdate((float) dt);
		SpinningUpdate((float) dt);
		MovementUpdate((float) dt);

		// fix spinner to not rotate off y axis.
		sFixedRotPos.y = ECS->GetComponent<Transform>(id).GetPosition().y;
		ECS->GetComponent<Transform>(id).SetPosition(sFixedRotPos);
	} 

	// update all timers
	void SpinningLasers::UpdateTimers(float dt)
	{
		sStartEndSpinning.update(dt);
		sActualSpinning.update(dt);
		sMoveTimer.update(dt);
		/*for (int i = 0; i < sLasersAttacks.size(); i++)
		{
			sLasersAttacks[i].UpdateCoolDown(dt);
		}*/
	}

	void SpinningLasers::LasersUpdate(float dt)
	{
		for (int i = 0; i < sLasersAttacks.size(); i++)
		{
			if (sLasersAttacks[i].IsChargeStart) {
				sLasersAttacks[i].UpdateCharging(dt);
			}

			if (!sLasersAttacks[i].IsOnCoolDown() && sCanStartLasering && !sLasersAttacks[i].IsInAnimation()) {
				sLasersAttacks[i].UpdateCharging(dt);
				sLasersAttacks[i].IsChargeStart = true;

				if(i == sLasersAttacks.size()-1)
					sCanStartLasering = false;
			}

			Vec3f temp_dir;
			if (sLasersAttacks[i].IsFiring() || sLasersAttacks[i].CanCloseBeam)
			{
				Mat3f rotMatrix;
				rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
				temp_dir = rotMatrix * sLasersAttacks[i].cLocalPosition * sLasersAttacks[i].cRayLength;
			}
			if (sLasersAttacks[i].UpdateBlasting(temp_dir, dt)) {
				StartSpinning();
			}
			sLasersAttacks[i].UpdateEndCharge(temp_dir, dt);
		}
	}

	void SpinningLasers::SpinningUpdate(float dt)
	{
		Vec3f curr_AnglVel = ECS->GetComponent<Physics>(id).GetAngularVelocity();
		ECS->GetComponent<Physics>(id).SetAngularVelocity(sSpinAxis * curr_AnglVel.Mag());
		ECS->GetComponent<Physics>(id).SetLinearVelocity(Vec3f(0.f,0.f,0.f));

		if (!sStartEndSpinning.IsTimerRunning() && !sActualSpinning.IsTimerRunning())
		{
			// Spinner slowing down.
			if (sActualSpinning.ended)
			{
				sStartEndSpinning.start = true;
				isSlowingDown = true;
			}
			// start spinning, if not slowing down
			else if (sCanStartSpinning && !isSlowingDown)
			{
				sStartEndSpinning.start = true;
				sCanStartSpinning = false;
			}
		}
		// charging up spinning (adding torque)
		if (sStartEndSpinning.IsTimerRunning() && !isSlowingDown)
		{
			ECS->GetComponent<Physics>(id).AddTorque(sSpinAxis * sSpinForce * sVolume * dt);
		}
		if (sStartEndSpinning.ended)
		{
			// when starting top speed spinning, get the top speed.
			if (!isSlowingDown) {
				sActualSpinning.start = true;
				sTopAngularVelocity = ECS->GetComponent<Physics>(id).GetAngularVelocity();
			}
			else {
				isSlowingDown = false;
			}
		}
		// Fix angular velocity at top speed after finish charging up.
		if (sActualSpinning.IsTimerRunning()) {
			ECS->GetComponent<Physics>(id).SetAngularVelocity(sTopAngularVelocity);
		}

	}

	void SpinningLasers::CancelLasers()
	{
		for (int i = 0; i < sLasersAttacks.size(); i++) {
			sLasersAttacks[i].CancelCharge();
		}
	}

	void SpinningLasers::MoveAndSpinToPosition(Vec3f const& pos, Vec3f const& rotSpeed, float duration)
	{
		sSavedEndPos = pos;
		Vec3f distvec = pos - ECS->GetComponent<Transform>(id).GetPosition();
		Vec3f dirNorm = distvec.Normalised();
		sMoveVelocity = dirNorm * (distvec.Mag() / duration);
		sMoveTimer.duration = duration;
		sMoveTimer.StartTimer();
		sRotationVelocity = rotSpeed;
	}

	void SpinningLasers::MovementUpdate(float dt)
	{
		if (sMoveTimer.IsTimerRunning()) {
			ECS->GetComponent<Physics>(id).SetLinearVelocity(sMoveVelocity);
			ECS->GetComponent<Physics>(id).SetAngularVelocity(sRotationVelocity);
		}
		if (sMoveTimer.IsTimerEnded()) {
			ECS->GetComponent<Physics>(id).SetLinearVelocity(Vec3f());
			ECS->GetComponent<Physics>(id).SetAngularVelocity(Vec3f());
			ECS->GetComponent<Transform>(id).SetPosition(sSavedEndPos);
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<SpinningLasers>(RegistrationNameCreator<SpinningLasers>())
		.constructor()
		.property("Spin Force", &SpinningLasers::sSpinForce)
		.property("Spin Axis", &SpinningLasers::sSpinAxis)
		.property("Spinning Timer", &SpinningLasers::sActualSpinning)
		.property("Spin Charge Up Timer", &SpinningLasers::sStartEndSpinning)
		.property("Laser Sources ID", &SpinningLasers::sLaserSources)
		.property("Laser Blasts ID", &SpinningLasers::sLaserBlasts);
}