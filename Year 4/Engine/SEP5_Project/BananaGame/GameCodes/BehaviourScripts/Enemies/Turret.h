#pragma once
#include <BananaEngine.h>

namespace TH
{

	class Turret : public BE::IScript
	{
		/*
			Constants
		*/
		//Max number of bullets
		static constexpr int maxBullets = 5;

		//Turret rate of fire, every this much time, shoot 1 bullet
		static constexpr float rateOfFire = 0.5f;

		//How long it fires for: +/- 20% of this time
		static constexpr float fireTime = 2.0f;

		//How long it stays inactive
		static constexpr float idleTime = 5.0f;


		/*
			Other properties
		*/
		//Bullets owned by this turret, the ones in used at pushed to the back
		std::array<BE::EntityID, maxBullets> m_bullets;
		size_t bulletsInUse = 0;

		//Other properties
		float currStateTime = 0.0f;		//How long left to it's firing/idling
		float currFireTime = 0.0f;		//How long since the last bullet fired
		float rateOfRotation = 0.0f;	//How fast it is rotating, in degree, aka angular velocity, can be serialised
		bool isFiring = false;			//Is it firing or not
		float detectionRange = 30.0f;	//If the player is further than this distance then don't update the turret

		//Reference to the player
		BE::BEID playerID;

	public:
		void Awake() override;				//Create the bullets
		void Init() override;				//Init the bullets and the turret
		void Update(double dt) override;	

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	class TurretBullet : public BE::IScript
	{
	public:
		/*
			Constants
		*/
		//Bullet damage dealt to player
		static constexpr float bulletDamage = 1.0f;
		
		//Bullet speed, to be set when turret shoots
		static constexpr float bulletSpeed = 20.0f;

		//Bullet knock back power
		static constexpr float bulletKnockBack = 35.0f;
	private:
		bool isDestroyed = false;

	public:
		void Update(double dt) override;
		void OnCollision(BE::BEID other) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}
