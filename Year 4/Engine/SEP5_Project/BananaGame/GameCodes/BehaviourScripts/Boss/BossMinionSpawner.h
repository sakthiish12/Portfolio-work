#pragma once

#include <BananaEngine.h>
#include "../Misc/Timers.h"
namespace TH
{
	using namespace BE;
	enum class SpawnType
	{
		MeleeRobot,
		RangedRobot,
		BananaCrate,
		MilkBottle,
		TotalTypes
	};

	struct SpawnBundle
	{
		std::vector<AssetID> prefabIDs; // index 0 is parent rest is children
		uint8_t spawnCount;
		SpawnType spawnType;
	};

	class BossMinionSpawner : public IScript
	{
		std::vector<SpawnBundle> SpawnTypes;
		std::vector<EntityID> MinionsActive;
		std::unordered_map<EntityID, Vec3f> RecentlySpawned;
		std::vector<EntityID> Doors;
		std::vector<EntityID> LightsSpawned;
		const Vec3f behindDoorDir = Vec3f().Backward();
		Timer doorOpened = Timer();
		Timer tossFinish = Timer(1.f);
		Timer canSpawnAgain;
		const size_t total_doors = 24;
		const size_t maxLightSpawnAllowed = 10;
		float doorOpenDuration = 2.f;
		float tossSpawnForce = 10.f;
		uint8_t num_of_DoorsOpen = 6;
		uint8_t maxConsumableSpawnPerRound = 2;

		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		void UpdateTimers(float dt);

		void SpawnBundleAtDoor(SpawnBundle const& bundle, EntityID DoorID);

	public:
		inline void SetNumDoorsOpen(uint8_t in) { 
			num_of_DoorsOpen = Math::Clamp((uint8_t)0, (uint8_t)total_doors, in);
		}
		 void KillAllRemainingSpawns();
		 void DestroyRemainingConsumables();
		inline void SetNumberOfDoorOpen(uint8_t in) { num_of_DoorsOpen = in; }
		inline uint8_t GetNumberOfDoorOpen() const { return num_of_DoorsOpen; }

		void OpenDoorsAndSpawnRandom(uint8_t num_of_doors_to_Open);
		void KnockBackMinionsActive(float kb);

		void Respawn();
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}