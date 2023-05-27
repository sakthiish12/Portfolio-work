#include "pch.h"
#include "../../Systems/PhysicsSystem/sPhysicsSystem.h"
#include "EntityTagFilter.h"
#include "../../Systems/ECS/ECS.h"

namespace BE
{
	EntityTagFilter::EntityTagFilter()
	{
	}

	//EntityTagFilter::EntityTagFilter(std::vector<TagID>const& filterTags, FilterType filtertype, short isTrigger)
	//	: tags{ filterTags }, filter_type{ filtertype }, OnlyTrigger{ isTrigger }
	//{
	//}

	EntityTagFilter::EntityTagFilter(std::set<std::string>const& filterTags, FilterType filtertype, short isTrigger, EntityID id_to_ignore)
	{
		for (auto const& string : filterTags) {
			tags.insert(ECS->GetTagID(string));
		}
		filter_type = filtertype;
		OnlyTrigger = isTrigger;
		ignore = id_to_ignore;
	}

	bool EntityTagFilter::ShouldCollide(const JPH::BodyID& inBodyID) const
	{
		EntityID id = PhySys->GetCorrespondingID(inBodyID);
		
		if (id == ignore) return false;
		//check if body is trigger
		if (OnlyTrigger >= 0) {
			bool istrigger = ECS->GetComponent<Physics>(id).GetIsTrigger();
			if ((OnlyTrigger > 0 && !istrigger) || (OnlyTrigger == 0 && istrigger)) return false;

		}
		std::vector<TagID> bodyTags = ECS->GetComponent<EntityInfo>(id).GetAllTags();
		return CheckForFilter(bodyTags, tags, filter_type);
	}

	/// Filter function. Returns true if we should collide with inBody (this is called after the body is locked and makes it possible to filter based on body members)
	bool EntityTagFilter::ShouldCollideLocked(const JPH::Body& inBody) const
	{
		EntityID id = PhySys->GetCorrespondingID(inBody.GetID());
		if (id == ignore) return false;
		//check if body is trigger
		if (OnlyTrigger >= 0) {
			bool istrigger = ECS->GetComponent<Physics>(id).GetIsTrigger();
			if ((OnlyTrigger > 0 && !istrigger) || (OnlyTrigger == 0 && istrigger)) return false;

		}
		std::vector<TagID> bodyTags = ECS->GetComponent<EntityInfo>(id).GetAllTags();
		return CheckForFilter(bodyTags, tags, filter_type);
	}

}