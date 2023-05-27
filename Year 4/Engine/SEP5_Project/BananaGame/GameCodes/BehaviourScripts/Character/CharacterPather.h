#pragma once
#ifndef	CHARACTER_PATHER_H
#define CHARACTER_PATHER_H

#include <BananaEngine.h>
#include "../Level/PathFinder.h"

namespace TH
{
	using namespace BE;

	class CharacterPather : public IScript
	{
		// Pathfinding
		PathRequest pather;
		Vec3f pathing_tile_goal;
		Vec3f pathing_dir_norm;
		Vec3f pSmoothMovementDirection;
		static Vec3f pOld_playerpos;
		Vec3f pOld_end_goal;
		bool MovingToTile = false;
		bool CanRecalculatePath = false;

		static PathFinder* pPathFinder;

		static inline float		pMaxSqrDistAwayFromOldGoal = 2.25f;
		static inline float		pDurationToRecalculatePath = 2.f;
		static inline float		pSqClosenessToGoal = 0.2f;
		float pTimer_toRecalculate = 0.f;

		void Init();
		void Update(double dt);

	public:
		inline void Reset_PathRecalculation_Restriction() { CanRecalculatePath = false;  pTimer_toRecalculate = 0.f; }
		inline void ClearPath() { pather.path.clear();  Reset_PathRecalculation_Restriction(); MovingToTile = false; }
		inline bool NoPath() const { return (pather.path.empty() && !MovingToTile); }
		inline bool CanFindNewPath() { return pather.path.empty() || (CanRecalculatePath && pOld_playerpos.SqDistBetween(ECS->GetComponent<Transform>(GLS->GetPlayerID()).GetPosition()) >= pMaxSqrDistAwayFromOldGoal); }

		inline Vec3f GetNormXZDirectionToPosition(Vec3f const& in);
		void UpdateGridOccupied(Vec3f const& oldpos, Vec3f const& newpos) const;
		inline bool IsAtPoint_XZPlane(Vec3f const& point) const;
		bool PathTo(float speed, Vec3f const& goal, EntityID target = NULL_ENTITY);

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}
#endif