#pragma once
#include <BananaEngine.h>

namespace TH
{
	using namespace BE;

	class PathFinder : public IScript
	{
		// static A starpath finder.
	public:
		const float tile_size = 1.5f;
		AStarPather Astar = AStarPather(GridSettings{ tile_size, 170, 120, Vec3f(63.5f, -18.f, -98.f) }, 13.f, { });
		//AStarPather Astar = AStarPather(GridSettings{ tile_size, 130, 68, Vec3f(44.f, 0.0f, -70.f) }, 14.f, { });

	private:

		bool pPatherInit = false;

		void Init();
		void Update(double dt);

	public:
		inline void UpdateGrid() {
			Astar.UpdateGrid();
		}
		inline PathResult compute_path(PathRequest& in) {
			return Astar.compute_path(in);
		}

		inline void DebugGrid() {
			Astar.GetGrid().DebugPrint();
		}

		void SetGridOccupyInBox(Vec3f const& worldpos, float size_x, float size_z, sizeType layer, bool occupy);

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}