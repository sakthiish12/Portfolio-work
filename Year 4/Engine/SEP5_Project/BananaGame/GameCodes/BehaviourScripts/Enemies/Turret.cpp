#include "Turret.h"
#include "../Character/CharacterScript.h"

namespace TH
{
	void Turret::Awake()
	{
		//Create prefabs
		BE::EntityID e = BE::ECS->SpawnPrefab(BE::AssetMgr->GetAssetID("../Assets/Prefabs/TurretBullet.prefab"));
		m_bullets[0] = e;

		//Then clone
		for (int i = 1; i < maxBullets; ++i)
		{
			//Create prefab and update m_bullets
			BE::EntityID clonedE = BE::ECS->CloneEntity(e);
			m_bullets[i] = clonedE;
		}
	}

	void Turret::Init()
	{
		for (size_t i = 0; i < m_bullets.size(); ++i)
		{
			//Turn off
			BE::ECS->GetComponent<BE::EntityInfo>(m_bullets[i]).SetFlag(BE::EntityFlag::active, false);

			//Call init for the scripts
			for (const auto& script : BE::ECS->GetComponent<BE::GameLogic>(m_bullets[i]).GetAllScripts())
				script->Init();
		}

		bulletsInUse = 0;
		currStateTime = 0;
		isFiring = false;

		playerID = BE::GLS->GetPlayerID();
	}

	void Turret::Update(double dt)
	{
		//only update if player is sufficiently close 
		if (BE::ECS->GetComponent<BE::Transform>(id).GetPosition().SqDistBetween(BE::ECS->GetComponent<BE::Transform>(playerID).GetPosition()) 
			> detectionRange * detectionRange)
			return;

		//Decrease remaining time for the current state
		currStateTime -= (float)dt;

		//Rotate the rig
		BE::EntityID rigEntity = BE::ECS->GetComponent<BE::Transform>(id).GetParent();
		Assert(rigEntity != BE::EntityID(0), "This entity %llu must have a parent (Turret right)", id);
		BE::Vec3f rigRotation = BE::ECS->GetComponent<BE::Transform>(rigEntity).GetEulerRotation();
		rigRotation.y += rateOfRotation * (float)dt;
		BE::ECS->GetComponent<BE::Transform>(rigEntity).SetEulerRotation(rigRotation);

		//Idling
		if (!isFiring)
		{
			//Reached 0s, start shooting
			if (currStateTime <= 0.0f)
			{
				//Init all the bullets
				for (size_t i = 0; i < m_bullets.size(); ++i)
				{
					//Turn off all the bullets
					BE::ECS->GetComponent<BE::EntityInfo>(m_bullets[i]).SetFlag(BE::EntityFlag::active, false);

					//Call init for the scripts
					for (const auto& script : BE::ECS->GetComponent<BE::GameLogic>(m_bullets[i]).GetAllScripts())
						script->Init();
				}
				bulletsInUse = 0;

				//Set the amount of time to shoot
				currStateTime += BE::Rand::Random<float>(fireTime * 0.8f, fireTime * 1.2f);
				currFireTime = rateOfFire;
				//Start firing
				isFiring = true;
			}
		}
		//Firing
		else
		{
			//If reached 0s, stop shooting
			if (currStateTime <= 0.0f)
			{
				isFiring = false;
				currStateTime += BE::Rand::Random<float>(idleTime * 0.8f, idleTime * 1.2f);
				return;
			}

			currFireTime -= (float)dt;

			//Time to fire a bullet, and if there's any remaining
			if (currFireTime <= 0.0f && bulletsInUse < m_bullets.size())
			{
				//Set bullet position to be position of the turret spawn point
				Assert(BE::ECS->GetComponent<BE::Transform>(id).GetChildren().size() == 1, "Must only have 1 children (bullet spawn point)");
				BE::EntityID spawnpointEn = BE::ECS->GetComponent<BE::Transform>(id).GetChildren().front();
				BE::Vec3f spawnpointPos = BE::ECS->GetComponent<BE::Transform>(spawnpointEn).GetPosition();
				BE::ECS->GetComponent<BE::Transform>(m_bullets[bulletsInUse]).SetPosition(spawnpointPos);

				//Set the velocity of the bullet
				const BE::Quatf& turretRot = BE::ECS->GetComponent<BE::Transform>(id).GetRotation();
				BE::Vec3f velocity = -BE::Vec3f(2 * (turretRot.GetI() * turretRot.GetK() + turretRot.GetJ() * turretRot.GetR()),
											   2 * (turretRot.GetJ() * turretRot.GetK() - turretRot.GetI() * turretRot.GetR()),
											   1 - (2 * turretRot.GetI() * turretRot.GetI()) - (2 * turretRot.GetJ() * turretRot.GetJ())) * TurretBullet::bulletSpeed;
				BE::ECS->GetComponent<BE::Physics>(m_bullets[bulletsInUse]).SetLinearVelocity(velocity);

				//Turn this entity on
				BE::ECS->GetComponent<BE::EntityInfo>(m_bullets[bulletsInUse]).SetFlag(BE::EntityFlag::active, true);
				BE::ECS->GetComponent<BE::Physics>(m_bullets[bulletsInUse]).SetIsTrigger(false);

				//Play TurretShooting.wav sound
				BE::ECS->GetComponent<BE::AudioSource>(id).Play(AssetID(15300463267994365381));

				currFireTime += rateOfFire;
				++bulletsInUse;
			}
		}
	}


	void TurretBullet::Update(double dt)
	{
		if (isDestroyed)
		{
			BE::ECS->GetComponent<BE::EntityInfo>(id).SetFlag(BE::EntityFlag::active, false);
			BE::ECS->GetComponent<BE::Physics>(id).SetIsTrigger(true);
			//BE::ECS->GetComponent<BE::Transform>(id).SetPosition(Vec3f(0, -100, 0));
			isDestroyed = false;
		}
	}
	
	void TurretBullet::OnCollision(BE::BEID other) 
	{

		auto playerID = BE::GLS->GetPlayerID();

		//If other object is a trigger then ignore it
		if (BE::ECS->GetComponent<BE::Physics>(other).GetIsTrigger() && other != playerID)
			return;

		//If collided with player then damage player
		if (other == playerID)
		{
			//Damage player - don't return here because need to run next line
			auto script = BE::ECS->GetComponent<BE::GameLogic>(playerID).GetScriptDerived<CharacterScript>();
			script->DamageHealth(bulletDamage);
			script->KnockBackCharacter(BE::ECS->GetComponent<BE::Physics>(id).GetLinearVelocity(), bulletKnockBack);
		}

		//Turn off the bullet if collided to other things
		isDestroyed = true;
	}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;

	registration::class_<Turret>(RegistrationNameCreator<Turret>())
		.constructor()
		.property("RateOfRotation", &Turret::rateOfRotation)
		.property("DetectionRange", &Turret::detectionRange);

	registration::class_<TurretBullet>(RegistrationNameCreator<TurretBullet>())
		.constructor();
}