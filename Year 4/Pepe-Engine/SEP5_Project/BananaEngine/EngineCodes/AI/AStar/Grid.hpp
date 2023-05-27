

namespace BE
{

	template <typename T>
	Grid3D<T>::Grid3D()
	{

	}

	template <typename T>
	Grid3D<T>::Grid3D(valueType _TileSize, Vec3<valueType> const& _GridFloorCenter, sizeType rows, sizeType columns, valueType first_height, std::set<TagID> const& tags)
	{
		settings.TileSize = _TileSize;
		settings.Rows = rows ? rows : 1;
		settings.Columns = columns ? columns : 1;
		settings.GridFloorCenter = _GridFloorCenter;
		registered_tags = tags;
		AddGridLayer(first_height);
	}

	template <typename T>
	Grid3D<T>::Grid3D(GridSettings const& _settings, valueType first_height, std::set<TagID> const& tags)
		: settings{ _settings }
	{
		registered_tags = tags;
		AddGridLayer(first_height);
	}

	template <typename T>
	Grid3D<T>::~Grid3D()
	{

	}

	template <typename T>
	typename sizeType Grid3D<T>::GetLayerNum(typename valueType world_pos_y) const
	{
		sizeType layernum = 0;
		valueType layerY = settings.GridFloorCenter.y;
		if (world_pos_y < settings.GridFloorCenter.y) {
			AssertCore(false, "world position y is outside the grid");
		}
		for (auto& layer : Grids) {
			layerY += layer.height;
			if (layerY > world_pos_y) {
				break;
			}
			layernum++;
		}
		return layernum;
	}

	template <typename T>
	void Grid3D<T>::AddGridLayer(typename valueType height)
	{
		Grids.emplace_back(Grid2D<T>(height, settings.Rows, settings.Columns));
	}

	template <typename T>
	GridPos Grid3D<T>::GetGridPosition(Vec3<float> const& worldPos) const
	{
		GridPos gridpos{};
		if (IsValidWorldCoords(worldPos)) {
			gridpos.layer = GetLayerNum(worldPos.y);
			gridpos.row = static_cast<sizeType>((worldPos.z - settings.GridFloorCenter.z + GetGridExtentsZ()) / settings.TileSize);
			gridpos.col = static_cast<sizeType>((worldPos.x - settings.GridFloorCenter.x + GetGridExtentsX()) / settings.TileSize);
		}
		else
		{
			//AssertCore(false, "world position is outside the grid");
		}
		return gridpos;
	}

	template <typename T>
	Vec3<float> Grid3D<T>::GetWorldPosition(GridPos const& gridpos) const
	{
		Vec3<float> worldpos{};
		if (IsValidGridCoords(gridpos)) {
			valueType worldy = settings.GridFloorCenter.y;
			sizeType count = 0;
			for (auto& grid : Grids) {
				if (count == gridpos.layer) {
					worldy += grid.height * 0.5f;
					break;
				}
				else worldy += grid.height;
				count++;
			}
			worldpos.y = worldy;
			worldpos.z = settings.GridFloorCenter.z - GetGridExtentsZ() + settings.TileSize * gridpos.row + settings.TileSize * 0.5f;
			worldpos.x = settings.GridFloorCenter.x - GetGridExtentsX() + settings.TileSize * gridpos.col + settings.TileSize * 0.5f;
		}
		else
		{
			//AssertCore(false, "grid position is outside the grid");
		}
		return worldpos;
	}

	template <typename T>
	bool Grid3D<T>::IsValidWorldCoords(Vec3<valueType> const& point) const
	{
		valueType topY = settings.GridFloorCenter.y;
		for (int i = 0; i < Grids.size(); i++) {
			topY += Grids[i].height;
		}
		if (point.y <= topY && point.y >= settings.GridFloorCenter.y &&
			point.x <= settings.GridFloorCenter.x + GetGridExtentsX() && point.x >= settings.GridFloorCenter.x - GetGridExtentsX() &&
			point.z <= settings.GridFloorCenter.z + GetGridExtentsZ() && point.z >= settings.GridFloorCenter.z - GetGridExtentsZ())
			return true;
		else return false;
	}

	template <typename T>
	bool Grid3D<T>::IsValidGridCoords(GridPos const& point) const
	{
		return (point.layer < Layers() && point.row < settings.Rows && point.col < settings.Columns);
	}

	template <typename T>
	void Grid3D<T>::UpdateGrid()
	{
		sizeType layernum = 0;
		valueType layerY = settings.GridFloorCenter.y;
		float GridMinX = settings.GridFloorCenter.x - GetGridExtentsX();
		float GridMinZ = settings.GridFloorCenter.z - GetGridExtentsZ();

		for (auto const& layer : Grids) {
			for (size_t row = 0; row < settings.Rows; row++) {
				for (size_t col = 0; col < settings.Columns; col++) {
					Vec3<float> BoxMin = { GridMinX + col * settings.TileSize + epsilon, layerY + epsilon, GridMinZ + row * settings.TileSize + epsilon };
					Vec3<float> BoxMax = { GridMinX + (col + 1) * settings.TileSize - epsilon, layerY + layer.height - epsilon, GridMinZ + (row + 1) * settings.TileSize - epsilon };

					// check if any objects in the box
					std::vector<EntityID> EntitiesInCell;
					PhySys->AABoxCollide(EntitiesInCell, BoxMin, BoxMax, registered_tags, FilterType::ContainsAny);

					if (!EntitiesInCell.empty()) {
						Grids[layernum][row][col].occupied = true;
						Grids[layernum][row][col].isWall = true;
					}
					else Grids[layernum][row][col].occupied = false;


				}
			}
			layerY += layer.height;
			layernum++;
		}
	}

	template <typename T>
	void Grid3D<T>::ResetGrid()
	{
		for (auto& grid2D : Grids) {
			grid2D.Reset();
		}
	}

	template <typename T>
	void Grid3D<T>::ResetLayer(sizeType layernum)
	{
		if (layernum < Layers()) {
			Grids[layernum].Reset();
		}
		else {
			AssertCore(false, "Reseting a layer number that is more than layer size");
		}
	}

	template <typename T>
	void Grid3D<T>::SetRows(sizeType _in)
	{
		sizeType in = _in ? _in : 1;
		if (settings.Rows == in) return;
		for (auto& grid2d : Grids) {
			sizeType num_of_col = grid2d.Columns();
			grid2d.Grid.resize(settings.Rows, std::vector<T>(num_of_col, T()));
		}
	}

	template <typename T>
	void Grid3D<T>::SetColumns(sizeType _in)
	{
		sizeType in = _in ? _in : 1;
		if (settings.Columns == in) return;
		for (auto& grid2d : Grids) {
			for (auto& row : grid2d.Grid) {
				row.resize(in, T());
			}
		}
	}

	//template <typename T>
	//void Grid3D<T>::AddTag(const std::string& tagname)
	//{
	//	TagID tag = ECS->GetTagID(tagname);
	//	registered_tags.insert(tag);
	//}

	//template <typename T>
	//void Grid3D<T>::RemoveTag(const std::string& tagname)
	//{
	//	TagID tag = ECS->GetTagID(tagname);
	//	registered_tags.erase(tag);
	//}

	template <typename T>
	void Grid3D<T>::DebugPrint() const
	{
		int layer = 0;
		std::cout << "\nPrinting Grid Data:\n";
		for (auto& grid2D : Grids) {
			std::cout << "<Layer " << layer++ << ">: \n";
			for (int i = static_cast<int>(grid2D.Grid.size() - 1); i >= 0; i--) {
				for (int j = 0; j < grid2D.Grid[i].size(); j++) {
					std::cout << bool(grid2D.Grid[i][j]) << " ";
				}
				std::cout << "\n";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	template <typename T>
	void Grid3D<T>::FloodOutsideMap(GridPos const& in)
	{
		//FloodIterative(starting);
		std::stack<GridPos> cellsToFill;
		cellsToFill.push(in);

		while (!cellsToFill.empty()) {
			auto currentcell = cellsToFill.top();
			cellsToFill.pop();
			sizeType col = currentcell.col;
			sizeType row = currentcell.row;
			if (col < 0 || col >= Columns() || row < 0 || row >= Rows() || bool(Grids[currentcell.layer][row][col])) {
				// Cell is out of bounds or already true, skip it
				continue;
			}

			// Fill current cell
			Grids[in.layer][row][col].occupied = true;

			// Add neighbours to stack
			cellsToFill.push(GridPos(row + 1, in.layer, col));
			cellsToFill.push(GridPos(row - 1, in.layer, col));
			cellsToFill.push(GridPos(row, in.layer, col + 1));
			cellsToFill.push(GridPos(row, in.layer, col - 1));
		}
	}

	template <typename T>
	void Grid3D<T>::FloodIterative(GridPos const& in)
	{
		if (!IsValidGridCoords(in)) return;

		if (!bool(Grids[in.layer][in.row][in.col]))
			Grids[in.layer][in.row][in.col].occupied = true;
		else return;

		FloodIterative(GridPos(in.row + 1, in.layer, in.col));
		FloodIterative(GridPos(in.row, in.layer, in.col + 1));
		FloodIterative(GridPos(in.row, in.layer, in.col - 1));
		FloodIterative(GridPos(in.row - 1, in.layer, in.col));
		/*auto neighbour = GetTileNeighbours(in);
		for (int i = 0; i < 8; i++) {
			FloodIterative(neighbour[i]);
		}*/
	}

	template <typename T>
	std::array<GridPos,8> Grid3D<T>::GetTileNeighbours(GridPos const& in) {
		std::array<GridPos, 8> grid_tiles;
		grid_tiles[0] = GridPos(in.row+1, in.layer, in.col);
		grid_tiles[1] = GridPos(in.row+1, in.layer, in.col-1);
		grid_tiles[2] = GridPos(in.row+1, in.layer, in.col+1);
		grid_tiles[3] = GridPos(in.row, in.layer, in.col-1);
		grid_tiles[4] = GridPos(in.row, in.layer, in.col+1);
		grid_tiles[5] = GridPos(in.row-1, in.layer, in.col);
		grid_tiles[6] = GridPos(in.row-1, in.layer, in.col-1);
		grid_tiles[7] = GridPos(in.row-1, in.layer, in.col+1);
		return grid_tiles;
	}
	//template <typename T>
	//GridPos* Grid3D<T>::GetTileNeighbours(GridPos const& in) {
	//	GridPos grid_tiles[8];

	//	const int steps = 2;
	//	int step_counter = 0;
	//	GridDirection direction = GridDirection::Left;
	//	for (int i = 0; i < 8; i++) {

	//		switch(direction){
	//		case GridDirection::Left: {

	//		}
	//		case GridDirection::Down: {
	//			
	//		}
	//		case GridDirection::Right:{

	//		}
	//		case GridDirection::Up: {

	//		}
	//		}

	//		step_counter++;
	//		if (step_counter == steps) {
	//			step_counter = 0;
	//			++direction;
	//		}
	//	}
	//	return grid_tiles;
	//}

}