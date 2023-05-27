#pragma once
#ifndef	ECS_ENTITY_MGR_H
#define ECS_ENTITY_MGR_H

#include "../ECSConfig.h"

namespace BE
{
	class ECSEntityMgr : public Singleton<ECSEntityMgr>
	{
		/*
			Entity tracking
		*/
		//An array that contains all entity IDs that are living
		std::vector<EntityID> m_allEntities{};

		//A collection of all the tags currently in use, maps from string to tag ID
		std::unordered_map<std::string, TagID> m_allTags{};

		//Component bitset for all entities - Each bit represents whether or not the entity has that component
		std::array<CompBitset, MAX_ENTITIES> m_entityComponentBitset{};

		/*
			Entity ID - Component bitset array index mapping
		*/

		//Maps from entity ID to index of component bitset array
		std::unordered_map<EntityID, size_t> m_IDToIndexMap;

		//Maps from index of component bitset array to entity ID
		std::unordered_map<size_t, EntityID> m_indexToIDMap;





		/*
			Constructor and Destructors	
		*/
		ECSEntityMgr();
		~ECSEntityMgr();



		/*
			Entities related functions
		*/
		//Creates an entity with a known ID, or leave parameter empty for randomly generated ID
		EntityID CreateEntity(EntityID _eid);

		//Removes the entity from the list of entities in use
		void DestroyEntity(EntityID _eid);
		


		/*
			Entity components
		*/
		//Add the specified component for the specified entity
		void AddComponent(EntityID _eid, ComponentID _cid);

		//Remove the specified component from the specified entity
		void RemoveComponent(EntityID _eid, ComponentID _cid);

		//Checks if entity has a specified component
		bool HasComponent(EntityID _eid, ComponentID _cid) const;

		//Get all the components that the entity has
		const CompBitset& GetEntityComponentBitset(EntityID _eid) const;



		/*
			Tags list
		*/
		//Create a new tag with a known ID, or leave 2nd parameter empty for randomly generated ID
		TagID CreateTag(std::string&& _tag, TagID _id);

		//Destroy tag by name
		void DestroyTag(const std::string& _tag);

		//Get the tag string using the ID, 0 if tag isn't created
		TagID GetTagID(const std::string& _tag) const;

		//Rename tag - if tag id doesn't exist, then just create a new one
		TagID RenameTag(const std::string& _oldTag, std::string&& _newTag);

		//Checks if a tag is created
		bool IsTagCreated(const std::string& _tag) const;

		//Get tag string by ID
		const std::string& GetTag(TagID id) const;

		//Destroy all the tags
		void DestroyAllTags();

		//Serialise and Deserialise all the tags - must call these before the EntityInfo's serialise/deserialise
		//Basically save the TagID mapping to Tag String
		void SerialiseAllTags(rapidjson::Document& d,rapidjson::Value& rootValue);
		void DeserialiseAllTags(rapidjson::Document& d, rapidjson::Value& rootValue);
		



		/*
			Getters
		*/
		//Get all entities that are currently in use
		inline const auto& GetAllEntities() const	{ return m_allEntities; }

		//Get the number of entities that are currently in use
		inline size_t GetEntitiesCount() const		{ return m_allEntities.size(); }

		//Get all the tags that are in use
		inline const auto& GetAllTags() const		{ return m_allTags; }



		//Only ECS coordinator can access this class
		friend class ECSCoordinator;
		friend class Singleton<ECSEntityMgr>;
	};
}
#endif
