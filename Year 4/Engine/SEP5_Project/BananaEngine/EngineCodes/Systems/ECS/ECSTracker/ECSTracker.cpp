#include "pch.h"
#include "ECSTracker.h"

namespace BE
{
	bool IECSTracker::IsEntityTracked(EntityID _eid, const std::optional<std::vector<EntityID>::const_iterator*>& _where) const
	{
		auto it = std::find(m_trackedEntities.begin(), m_trackedEntities.end(), _eid);

		//If specified a pointer to iterator to store where _eid is located in m_trackedEntities, then assign the value
		if (_where)
			**_where = it;

		return it != m_trackedEntities.end();
	}
}