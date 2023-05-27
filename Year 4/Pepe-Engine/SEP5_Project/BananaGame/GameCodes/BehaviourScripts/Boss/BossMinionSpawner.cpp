
#include "BossMinionSpawner.h"
#include "../Environment/Door.h"
#include "../Enemies/EnemyScript.h"

namespace TH
{
	using namespace BE;

	void BossMinionSpawner::Awake()
	{
		SpawnTypes.emplace_back(SpawnBundle(
			{AssetMgr->GetAssetID("../Assets/Prefabs/MeleeRobot.prefab"),
			AssetMgr->GetAssetID("../Assets/Prefabs/RobotModel.prefab")},
			1 , SpawnType::MeleeRobot
		));

		SpawnTypes.emplace_back(SpawnBundle(
			{ AssetMgr->GetAssetID("../Assets/Prefabs/RangedRobot.prefab"),
			AssetMgr->GetAssetID("../Assets/Prefabs/RangedModel.prefab") },
			1, SpawnType::RangedRobot
		));

		SpawnTypes.emplace_back(SpawnBundle(
			{ AssetMgr->GetAssetID("../Assets/Prefabs/BananaCrate1.prefab"),
			AssetMgr->GetAssetID("../Assets/Prefabs/BananaLight.prefab") },
			1, SpawnType::BananaCrate
		));

		SpawnTypes.emplace_back(SpawnBundle(
			{ AssetMgr->GetAssetID("../Assets/Prefabs/MilkBottle.prefab"),
			AssetMgr->GetAssetID("../Assets/Prefabs/milkbottlelight.prefab"),
			AssetMgr->GetAssetID("../Assets/Prefabs/milkbottleModel.prefab"), },
			1, SpawnType::MilkBottle
		));

	}
	void BossMinionSpawner::Init()
	{
		if (Doors.empty() || Doors.size() != total_doors) {
			for (int i = 0; i < total_doors; i++)
			{
				std::stringstream ss;
				ss << "EnemyContainerDoor" << i;
				auto find_entity = ECS->FindEntitiesByName(ss.str(), 1);
				if (!find_entity.empty()) {
					Doors.emplace_back(find_entity.front());
					auto doorScript = std::dynamic_pointer_cast<DoorScript>(ECS->GetComponent<GameLogic>(find_entity.front()).GetScript<DoorScript>());
					if (doorScript) {
						doorScript->SetAllowTriggerOpen(false);
					}
				}
			}
		}

		if (!Doors.empty()) {
			auto script = std::dynamic_pointer_cast<DoorScript>(ECS->GetComponent<GameLogic>(Doors.front()).GetScript<DoorScript>());
			if (script) {
				canSpawnAgain.duration = script->GetDoorOpenCloseDuration() * 2.f + doorOpenDuration;
			}
		}
	}

	void BossMinionSpawner::Update(double dt)
	{
		UpdateTimers((float)dt);
		//if (Input->IsKeyReleased(Key::_U)) {
		//	OpenDoorsAndSpawnRandom(num_of_DoorsOpen);
		//}

		// check if any lights spawned is removed.
		for (auto itr = LightsSpawned.begin(); itr != LightsSpawned.end(); )
		{
			if (!ECS->DoesEntityExists(*itr))
				itr = LightsSpawned.erase(itr); // remove even elements
			else
				++itr; // advance to the next element
		}

		// when door finishes opening add force to the spawned object to shoot it out.
		if (doorOpened.IsTimerEnded())
		{
			for (auto const& spawn: RecentlySpawned)
			{
				if (ECS->HasComponent<Physics>(spawn.first)) {
					Vec3f tossVel = spawn.second * tossSpawnForce;
					tossVel.y += tossSpawnForce * 0.5f;
					ECS->GetComponent<Physics>(spawn.first).SetLinearVelocity(tossVel);
				}
			}
			tossFinish.StartTimer();
		}
		// after finish tossing set enemy to active again
		if (tossFinish.IsTimerEnded())
		{
			for (auto const& spawn : RecentlySpawned)
			{
				if (ECS->HasComponent<GameLogic>(spawn.first)) {
					auto enemyScript = std::dynamic_pointer_cast<EnemyBase>(ECS->GetComponent<GameLogic>(spawn.first).GetScript<EnemyBase>());
					if (enemyScript) {
						enemyScript->SetAsleep(false);
						enemyScript->SetTarget(GLS->GetPlayerID());
					}
				}
			}
			RecentlySpawned.clear();
		}
	}

	void BossMinionSpawner::UpdateTimers(float dt)
	{
		doorOpened.update(dt);
		tossFinish.update(dt);
		canSpawnAgain.update(dt);
	}

	void BossMinionSpawner::OpenDoorsAndSpawnRandom(uint8_t num_of_doors_to_Open)
	{
		if(num_of_doors_to_Open <= Doors.size()) num_of_DoorsOpen = num_of_doors_to_Open;
		else num_of_DoorsOpen = (uint8_t)Doors.size();

		if (canSpawnAgain.IsTimerRunning()) return;
		if (Doors.empty() || Doors.size() != total_doors) return;
		// change this value to generate a different number of random numbers
		int minVal = 0;
		int maxVal = (int)(Doors.size() - 1);
		// Create a vector to store the generated numbers
		std::vector<int> generated_numbers;

		// Seed the random number generator
		std::random_device rd;
		std::mt19937 rng(rd());

		// Generate the unique random numbers
		while (generated_numbers.size() < num_of_DoorsOpen) {
			int num = std::uniform_int_distribution<int>{ minVal, maxVal }(rng);
			if (std::find(generated_numbers.begin(), generated_numbers.end(), num) == generated_numbers.end()) {
				generated_numbers.push_back(num);
			}
		}
		
		uint8_t currConsumeSpawn = 0;
		for (uint8_t i = 0; i < num_of_DoorsOpen; i++)
		{
			int randIndex = generated_numbers[i]; // generate a random number within the specified range
			EntityID doorID = Doors[randIndex];

			auto doorScript = std::dynamic_pointer_cast<DoorScript>(ECS->GetComponent<GameLogic>(doorID).GetScript<DoorScript>());
			if (doorScript) 
			{
				doorScript->OpenDoorsAndCloseAfter(doorOpenDuration);
	
				// spawn a random type behind the door
				std::random_device rd2;  // obtain a random number from hardware
				std::mt19937 gen(rd2()); // seed the generator
				int maxIndex = LightsSpawned.size() == maxLightSpawnAllowed || currConsumeSpawn == maxConsumableSpawnPerRound ?
					(int)(SpawnTypes.size() - 3) : (int)(SpawnTypes.size() - 1);
				std::uniform_int_distribution<> distr(0, maxIndex); // define the range
				int randSpawn = distr(gen);

				if (SpawnTypes[randSpawn].spawnType == SpawnType::BananaCrate || SpawnTypes[randSpawn].spawnType == SpawnType::MilkBottle)
					currConsumeSpawn++;

				SpawnBundleAtDoor(SpawnTypes[randSpawn], doorID);

				doorOpened.duration = doorScript->GetDoorOpenCloseDuration();
			}
		}
		doorOpened.StartTimer();
		canSpawnAgain.StartTimer();
	}

	void BossMinionSpawner::SpawnBundleAtDoor(SpawnBundle const& bundle, EntityID DoorID)
	{
		if (bundle.prefabIDs.empty()) return;

		Vec3f doorPos = ECS->GetComponent<Transform>(DoorID).GetPosition();
		Mat3f doorRotMatrix;
		doorRotMatrix.SetRotation(ECS->GetComponent<Transform>(DoorID).GetRotation());

		for (uint8_t i = 0; i < bundle.spawnCount; i++)
		{
			// spawn the parent prefab
			EntityID Parentid = ECS->SpawnPrefab(bundle.prefabIDs[0]);
			Transform& transf = ECS->GetComponent<Transform>(Parentid);

			// set its position to be behind the door.
			Vec3f dirBehindDoorRotated = doorRotMatrix * behindDoorDir;
			Vec3f spawnPos = doorPos + dirBehindDoorRotated * transf.GetScale().z;
			transf.SetPosition(spawnPos);

			// spawm all the child prefab and attach to the parent.
			for (size_t j = 1; j < bundle.prefabIDs.size(); j++) {
				EntityID childID = ECS->SpawnPrefab(bundle.prefabIDs[j]);
				Transform& mtransf = ECS->GetComponent<Transform>(childID);
				Vec3f savedLocalPos = mtransf.GetLocalPosition();
				mtransf.AttachParent(Parentid);
				ECS->GetComponent<Transform>(childID).SetLocalPosition(savedLocalPos);
			}

			// set its rotation to face out of door (after attaching children)
			// Mat3f spawnRotMatrix;
			// spawnRotMatrix.SetRotation(ECS->GetComponent<Transform>(Parentid).GetRotation());
			auto spawnRotation = ECS->GetComponent<Transform>(DoorID).GetRotation();//(-dirBehindDoorRotated).get_rotation_between(Vec3f().Forward() * spawnRotMatrix);
			ECS->GetComponent<Transform>(Parentid).SetRotation(spawnRotation);

			// add light object spawn count if it is bananacrate or MilkBottle
			if (bundle.spawnType == SpawnType::BananaCrate || bundle.spawnType == SpawnType::MilkBottle){
				LightsSpawned.emplace_back(Parentid);
			}
			else {
				auto enemyScript = std::dynamic_pointer_cast<EnemyBase>(ECS->GetComponent<GameLogic>(Parentid).GetScript<EnemyBase>());
				if (enemyScript) {
					enemyScript->SetAsleep(true);
				}
				auto charac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(Parentid).GetScript<CharacterScript>());
				if (charac) {
					charac->canDestroy = true;
				}
				MinionsActive.emplace_back(Parentid);
			}
			RecentlySpawned.insert(std::make_pair(Parentid, -dirBehindDoorRotated));
		}
	}

	void BossMinionSpawner::KillAllRemainingSpawns()
	{
		for (int i = 0; i < MinionsActive.size(); i++) {
			if (ECS->DoesEntityExists(MinionsActive[i]) && ECS->HasComponent<GameLogic>(MinionsActive[i])) {
				auto enemyScript = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(MinionsActive[i]).GetScript<CharacterScript>());
				if (enemyScript) {
					enemyScript->KillCharacter();
				}
			}
		}
		MinionsActive.clear();
	}

	void BossMinionSpawner::DestroyRemainingConsumables()
	{
		for (int i = 0; i < LightsSpawned.size(); i++) {
			if (ECS->DoesEntityExists(LightsSpawned[i])) {
				ECS->DestroyEntity(LightsSpawned[i]);
			}
		}
		LightsSpawned.clear();
	}

	void BossMinionSpawner::KnockBackMinionsActive(float kb)
	{
		for (int i = 0; i < MinionsActive.size(); i++) {
			auto echarac = std::dynamic_pointer_cast<CharacterScript>(ECS->GetComponent<GameLogic>(MinionsActive[i]).GetScript<CharacterScript>());
			if (echarac)
			{
				Vec3f kb_dir = ECS->GetComponent<Transform>(MinionsActive[i]).GetPosition() - ECS->GetComponent<Transform>(id).GetPosition();
				kb_dir.y = 0.f;
				echarac->KnockBackCharacter(kb_dir, kb);
			}
		}
	}


	void BossMinionSpawner::Respawn()
	{
		doorOpened.ResetTimer();
		tossFinish.ResetTimer();
		canSpawnAgain.ResetTimer();
		KillAllRemainingSpawns();
		DestroyRemainingConsumables();
		RecentlySpawned.clear();
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<BossMinionSpawner>(RegistrationNameCreator<BossMinionSpawner>())
		.constructor()
		.property("Doors ID", &BossMinionSpawner::Doors);
}