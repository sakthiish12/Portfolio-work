#include "pch.h"
#include "Area.h"

namespace BE
{
	bool Area::isAdjacent(const Area& other) const
	{
		if (x == other.x && width == other.width && y + height == other.y) {
			// Other is immediately below
			return true;
		}

		if (x == other.x && width == other.width && other.y + other.height == y) {
			// Other is immediately above
			return true;
		}

		if (y == other.y && height == other.height && x + width == other.x) {
			// Other is immediately to the right
			return true;
		}

		if (y == other.y && height == other.height && other.x + other.width == x) {
			// Other is immediately to the left
			return true;
		}

		return false;
	}

	bool Area::combineWith(const Area& other)
	{
		if (x == other.x && width == other.width && y + height == other.y) {
			// Other is immediately below
			height += other.height;
			return true;
		}

		if (x == other.x && width == other.width && other.y + other.height == y) {
			// Other is immediately above
			y -= other.height;
			height += other.height;
			return true;
		}

		if (y == other.y && height == other.height && x + width == other.x) {
			// Other is immediately to the right
			width += other.width;
			return true;
		}

		if (y == other.y && height == other.height && other.x + other.width == x) {
			// Other is immediately to the left
			x -= other.width;
			width += other.width;
			return true;
		}

		return false;
	}
	bool Area::operator==(const Area& rhs)
	{
		return (x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height);
	}
	void Area::operator=(const Area& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		width = rhs.width;
		height = rhs.height;
	}
}
