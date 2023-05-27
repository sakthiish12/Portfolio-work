#include "PathFinder.h"

namespace TH
{
	using namespace BE;

	void PathFinder::Init()
	{
		if (!pPatherInit) {
			pPatherInit = true;
			Astar.initialize();

			//TagID wall = ECS->CreateTag("Wall");
			Astar.SetTags({ ECS->GetTagID("Wall") });
			//using namespace std::chrono;
			//high_resolution_clock::time_point t1;
			//std::stringstream ss;
			//t1 = high_resolution_clock::now();
			Astar.AddGridLayer(13.f);
			Astar.UpdateGrid();
			//// Flood all four corners of the grid, setting them unpathable outside the playing map.
			Astar.FloodOutsideMap(GridPos(0,0,0));
			Astar.FloodOutsideMap(GridPos(0, 0, Astar.GetGridColumns() - 1));
			Astar.FloodOutsideMap(GridPos(Astar.GetGridRows() - 1, 0, 0));
			Astar.FloodOutsideMap(GridPos(Astar.GetGridRows() - 1, 0, Astar.GetGridColumns() - 1));

			// Flood all four corners of the grid, setting them unpathable outside the playing map.
			Astar.FloodOutsideMap(GridPos(0, 1, 0));
			Astar.FloodOutsideMap(GridPos(0, 1, Astar.GetGridColumns() - 1));
			Astar.FloodOutsideMap(GridPos(Astar.GetGridRows() - 1, 1, 0));
			Astar.FloodOutsideMap(GridPos(Astar.GetGridRows() - 1, 1, Astar.GetGridColumns() - 1));

			Astar.AddLayerLink(GridPos(3, 0, 17), GridPos(3, 1, 42));
			Astar.AddLayerLink(GridPos(4, 0, 17), GridPos(4, 1, 42));
			//Astar.GetGrid().DebugPrint();
			//duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
			//double time = time_span.count() * 1000;
			//ss << "Time amount: " << time ;
			//std::cout << ss.str() << "\n";
		}
	}
	void PathFinder::Update(double dt)
	{
		//if (Input->IsKeyPressed(Key::_H)) Astar.GetGrid().DebugPrint();
	}

	void PathFinder::SetGridOccupyInBox(Vec3f const& worldpos, float size_x, float size_z, sizeType layer, bool occupy)
	{
		auto grid_min = Astar.GetGrid().GetGridPosition(worldpos - Vec3f(size_x, 0.f, size_z) * 0.5f);
		auto grid_max = Astar.GetGrid().GetGridPosition(worldpos + Vec3f(size_x, 0.f, size_z) * 0.5f);
		for (sizeType i = grid_min.col; i < grid_max.col; i++) {
			for (sizeType j = grid_min.row; j < grid_max.row; j++) {
				Astar.SetPathable(GridPos(j, layer, i), occupy);
			}
		}

	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<PathFinder>(RegistrationNameCreator<PathFinder>())
		.constructor();
}


//
//float GridMinX = 43.f - Astar.GetGrid().GetGridExtentsX();
//float GridMinZ = -98.f - Astar.GetGrid().GetGridExtentsZ();
//float epsilon = 0.0001f;
//Vec3<float> BoxMin = { GridMinX + 9 * tile_size + epsilon, -2.5f + epsilon, GridMinZ + 9 * tile_size + epsilon };
//Vec3<float> BoxMax = { GridMinX + (9 + 1) * tile_size - epsilon, -2.5f + 13.f - epsilon, GridMinZ + (9 + 1) * tile_size - epsilon };
//
//// check if any objects in the box
//std::vector<EntityID> EntitiesInCell;
//PhySys->AABoxCollide(EntitiesInCell, BoxMin, BoxMax, { ECS->GetTagID("Wall") }, FilterType::ContainsAny);
//
//if (!EntitiesInCell.empty()) {
//	std::cout << EntitiesInCell.front() << " with name: " << ECS->GetComponent<EntityInfo>(EntitiesInCell.front()).GetName() << "\n";
//}