#pragma once

#include <BananaEngine.h>
#include "../Character/BaseSkill.h"

namespace TH
{
	using namespace BE;

	class ElectricTowers : public IScript
	{
		using TowerStep = unsigned int;
		using TileIndex = unsigned int;
		using TowerPattern = std::multimap<TowerStep, TileIndex>;

		std::vector<TowerPattern> eTowersPatterns;
		std::vector<EntityID> eTowers;
		std::vector<float> original_heights;
		EntityID eElectricFloor = NULL_ENTITY;
		Timer eTowerRising_timer = Timer(6.f);
		Timer eFloorRising_timer = Timer(0.5f);
		float eFloorHeight = 2.0f;
		float eTowerHeight_per_unit = 3.5f;
		float eTowerDuration_per_unit = 1.f;
		float eOriginal_ypos;
		float eOriginal_Floorpos;
		Quatf eOriginal_rot;
		Quatf eOriginal_FloorRot;
		const int totaltiles = 36;
		const TowerStep highest_unit = 3;
		bool current_pattern = false;
		bool isLowering = false;
		bool hasLifted = false;
		bool AtHighest = false;
		bool AtLowest = true;

		AssetID SFX_PlatformRising = AssetID(15982946267233202529);
		AssetID SFX_PlatformStop = AssetID(14038442959530087640);
		AssetID SFX_Electric = AssetID(3789064965570303426);


		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		void UpdateTimers(float dt);
		void StopTowers(TowerStep i);
		void LiftTowers();
		void LiftElectricFloor();
		void LowerTowers();
		void LowerElectricFloor();

	public:
		void LiftTowersAndElectricFloor();
		void LowerTowersAndElectricFloor();
		void ReturnToOriginalHeight();
		inline bool IsAtHighest() const { return AtHighest; };
		void Respawn();
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}