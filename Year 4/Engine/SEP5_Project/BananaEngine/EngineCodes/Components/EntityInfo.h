#pragma once
#ifndef	ECS_ENTITY_H
#define ECS_ENTITY_H
#include "ECSComponent.h"

namespace BE
{
	//Entity flags
	enum class EntityFlag
	{
		active,							//Whether the entity is active or not
		updateWhenPaused,				//Whether to update entity when game is paused
		updateWhenNotVisible,			//Whether to update entity when the entity is not visible (eg: outside viewport)
		dontDestroyOnSceneChange,		//Do not destroy this object when changing scene
		// ^^ Only use these

		destroy,						//Used internally by ECS Update
		flagLast
	};

	class EntityInfo : public IECSComponent
	{
		std::string m_name;												//Name of the entity
		std::vector<TagID> m_tags;										//The tags that this entity has, store as TagID
		std::bitset<static_cast<size_t>(EntityFlag::flagLast)> m_flags;	//The flags that this entity has

	public:
		EntityInfo();
		EntityInfo(const EntityInfo&);

		/*
			Entity Name
		*/
		//Changes the name of the entity
		inline void SetName(std::string&& _newName)				{ m_name = std::move(_newName); }

		//Gets the name of the entity
		inline const std::string& GetName() const				{ return m_name; }



		/*
			Entity Tags
		*/
		//Add tag to the entity
		void AddTag(const std::string& _tag);

		//Remove tag from the entity
		void RemoveTag(const std::string& _tag);
		
		//Check if entity has the tag. _where is a return value for the iterator to the _tag in the m_tags container
		bool HasTag(const std::string& _tag, const std::optional<std::vector<TagID>::const_iterator*>& _where = std::nullopt) const;

		//Get all the tags that this entity has
		inline const auto& GetAllTags() const					{ return m_tags; }



		/*
			Entity Flags
		*/
		//Set the entity flag individually
		void SetFlag(EntityFlag _f, bool _b = true);			// { m_flags.set(static_cast<size_t>(_f), _b); }

		//Set the entity flag as unsigned int, will overwrite all existing flags
		void SetAllFlags(std::uint64_t _fs);						//{ m_flags = _fs; }

		//Check if the entity has a certain flag turned on
		inline bool HasFlag(EntityFlag _f) const				{ return m_flags.test(static_cast<size_t>(_f)); }

		//Get all the flags that this entity has as unsigned int
		inline std::uint64_t GetAllFlags() const				{ return static_cast<std::uint64_t>(m_flags.to_ullong()); }

		inline uint32_t GetFlagsAsUint() const					{ return m_flags.to_ulong(); }
		inline void SetFlagsAsUint(uint32_t flag)				{ m_flags = flag; }
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	};
}

#endif
