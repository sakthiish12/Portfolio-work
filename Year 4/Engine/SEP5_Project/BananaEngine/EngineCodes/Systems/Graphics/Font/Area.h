#pragma once
#include <pch.h>

namespace BE
{
	struct Area
	{
		int x{}, y{}, width{}, height{};
		bool isAdjacent(const Area& other) const;
		bool combineWith(const Area& other);
		bool operator==(const Area& rhs);
		void operator=(const Area& rhs);
	};
}