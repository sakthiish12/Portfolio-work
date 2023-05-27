
#include "ElectricTowers.h"
#include "../Environment/DOTObject.h"

namespace TH
{
	using namespace BE;

	void ElectricTowers::Awake()
	{
		// pattern 1
		TowerPattern pattern1 = {
			{1, 0}, {2, 3}, {1, 4},
			{1, 7}, {3, 10},
			{2, 12}, {3, 13}, {2, 17},
			{2, 19}, {3 ,22},
			{1, 24}, {1, 25}, {2, 28},
			{2, 30}, {1, 33}
		};
		eTowersPatterns.emplace_back(pattern1);
		// pattern 2
		TowerPattern pattern2 = {
			{1, 1}, {2, 2}, {1, 4}, {1, 5},
			{3, 9}, {2, 10},
			{2, 12}, {3, 13}, {1, 16},
			{1, 18},
			{2, 24}, {1, 26}, {1, 27}, {2, 28}, {3, 29},
			{2, 31}, {2, 35}
		};
		eTowersPatterns.emplace_back(pattern2);
	}
	void ElectricTowers::Init()
	{
		if (eTowers.empty() || eTowers.size() != totaltiles) {
			for (int i = 0; i < totaltiles; i++)
			{
				std::stringstream ss;
				ss << "RisingPlatform " << i;
				auto find_entity = ECS->FindEntitiesByName(ss.str(), 1);
				if (!find_entity.empty()) {
					eTowers.emplace_back(find_entity.front());
					eOriginal_ypos = ECS->GetComponent<Transform>(find_entity.front()).GetPosition().y;
					eOriginal_rot = ECS->GetComponent<Transform>(eTowers.front()).GetRotation();
				}
			}
		}
		else {
			eOriginal_ypos = ECS->GetComponent<Transform>(eTowers.front()).GetPosition().y;
			eOriginal_rot = ECS->GetComponent<Transform>(eTowers.front()).GetRotation();
		}

		if (original_heights.empty()) {
			auto& children = ECS->GetComponent<Transform>(eTowers.front()).GetChildren();
			for (int i=0; i< children.size(); i++)
			{
				original_heights.emplace_back(ECS->GetComponent<Transform>(children[i]).GetPosition().y);
			}
		}

		auto find_entity = ECS->FindEntitiesByName("Boss Electric Floor", 1);
		if (!find_entity.empty()) {
			eElectricFloor = find_entity.front();
		}

		if (ECS->DoesEntityExists(eElectricFloor) && ECS->HasComponent<GameLogic>(eElectricFloor)) {

			eOriginal_FloorRot = ECS->GetComponent<Transform>(eElectricFloor).GetRotation();
			eOriginal_Floorpos = ECS->GetComponent<Transform>(eElectricFloor).GetPosition().y;
			auto script = std::dynamic_pointer_cast<DOTObject>(ECS->GetComponent<GameLogic>(eElectricFloor).GetScript<DOTObject>());
			if (script) {
				script->SetAttackerID(id);
				script->SetDamageRate(2.f);
				script->SetBaseAttack(10.f);
				script->SetDamageMultiplier(0.1f);
				script->SetIgnoreTag("Boss");
			}
		}
		eTowerRising_timer.duration = highest_unit * eTowerDuration_per_unit;
		eFloorRising_timer.duration = 0.5f;
	}

	void ElectricTowers::Update(double dt)
	{
		UpdateTimers((float)dt);

		//if (Input->IsKeyReleased(Key::_J)) {
		//	LiftTowersAndElectricFloor();
		//}
		//else if (Input->IsKeyReleased(Key::_K)) {
		//	LowerTowersAndElectricFloor();
		//}

		int numberOfSteps = Engine->GetPhysSteps();
		bool towertimerEnded = false;
		bool floortimerEnded = false;
		for (int step = 0; step < numberOfSteps; ++step)
		{
			eTowerRising_timer.update(static_cast<float>(Engine->GetFixedDeltaTime() * EngineSpeed->GetEngineSpeedMultiplier()));
			eFloorRising_timer.update(static_cast<float>(Engine->GetFixedDeltaTime() * EngineSpeed->GetEngineSpeedMultiplier()));
			if (!towertimerEnded) towertimerEnded = eTowerRising_timer.ended;
			if (!floortimerEnded) floortimerEnded = eFloorRising_timer.ended;


		}
		// stop floor
		if (floortimerEnded) {
			if (ECS->DoesEntityExists(eElectricFloor)) {

				ECS->GetComponent<Physics>(eElectricFloor).SetLinearVelocity(Vec3f());
				if (isLowering) {
					Vec3f stoppos = ECS->GetComponent<Transform>(eElectricFloor).GetPosition();
					stoppos.y = eOriginal_Floorpos;
					ECS->GetComponent<Transform>(eElectricFloor).SetPosition(stoppos);
				}
			}
		}

		for (TowerStep i = 1; i <= highest_unit; i++)
		{
			if ((eTowerRising_timer.timer >= i * eTowerDuration_per_unit) ||
				(i == highest_unit && towertimerEnded)) {
				StopTowers(i);
			}
		}

		if (hasLifted) {
			AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
			if (!aSource.IsAudioPlaying(SFX_Electric)) aSource.Play(SFX_Electric);
		}
	}

	void ElectricTowers::UpdateTimers(float dt)
	{
	}

	void ElectricTowers::StopTowers(TowerStep i)
	{
		auto range = eTowersPatterns[static_cast<int>(current_pattern)].equal_range(i);
		for (auto it = range.first; it != range.second; ++it) {
			TileIndex idx = it->second;
			ECS->GetComponent<Physics>(eTowers[idx]).SetLinearVelocity(Vec3f());

			for (auto child : ECS->GetComponent<Transform>(eTowers[idx]).GetChildren())
			{
				if (ECS->HasComponent<Physics>(child))
				{
					ECS->GetComponent<Physics>(child).SetLinearVelocity(Vec3f());
				}
			}

			if (isLowering) {
				Vec3f lowest = ECS->GetComponent<Transform>(eTowers[idx]).GetPosition();
				lowest.y = eOriginal_ypos;
				ECS->GetComponent<Transform>(eTowers[idx]).SetPosition(lowest);
			}
		}

		if (i == highest_unit) {
			if (isLowering){
				AtLowest = true;
				current_pattern = !current_pattern;
				isLowering = false;
				hasLifted = false;
			}
			else {
				AtHighest = true;
				AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
				aSource.Play(SFX_PlatformStop);
			}
		}
	}

	void ElectricTowers::LiftTowers()
	{
		if (hasLifted) return;
		hasLifted = true;
		const auto& curr_pattern = eTowersPatterns[static_cast<int>(current_pattern)];

		for (auto const& pair: curr_pattern)
		{
			TileIndex idx = pair.second;
			if (idx < eTowers.size()) {
				Vec3f EndPos = ECS->GetComponent<Transform>(eTowers[idx]).GetPosition();
				EndPos.y += eTowerHeight_per_unit * pair.first;
				ECS->GetComponent<Physics>(eTowers[idx]).MoveKinematic(EndPos , eOriginal_rot, eTowerDuration_per_unit * pair.first);

				for (auto child : ECS->GetComponent<Transform>(eTowers[idx]).GetChildren())
				{
					if (ECS->HasComponent<Physics>(child))
					{
						Vec3f childEndPos = ECS->GetComponent<Transform>(child).GetPosition();
						childEndPos.y += eTowerHeight_per_unit * pair.first;
						ECS->GetComponent<Physics>(child).MoveKinematic(childEndPos, ECS->GetComponent<Transform>(child).GetRotation(), eTowerDuration_per_unit * pair.first);
					}
				}
			}
		}
		eTowerRising_timer.start = true;

		AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
		aSource.Play(SFX_PlatformRising);
	}

	void ElectricTowers::LiftElectricFloor()
	{
		if (ECS->DoesEntityExists(eElectricFloor))
		{
			Vec3f EndPos = ECS->GetComponent<Transform>(eElectricFloor).GetPosition();
			EndPos.y += eFloorHeight;
			ECS->GetComponent<Physics>(eElectricFloor).MoveKinematic(EndPos, eOriginal_FloorRot, eFloorRising_timer.duration);
		}
		eFloorRising_timer.start = true;
	}

	void ElectricTowers::LowerTowers()
	{
		const auto& curr_pattern = eTowersPatterns[static_cast<int>(current_pattern)];
		for (auto const& pair : curr_pattern)
		{
			TileIndex idx = pair.second;
			if (idx < eTowers.size()) {
				Vec3f EndPos = ECS->GetComponent<Transform>(eTowers[idx]).GetPosition();
				EndPos.y = eOriginal_Floorpos;
				ECS->GetComponent<Physics>(eTowers[idx]).MoveKinematic(EndPos, eOriginal_rot, eTowerDuration_per_unit * pair.first);
				
				auto& children = ECS->GetComponent<Transform>(eTowers[idx]).GetChildren();
				for (int i=0; i < children.size(); i++ )
				{
					auto child = children[i];
					if (ECS->HasComponent<Physics>(child))
					{
						Vec3f childEndPos = ECS->GetComponent<Transform>(child).GetPosition();
						childEndPos.y = original_heights[i];
						ECS->GetComponent<Physics>(child).MoveKinematic(childEndPos, ECS->GetComponent<Transform>(child).GetRotation(), eTowerDuration_per_unit * pair.first);
					}
				}
			}
		}
		isLowering = true;
		eTowerRising_timer.start = true;

		AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
		aSource.Play(SFX_PlatformRising);
	}

	void ElectricTowers::LowerElectricFloor()
	{
		if (ECS->DoesEntityExists(eElectricFloor))
		{
			Vec3f EndPos = ECS->GetComponent<Transform>(eElectricFloor).GetPosition();
			EndPos.y -= eFloorHeight;
			ECS->GetComponent<Physics>(eElectricFloor).MoveKinematic(EndPos, eOriginal_FloorRot, eFloorRising_timer.duration);
		}
		eFloorRising_timer.start = true;
	}

	void ElectricTowers::LiftTowersAndElectricFloor()
	{
		if (AtLowest) {
			LiftTowers();
			LiftElectricFloor();
			AtLowest = false;
		}
	}

	void ElectricTowers::LowerTowersAndElectricFloor()
	{
		if (AtHighest) {
			LowerTowers();
			LowerElectricFloor();
			AtHighest = false;
		}
	}

	void ElectricTowers::Respawn()
	{
		for (int i = 0; i < eTowers.size(); i++)
		{
			auto& towerTrans = ECS->GetComponent<Transform>(eTowers[i]);
			Vec3f currpos = towerTrans.GetPosition();
			currpos.y = eOriginal_ypos;
			towerTrans.SetPosition(currpos);
		}

		auto& floorTrans = ECS->GetComponent<Transform>(eElectricFloor);
		Vec3f floorcurrpos = floorTrans.GetPosition();
		floorcurrpos.y = eOriginal_Floorpos;
		floorTrans.SetPosition(floorcurrpos);

		isLowering = false;
		hasLifted = false;
		AtHighest = false;
		AtLowest = true;

		eTowerRising_timer.ResetTimer();
		eFloorRising_timer.ResetTimer();

	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<ElectricTowers>(RegistrationNameCreator<ElectricTowers>())
		.constructor()
		.property("Electric Floor ID", &ElectricTowers::eElectricFloor)
		.property("Towers ID", &ElectricTowers::eTowers);
}