#include "PlayerSurround.h"

namespace TH
{
	using namespace BE;

	void PlayerSurround::Init()
	{
		const auto results = ECS->FindEntitiesByName("GameManager", 1);
		if (!results.empty())
		{
			auto scptr = ECS->GetComponent<GameLogic>(results.front()).GetScript<PathFinder>();
			PathFinder* patherfinder = dynamic_cast<PathFinder*>(scptr.get());
			pPathFinder = patherfinder;
		}

		oldPosition = ECS->GetComponent<Transform>(id).GetPosition();
		AddNewOccupied();
	}

	void PlayerSurround::Update(double dt)
	{
		if (oldPosition.SqDistBetween(ECS->GetComponent<Transform>(id).GetPosition()) >= pPathFinder->tile_size * pPathFinder->tile_size)
		{
			//UpdateGridOccupied(oldPosition, ECS->GetComponent<Transform>(id).GetPosition());
			oldPosition = ECS->GetComponent<Transform>(id).GetPosition();
		}
		//std::cout << pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition()).toString() << "\n";
	}

	SurroundPos PlayerSurround::GetAvailableSpot(Vec3f const& enemyPos)
	{
		Vec3f forward = Vec3f::Forward();
		Vec3f enemyDir = enemyPos - ECS->GetComponent<Transform>(id).GetPosition();
		GridPos playerTile = pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition());
		SurroundPos surround = toDirection(enemyDir);
		for (short ring_counter = 1; ring_counter <= static_cast<short>(sMaxRingSearch); ring_counter++) 
		{
			
			SurroundPos saveinitial = surround;

			for (short i = 0; i < ring_counter * 8; i++) {
				auto itr = surroundOccupied.find(surround);
				if (itr == surroundOccupied.end()) {
					GridPos tile = surround + pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition());
					if (pPathFinder->Astar.IsPathable(tile)) {
						surroundOccupied.insert(surround);
						return surround;
					}
				}

				// Rotate CCW
				if (surround.z < 0 && surround.x != ring_counter) {
					surround.x += 1;
				}
				else if (surround.z > 0 && surround.x != -ring_counter) {
					surround.x -= 1;
				}
				else if (surround.x < 0 && surround.z != -ring_counter) {
					surround.z -= 1;
				}
				else if (surround.x > 0 && surround.z != ring_counter) {
					surround.z += 1;
				}

			}
			surround = saveinitial.GetOuterRing();

		}
		return SurroundPos(0,0);
	}

	SurroundPos PlayerSurround::UpdateCurrentSpot(SurroundPos surround, Vec3f const& enemyPos, Vec3f& playerOldPos)
	{
		if (surround == SurroundPos(0, 0)) {
			playerOldPos = ECS->GetComponent<Transform>(id).GetPosition();
			return GetAvailableSpot(enemyPos);
		}
		if (playerOldPos.SqDistBetween(ECS->GetComponent<Transform>(id).GetPosition()) >= pPathFinder->tile_size * pPathFinder->tile_size) {
			GridPos playerTile = pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition());
			auto itr = surroundOccupied.find(surround);
			if (itr != surroundOccupied.end()) {
				surroundOccupied.erase(itr);
			}
			playerOldPos = ECS->GetComponent<Transform>(id).GetPosition();
			return GetAvailableSpot(enemyPos);
		}
		return surround;
	}

}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<PlayerSurround>(RegistrationNameCreator<PlayerSurround>())
		.constructor();
}