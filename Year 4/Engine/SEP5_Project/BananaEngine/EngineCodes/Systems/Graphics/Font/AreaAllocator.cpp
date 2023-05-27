#include "pch.h"
#include "AreaAllocator.h"
#include "../Be_device.hpp"

namespace BE
{
	AreaAllocator::AreaAllocator()
	{
		m_width = 0;
		m_height = 0;
	}
	void AreaAllocator::Initialize(int width, int height)
	{
		m_width = width;
		m_height = height;

		m_freeAreas.emplace_back(Area{ 0, 0, width, height });
	}
	int AreaAllocator::GetTotalWidth()
	{
		return m_width;
	}
	int AreaAllocator::GetTotalHeight()
	{
		return m_height;
	}
	int AreaAllocator::accumulateAreaSize(const AreaList& areaList) const
	{
		int size = 0;
		for (const auto& area : areaList) {
			size += area.width * area.height;
		}
		return size;
	}
	Area AreaAllocator::getFreeArea(int width, int height)
	{
		Area oldArea{0,0,0,0};

		auto foundIt = m_freeAreas.findArea(width, height);
		if (foundIt != m_freeAreas.end()) {
			oldArea = *foundIt;
			m_freeAreas.erase(foundIt);
		}
		return oldArea;
	}
	void AreaAllocator::collapseFreeAreas()
	{
		if (m_freeAreas.size() < 2)
		{
			return;
		}

		int collapsed = 0;
		do {
			collapsed = 0;
			AreaList collapsedAreas;
			while (!m_freeAreas.empty()) {
				auto first = m_freeAreas.front();
				m_freeAreas.pop_front();
				while (!m_freeAreas.empty()) {
					auto other = m_freeAreas.findAdjacent(first);
					if (other != m_freeAreas.end()) {
						first.combineWith(*other);
						m_freeAreas.erase(other);
						++collapsed;
					}
					else {
						break;
					}
				}
				collapsedAreas.emplace_back(first);
			}
			m_freeAreas = collapsedAreas;
		} while (collapsed > 0);
	}
	int AreaAllocator::GetTotalAreaSize()
	{
		return m_width * m_height;
	}
	int AreaAllocator::GetFreeAreaCount()
	{
		return static_cast<int>(m_freeAreas.size());;
	}
	int AreaAllocator::GetFreeAreaSize()
	{
		return accumulateAreaSize(m_freeAreas);
	}
	int AreaAllocator::GetAllocatedAreaCount()
	{
		return static_cast<int>(m_allocatedAreas.size());
	}
	int AreaAllocator::GetAllocatedAreaSize()
	{
		return accumulateAreaSize(m_allocatedAreas);
	}
	Area AreaAllocator::Allocate(int width, int height)
	{
		Area oldArea = getFreeArea(width, height);

		if (oldArea == Area(0,0,0,0)) {
			collapseFreeAreas();
			oldArea = getFreeArea(width, height);
		}

		if (oldArea == Area(0,0,0,0)) {
			return Area(0, 0, 0, 0);
		}

		auto newArea = Area{ oldArea.x, oldArea.y, width, height };
		m_allocatedAreas.emplace_front(newArea);


		if (oldArea.width > width) {
			// Add an area to the right of newly allocated area
			m_freeAreas.emplace_back( Area{ oldArea.x + width+1, oldArea.y, oldArea.width - width-1, height });
		}
		if (oldArea.height > height) {
			// Add an area below the newly allocated area
			m_freeAreas.emplace_back( Area{ oldArea.x, oldArea.y + height+1, width, oldArea.height - height -1});
		}
		if (oldArea.width > width && oldArea.height > height) {
			// Add an area diagonally to the right and below the newly allocated area
			m_freeAreas.emplace_back( Area{ oldArea.x + width+1, oldArea.y + height+1, oldArea.width - width-1, oldArea.height - height-1 });
		}
		return newArea;
	}
	void AreaAllocator::Free(Area)
	{
		//m_allocatedAreas.remove(area);
		//m_freeAreas.emplace_back(*area);
	}
}
