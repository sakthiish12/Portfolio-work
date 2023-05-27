#include "CharacterPather.h"

namespace TH
{
	using namespace BE;

	PathFinder* CharacterPather::pPathFinder = nullptr;
	Vec3f CharacterPather::pOld_playerpos;

	void CharacterPather::Init()
	{
		const auto results = ECS->FindEntitiesByName("GameManager", 1);
		if (!results.empty())
		{
			auto scptr = ECS->GetComponent<GameLogic>(results.front()).GetScript<PathFinder>();
			PathFinder* patherfinder = dynamic_cast<PathFinder*>(scptr.get());
			if (patherfinder) pPathFinder = patherfinder;
		}

		// init pather
		pather.settings.heuristic = Heuristic::MANHATTAN;
		pather.settings.rubberBanding = false;
		pather.settings.smoothing = false;
		pather.settings.singleStep = false;

		pOld_playerpos = ECS->GetComponent<Transform>(GLS->GetPlayerID()).GetPosition();
	}

	void CharacterPather::Update(double dt)
	{
		if (!CanRecalculatePath) {
			pTimer_toRecalculate += static_cast<float>(dt);
			if (pTimer_toRecalculate >= pDurationToRecalculatePath) {
				pTimer_toRecalculate = 0.f;
				CanRecalculatePath = true;
			}
		}
	}

	inline Vec3f CharacterPather::GetNormXZDirectionToPosition(Vec3f const& in)
	{
		Vec3f dir = in - ECS->GetComponent<Transform>(id).GetPosition();
		dir.y = 0.f;
		return dir.Normalised();
	}

	void CharacterPather::UpdateGridOccupied(Vec3f const& oldpos, Vec3f const& newpos) const
	{
		if (pPathFinder) {
			pPathFinder->Astar.SetPathable(pPathFinder->Astar.GetGridPosition(oldpos), false);
			pPathFinder->Astar.SetPathable(pPathFinder->Astar.GetGridPosition(newpos), true);
		}
	}

	inline bool CharacterPather::IsAtPoint_XZPlane(Vec3f const& point) const
	{
		Vec3f pt = point;
		pt.y = ECS->GetComponent<Transform>(id).GetPosition().y;
		return ECS->GetComponent<Transform>(id).GetPosition().SqDistBetween(pt) <= pSqClosenessToGoal;
	}

	bool CharacterPather::PathTo(float speed, Vec3f const& goal, EntityID target)
	{
		bool FoundNewPath = false;
		if (pather.path.empty() || (CanRecalculatePath && goal.SqDistBetween(pOld_end_goal) >= pMaxSqrDistAwayFromOldGoal))
		{
			//std::cout << id << " calculated new goal: " << pPathFinder->Astar.GetGridPosition(goal).toString() << "\n";
			pather.start = ECS->GetComponent<Transform>(id).GetPosition();
			pather.goal = goal;
			pather.newRequest = true;
			auto results = pPathFinder->compute_path(pather);
			if (results == BE::PathResult::COMPLETE) {
				pather.path.pop_front();
				Reset_PathRecalculation_Restriction();
				pOld_end_goal = goal;
				pOld_playerpos = ECS->GetComponent<Transform>(GLS->GetPlayerID()).GetPosition();
				FoundNewPath = true;
				//std::cout << pather.path.size() << " << size \n";
			}
		}
		else {
			if (!MovingToTile) {
				pathing_tile_goal = pather.path.front();
				/*Vec3f pt = pathing_tile_goal;
				pt.y = ECS->GetComponent<Transform>(id).GetPositon().y;
				ECS->GetComponent<Transform>(testID).SetPosition(pt);*/
				pather.path.pop_front();

				pathing_dir_norm = GetNormXZDirectionToPosition(pathing_tile_goal);
				MovingToTile = true;
				ECS->GetComponent<Transform>(id).SetRotation(Vec3f::Forward().get_rotation_between(pathing_dir_norm));
				float euler = ECS->GetComponent<Transform>(id).GetEulerRotation().y;
				ECS->GetComponent<Transform>(id).SetEulerRotation(Vec3f(0.f, euler, 0.f));
			}
			if (MovingToTile) {
				pSmoothMovementDirection = 0.25f * pathing_dir_norm + 0.75f * pSmoothMovementDirection;

				Vec3f old = ECS->GetComponent<Physics>(id).GetLinearVelocity();
				Vec3f new_velocity = Vec3f(0.f, old.y, 0);;

				new_velocity += pSmoothMovementDirection * speed;

				Vec3f actual = pathing_tile_goal - ECS->GetComponent<Transform>(id).GetPosition();

				if (old.x * actual.x + old.z * actual.z < -0.0001f) {
					ECS->GetComponent<Physics>(id).SetLinearVelocity(Vec3f(0.f, 0.f, 0.f));
					ClearPath();
					return FoundNewPath;
				}
				ECS->GetComponent<Physics>(id).SetLinearVelocity(new_velocity);
				
			}
		}
		if (IsAtPoint_XZPlane(pathing_tile_goal)) {
			MovingToTile = false;
		}
		return FoundNewPath;
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<CharacterPather>(RegistrationNameCreator<CharacterPather>())
		.constructor();
}