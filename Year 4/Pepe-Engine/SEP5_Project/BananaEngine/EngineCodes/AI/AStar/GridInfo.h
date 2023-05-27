#pragma once
#ifndef GRIDINFO_H
#define GRIDINFO_H

#include <Maths/Vector3D.h>

namespace BE
{
	using valueType = float;
	using sizeType = unsigned int;

	struct GridPos
	{
		sizeType row;		//z
		sizeType layer;		//y
		sizeType col;		//x

		bool operator==(const GridPos& rhs) const
		{
			return row == rhs.row && col == rhs.col && layer == rhs.layer;
		}

		bool operator!=(const GridPos& rhs) const
		{
			return row != rhs.row || col != rhs.col || layer != rhs.layer;
		}


		std::string toString() const {
			std::stringstream ss;
			ss << "GridPos: X: " << col << " Y: " << layer << " Z: " << row;
			return ss.str();
		}
	};


	// get the height different between 2 points of different layer
	inline valueType GetCellDifference(GridPos const& pt1, GridPos const& pt2) {
		return std::sqrtf(static_cast<float>((pt1.col - pt2.col) * (pt1.col - pt2.col) + (pt1.layer - pt2.layer) * (pt1.layer - pt2.layer) + (pt1.row - pt2.row) * (pt1.row - pt2.row)));
	}

	struct GridSettings
	{
		valueType TileSize = 0;
		sizeType Rows = 1;
		sizeType Columns = 1;
		Vec3<valueType> GridFloorCenter;
	};

	struct CellInfo
	{
		bool occupied = false;
		bool isWall = false;
		virtual operator bool() { return occupied; }
		virtual operator bool() const { return occupied; }
		virtual CellInfo& operator=(bool in) { occupied = in;  return *this; }
	};

	enum class GridDirection {
		Left = 0,
		Down,
		Right,
		Up
	};

	//GridDirection& operator++(GridDirection& e) {
	//	switch (e) {
	//	case GridDirection::Left: return e = GridDirection::Down;
	//	case GridDirection::Down: return e = GridDirection::Right;
	//	case GridDirection::Right: return e = GridDirection::Up;
	//	case GridDirection::Up: return e = GridDirection::Left;	
	//	}
	//}
}


#endif