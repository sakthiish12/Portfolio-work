#pragma once
#ifndef	ENTITYTAGFILTER_H
#define ENTITYTAGFILTER_H

#include "../../Systems/ECS/ECSConfig.h"

namespace BE
{
	enum class FilterType
	{
		NoFilter,
		DoesNotContainAny,
		ContainsAny,
		ContainsAll
	};

	// Depending on the Filter Type, this function compares 2 vectors(main and filter) and check if it (contains/does not contains matching elements)
	// Contains All: returns true if the main set to check matches all elements in the filter set.
	// Contains Any: returns true if the main set matches any element in the filter set.
	// DoesNotContainAny: returns true if the main set does not have any matching element with the filter set.
	template <typename T>
	bool CheckForFilter(std::vector<T> const& toCheck, std::set<T> const& Filter_set, FilterType filterset)
	{
		if (filterset == FilterType::NoFilter) return true;
		size_t matches = 0;
		for (auto const& filter : Filter_set) {
			if (std::find(toCheck.begin(), toCheck.end(), filter) != toCheck.end()) {
				matches++;
			}
		}
		bool HasFilter = matches != 0;
		bool ContainsAll = matches == Filter_set.size();
		if ((HasFilter && filterset == FilterType::ContainsAny) ||
			(!HasFilter && filterset == FilterType::DoesNotContainAny) || (ContainsAll && filterset == FilterType::ContainsAll))
		{
			return true;
		}
		return false;
	}

	class EntityTagFilter : public JPH::BodyFilter
	{
	public:
		std::set<TagID> tags;
		FilterType filter_type = FilterType::NoFilter;
		short OnlyTrigger = -1;

		EntityID ignore;
		EntityTagFilter();
		EntityTagFilter(short _OnlyTrigger) {OnlyTrigger = _OnlyTrigger;}
		//EntityTagFilter(std::vector<TagID>const& filterTags, FilterType filtertype, short isTrigger = -1);
		EntityTagFilter(std::set<std::string>const& filterTags, FilterType filtertype, short isTrigger = -1, EntityID id_to_ignore = NULL_ENTITY);


		/// Filter function. Returns true if we should collide with inBodyID
		bool ShouldCollide(const JPH::BodyID& inBodyID) const override;
		/// Filter function. Returns true if we should collide with inBody (this is called after the body is locked and makes it possible to filter based on body members)
		bool ShouldCollideLocked(const JPH::Body& inBody) const override;
	};
}

#endif