#pragma once
#include <pch.h>
#include <list>
#include "Area.h"
#include "AreaList.h"

namespace BE
{
	class AreaAllocator
	{
	public:
		AreaAllocator();
		void Initialize(int width, int height);

		int GetTotalWidth();
		int GetTotalHeight();
		int GetTotalAreaSize();
		int GetFreeAreaCount();
		int GetFreeAreaSize();
		int GetAllocatedAreaCount();
		int GetAllocatedAreaSize();
		
		Area Allocate(int width, int height);
		void Free(Area area);
	protected:
		int m_width{};
		int m_height{};

		AreaList m_freeAreas;
		AreaList m_allocatedAreas;

		int accumulateAreaSize(const AreaList& areaList) const;
		Area getFreeArea(int width, int height);
		void collapseFreeAreas();
	};
}