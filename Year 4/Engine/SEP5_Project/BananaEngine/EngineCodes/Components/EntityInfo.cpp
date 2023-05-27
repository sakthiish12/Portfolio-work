#include "pch.h"
#include "EntityInfo.h"
#include <Systems/ECS/ECS.h>
#include <Serialisation/BananaSerialisation.h>
namespace BE
{
	EntityInfo::EntityInfo() 
		: m_name{ "New Entity" }, m_tags{}, m_flags{}
	{
		//Reserve some space for the entity name
		m_name.reserve(32);
	}

	EntityInfo::EntityInfo(const EntityInfo& rhs)
		: m_name{ rhs.m_name }, m_tags{ rhs.m_tags }, m_flags{ rhs.m_flags }
	{

	}

	//Add tag to the entity
	void EntityInfo::AddTag(const std::string& _tag)
	{		
		//If tag is not added to this entity, then add that tag
		if (!HasTag(_tag))
		{
			TagID id = ECS->GetTagID(_tag);
			if(id != NULL_TAG)
				m_tags.push_back(id);
		}
		else
			LogCoreError("Entity already has the Tag \"%s\".", _tag.c_str());
	}

	//Remove tag from the entity
	void EntityInfo::RemoveTag(const std::string& _tag)
	{
		//If tag is added to this entity, then remove that tag
		decltype(m_tags)::const_iterator it;

		if (HasTag(_tag, &it))
			m_tags.erase(it);
		else
			LogCoreError("Entity does not have the Tag \"%s\".", _tag.c_str());
	}

	//Check if entity has the tag
	bool EntityInfo::HasTag(const std::string& _tag, const std::optional<std::vector<TagID>::const_iterator*>& _where) const
	{
		//If tag doesnt exist then it wont exist on any entity
		if (!ECS->IsTagCreated(_tag))
			return false;

		auto it = std::find(m_tags.begin(), m_tags.end(), ECS->GetTagID(_tag));

		//If specified a pointer to iterator to store where _tag is located in m_tags, then assign the value
		if (_where)
			**_where = it;

		return it != m_tags.end();
	}

	void EntityInfo::SetFlag(EntityFlag _f, bool _b)
	{
		m_flags.set(static_cast<size_t>(_f), _b);

		//Also set for all the children
		Transform& t = ECS->GetComponent<Transform>(ECS->FindOwnerOfComponentData(*this));

		for (EntityID child : t.GetChildren())
			ECS->GetComponent<EntityInfo>(child).SetFlag(_f, _b);
	}

	void EntityInfo::SetAllFlags(std::uint64_t _fs)
	{
		m_flags = _fs;

		//Also set for all the children
		Transform& t = ECS->GetComponent<Transform>(ECS->FindOwnerOfComponentData(*this));

		for (EntityID child : t.GetChildren())
			ECS->GetComponent<EntityInfo>(child).SetAllFlags(_fs);
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<EntityInfo>(RegistrationNameCreator<EntityInfo>())
		.constructor()
		.constructor<const EntityInfo&>()
		.property("Entity Name", &EntityInfo::m_name)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Tags", &EntityInfo::m_tags)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::EntityTag, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Flags", &EntityInfo::GetFlagsAsUint, &EntityInfo::SetFlagsAsUint)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::EntityFlag, true),
			metadata(MetaDataKey::Copy, true)
		);
}