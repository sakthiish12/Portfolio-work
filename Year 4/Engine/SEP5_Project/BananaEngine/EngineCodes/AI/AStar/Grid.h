#pragma once
#ifndef GRID_H
#define GRID_H

#include "pch.h"
#include "../Systems/PhysicsSystem/sPhysicsSystem.h"
#include "../Systems/ECS/ECS.h"
#include "GridInfo.h"
namespace BE
{
	template <typename T = CellInfo>
	struct Grid2D
	{
		std::vector<std::vector<T>> Grid;
		valueType height;

		Grid2D(valueType _height, sizeType rows, sizeType columns)
			:Grid{ std::vector<std::vector<T>>(rows, std::vector<T>(columns, T())) },
			height{ _height }
		{
		}

		// subscript operator overload to access the grid columns easily.
		inline std::vector<T>& operator[](size_t row) { return Grid[row]; }
		inline std::vector<T>const& operator[](size_t row) const { return Grid[row]; }
		
		// get the number of columns in this 2d grid
		inline sizeType Columns() { if (Grid.empty()) return 0; else return static_cast<sizeType>(Grid[0].size()); }
		
		// get the number of rows in this 2D grid
		inline sizeType Rows() { return static_cast<sizeType>(Grid.size()); }
		
		// reset the cell info in all the cells of this 2D grid.
		inline void Reset() { for (auto& rows : Grid) { rows.assign(rows.size(), T()); } }
	};

	template <typename T = CellInfo>
	class Grid3D
	{
	private:
		GridSettings settings{};
		static constexpr float epsilon = 0.000001f;
		// y , z , x
		//layer, row, column
		std::vector<Grid2D<T>> Grids;
		std::set<TagID> registered_tags;

	public:
		Grid3D();

		// constructor using raw values
		Grid3D(valueType _TileSize, Vec3<valueType> const& _GridFloorCenter,
			sizeType rows, sizeType columns, valueType first_height, std::set<TagID> const& tags = {});

		// constructor using grid settings
		Grid3D(GridSettings const& _settings, valueType first_height, std::set<TagID> const& tags = {});
		
		~Grid3D();

		/*
			Grid Getters:
		*/
		// get the layer number with a given world y position.
		sizeType GetLayerNum(valueType world_pos_y) const;

		// get the grid position of a given world postion.
		GridPos GetGridPosition(Vec3<float> const& worldPos) const;

		// get the world position at the center of a given grid postion.
		Vec3<float> GetWorldPosition(GridPos const& gridpos) const;

		// get half width of the grid
		inline float GetGridExtentsX() const					{ return settings.Columns * 0.5f * settings.TileSize;  }
		
		// get half breadth of the grid
		inline float GetGridExtentsZ() const					{ return settings.Rows * 0.5f * settings.TileSize; }
		
		// get the tile size, tiles are squares; width and breadth is equal
		inline valueType GetTileSize() const					{ return settings.TileSize; }
		
		// get the grid floor center position
		inline Vec3<valueType> const& GetGridFloorCenter() const{ return settings.GridFloorCenter; }

		// get the number of columns
		inline sizeType Columns() const							{ return settings.Columns; }

		// get the number of rows
		inline sizeType Rows() const							{ return settings.Rows; }
		
		// get the number of vertical layers this 3D grid has
		inline sizeType Layers() const							{ return static_cast<sizeType>(Grids.size()); }

		// get a const reference to the full grid.
		inline std::vector<Grid2D<T>> const& GetFullGrid()	const	{ return Grids; }

		// subscript operator overload to access each 2D grid easily.
		inline Grid2D<T>& operator[](size_t layer)					{ return Grids[layer]; }
		inline Grid2D<T> const& operator[](size_t layer) const		{ return Grids[layer]; }

		/*
			Grid Modifiers:
				remember to call update to re-register the entities inside the grid
				after modifying grid settings.
		*/

		// Adds a grid layer into the 3D grid.
		void AddGridLayer(valueType height);

		// set the number of rows in the whole 3D grid
		void SetRows(sizeType in);

		// set the number of columns in the whole 3D grid
		void SetColumns(sizeType in);

		// add a tag into the 3D grid, when update is called,
		// it will check entities with that tag is in each cells
		//void AddTag(const std::string& tagname);

		// remove a tag, so that update doesnt check entities with the given tag.
		//void RemoveTag(const std::string& tagname);

		// set the square tile size of the 3D grid.
		inline void SetTileSize(valueType in)					{ settings.TileSize = in; }

		// set the floor center of the 3D grid.
		inline void SetGridFloorCenter(Vec3<valueType> in)		{ settings.GridFloorCenter = in; }

		// set the tags with a given set of tags.
		inline void SetTags(std::set<TagID> const& in)			{ registered_tags = in; }

		// set the grid settings, with a given grid settings
		inline void SetGridSettings(GridSettings const& in)		{ settings = in; }

		/*
			Grid Utilities :
		*/
		// checks if a given world coordinate is inside the 3D grid.
		bool IsValidWorldCoords(Vec3<float>const& point) const;

		// checks if a given grid coordinate is inside the 3D grid.
		bool IsValidGridCoords(GridPos const& gridpos) const;

		// update the grid by calling the physics system to check if tagged entities are inside each cell.
		void UpdateGrid();
		
		// resets the whole 3D grid, setting all cells to zero.
		void ResetGrid();

		// resets a 2D grid, with a given layer number
		void ResetLayer(sizeType layer_num);

		// std::out debug printing the 3D grid
		void DebugPrint() const;

		// Flood the grid tiles that are outside the playing map to be unpathable.
		void FloodOutsideMap(GridPos const& starting);
		void FloodIterative(GridPos const& in);
		std::array<GridPos, 8> GetTileNeighbours(GridPos const& in);
	};
	//template class Grid3D<CellInfo>;
	//template class Grid3D<bool>;
	//template class Grid3D<ListNode>;
}
#include "Grid.hpp"
#endif