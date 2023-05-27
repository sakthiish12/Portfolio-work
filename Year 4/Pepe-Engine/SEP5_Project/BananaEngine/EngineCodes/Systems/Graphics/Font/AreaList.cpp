#pragma once
#include "pch.h"
#include "AreaList.h"

namespace BE
{
	AreaList::iterator  AreaList::findAdjacent(const Area& area)
	{
		for (auto it = begin(); it != end(); ++it) {
			if (area.isAdjacent(*it)) {
				return it;
			}
		}
		return end();
	}

	AreaList::iterator  AreaList::findArea(int width, int height)
	{
		auto foundIt = end();

		for (auto it = begin(); it != end(); ++it) {
			if ((it)->width > width && (it)->height > height) {
				foundIt = it;
				break;
			}
		}

		return foundIt;
	}
}