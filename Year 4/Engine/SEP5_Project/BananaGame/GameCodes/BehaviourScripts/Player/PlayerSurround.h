#pragma once
#include <BananaEngine.h>
#include "../Level/PathFinder.h"

namespace TH
{
	using namespace BE;

	struct SurroundPos
	{
		short x;
		short z;

		bool operator==(const SurroundPos& rhs) const
		{
			return x == rhs.x && z == rhs.z;
		}

		bool operator!=(const SurroundPos& rhs) const
		{
			return x != rhs.x || z != rhs.z;
		}

		GridPos operator+(const GridPos& rhs) const
		{
			return GridPos(rhs.row + z, rhs.layer, rhs.col + x);
		}
		inline bool isInRing(unsigned short index) const
		{
			return abs(x) == index || abs(z) == index;
		}

		inline bool isCorner() const
		{
			return abs(x) == abs(z);
		}

		inline SurroundPos GetOuterRing() const
		{
			short _x = x < 0 ? x - 1 : ((x > 0) ? x + 1 : 0);
			short _z = z < 0 ? z - 1 : ((z > 0) ? z + 1 : 0);
			return SurroundPos(_x, _z);
		}

	};
}

namespace std {
	template<>
	struct hash<TH::SurroundPos> {
		std::size_t operator()(const TH::SurroundPos& p) const {
			std::hash<short> x_hash;
			std::hash<short> z_hash;
			return x_hash(p.x) ^ z_hash(p.z);
		}
	};
}

namespace TH
{
	using namespace BE;

	class PlayerSurround : public IScript
	{
		const std::array<SurroundPos, 8> Directions = {{ {1, 0}, {1, -1}, {1, 1}, {0, 1}, {0, -1}, {-1, 0}, {-1, -1}, {-1, 1} }};
		const unsigned short sMaxRingSearch = 5;
		std::unordered_set<SurroundPos> surroundOccupied;
		Vec3f oldPosition;
		PathFinder* pPathFinder = nullptr;
		float tile_distance = 1.5f;

		void Init();

		void Update(double dt);

		// Convert a vector to the closest direction
		SurroundPos toDirection(const Vec3f& vec) {
			Vec3f norm = vec.Normalised();

			int closestIndex = 0;
			float closestDot = norm.x * Directions[0].x + norm.z * Directions[0].z;

			for (int i = 1; i < Directions.size(); i++) {
				float dot = norm.x * Directions[i].x + norm.z * Directions[i].z;
				if (dot > closestDot) {
					closestDot = dot;
					closestIndex = i;
				}
			}
			return Directions[closestIndex];
		}

		SurroundPos GetAvailableSpot(Vec3f const& enemyPos);

	public:

		SurroundPos UpdateCurrentSpot(SurroundPos surround, Vec3f const& enemyPos, Vec3f& playerOldPos);

		Vec3f GetWorldPositionAt(SurroundPos surround)
		{
			GridPos playerTile = pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition());
			return pPathFinder->Astar.GetWorldPosition(surround + playerTile);
		}

		void RemovePositionalCount(SurroundPos surround)
		{
			auto itr = surroundOccupied.find(surround);
			if (itr != surroundOccupied.end()) {
				surroundOccupied.erase(itr);
			}
		}
		void UpdateGridOccupied(Vec3f const& oldpos, Vec3f const& newpos)
		{
			RemoveOldOccupied();
			AddNewOccupied();
		}

		void RemoveOldOccupied()
		{
			GridPos playerTile = pPathFinder->Astar.GetGridPosition(oldPosition);
			/*for (auto const& pos : surroundOccupied) {
				pPathFinder->Astar.SetPathable(pos + playerTile, false);
			}*/
			pPathFinder->Astar.SetPathable(playerTile, false);
		}
		void AddNewOccupied()
		{
			GridPos playerTile = pPathFinder->Astar.GetGridPosition(ECS->GetComponent<Transform>(id).GetPosition());
			/*for (auto const& pos: surroundOccupied) {
				pPathFinder->Astar.SetPathable(pos + playerTile, true);
			}*/
			pPathFinder->Astar.SetPathable(playerTile, true);
		}

	
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}

