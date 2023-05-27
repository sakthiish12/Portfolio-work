#pragma once
#include <pch.h>
#include "Area.h"
#include <list>

namespace BE
{
	class AreaList : public std::list<Area>
	{
	public:
		AreaList::iterator  findAdjacent(const Area& area);
		AreaList::iterator  findArea(int width, int height);
	};
}