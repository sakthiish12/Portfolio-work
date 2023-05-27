// /******************************************************************************/
// /*!
// \file         ECS.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        Definition of ECS class and function declarations
// \lines        107 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Main Author: Ryan Wang

//The interface of the entity component system
//Also serves as the entity factory
//TODO document more for here
//TODO change push_back functions to emplace_back if possible
//TODO attempt to make system looping functions be more efficient
//TODO eventually connect with ImGui
//TODO connect with logger and assert
//TODO practice brace-initialisation instead of old C-style initialisation
//TODO increase safety of ECS use by encapsulation and friend functions (within system.h, component.h)

#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>

#include "componentArray.h"
#include "system.h"

#include "../Serializer/ISerializable.h"

//include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//include glm
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../pch.h"

#include "../Editor/UI Layer/UiLayer.h"

class ECS
{
	//FRIEND FUNCTIONS/////////////////////////////////////////////////////////
	//Grant these functions access to ECS private data

	friend void UiLayer::ShowHierarchy(bool* p_open);
	friend void UiLayer::ShowInspector(bool* p_open);
	friend void UiLayer::ShowLoadEntityDialog(bool* p_open);
	friend void UiLayer::LoadSceneEditor(bool* p_open);
	friend void UiLayer::displayHierarchyEntry(entityID e, std::map<entityID, std::string> const& names);

private:
	//SINGLETON////////////////////////////////////////////////////////////////
	
	//Pointer to the singleton instance
	//static ECS* ecsInstance;
	//static std::unique_ptr<ECS> ecsInstance;

	//Singleton. Constructors should not be accessible
	//Though, there actually needs to exist a constructor for the
	//code to compile
	ECS() = default;

	//Delete copy/move ctors so extra instances cannot be created.
	//Singletons should not be clonable
	ECS(const ECS&) = delete;
	ECS(ECS&&) = delete;

	//Singletons should not be assignable
	ECS& operator=(const ECS&) = delete;
	ECS& operator=(ECS&&) = delete;

	//EDITOR SELECTION/////////////////////////////////////////////////////////
	
	//Whether a selection is made or not
	bool isEntitySelected{ false };

	//Presently selected entity in the inspector
	entityID selectedEntityID{ 0 };

	//Flag if refreshing of entity hierarchy is needed
	bool toRefreshEntityHierarchy{ false };

	//TO FACILITATE ENTITY SAVING//////////////////////////////////////////////
	
	//Entity that is presently selected for saving
	entityID savedEntityID{ 0 };

	//COMPONENT TYPES//////////////////////////////////////////////////////////

	//Present number of componentTypes
	size_t componentTypeCount{ 0 };

	//std::vector of std::string
	//Detailing the names of component types in the ECS architecture
	std::vector<std::string> componentTypeNames;

	//std::vector of std::shared_ptr to ISerializable
	//Detailing the components for deserialisation purposes
	//TODO see if this could be used for serialisation later on
	std::vector<std::shared_ptr<componentSerializable>> componentSerializers;

	//std::vector of std::shared_ptr to componentImGui
	//Detailing the components for ImGui purposes
	std::vector<std::shared_ptr<componentImGui>> componentImGuis;

	//std::vector of std::shared_ptr to componentVectorBase
	//Detailing the component vectors presently in the ECS architecture
	std::vector<std::shared_ptr<componentVectorBase>> componentVectors;

	//ENTITY PROPERTIES////////////////////////////////////////////////////////

	//A map (usually implemented as a RB tree that is sorted by unique
	//keys and these keys contain non-unique values)
	//of entities currently in use and their std::vector<bool>'s
	//(specialised bitset-like:
	//https://en.cppreference.com/w/cpp/container/vector_bool)
	//specifying the type of components they presently have
	//Used for comparing whether they should be included in systems
	std::map<entityID, std::vector<bool>> entitySignatures;

	//A map of entity names
	//Used for editor scene hierarchy
	std::map<entityID, std::string> entityNames;

	//A map of entities' parents
	//An entity may contain multiple children, but it can only have one parent
	//A lot has to be considered for this
	//Deletion of parent entities will result in deletion of children
	//Various component data (i.e. transform follows local system)
	//depend on component data of parent.
	//Rendering in the scene hierarchy based-off 
	//The first entityID denotes the entity, the second its parent.
	//This map is for:
	// 	   1) Recursively applying ancestors' component data in order
	// 	   2) Ensuring entities only have zero or one parent
	// 	   3) Be able to fetch a vector of ancestors' entity IDs
	std::map<entityID, entityID> entityParents;

	//A multimap of entities' children
	//The first entityID denotes the entity, the second its children.
	//This multimap is for:
	// 	   1) Rendering tree nodes in ImGui Hierarchy
	// 	   2) Deleting all descendants with their ancestor
	// 	   3) Be able to fetch a vector of descendants' entity IDs
	std::multimap<entityID, entityID> entityChildren;

	//A set (like a map but without values for its keys)
	//of entityIDs that describe entities that have been deleted AFTER
	//having had components (i.e. the entity previously existed).
	//Used to keep track of which entityIDs can be re-used and
	//prevent entityID pool from running out (ID reaching 2^32 - 1)
	//(though it takes very long for entityID pool to run out)
	//(Assuming 1000 new entites per second, that will take almost 50 days)
	std::set<entityID> deletedEntities;

	//(1 Jan) A vector of entities to delete at the end of every loop
	//To prevent excepetions in debug mode
	std::vector<entityID> entitiesToDelete;

	//SYSTEMS//////////////////////////////////////////////////////////////////

	//Present number of systems
	size_t systemCount{ 0 };

	//std::vector of std::shared_ptr to systemBase
	//Detailing the systems presently in the ECS architecture
	std::vector<std::shared_ptr<systemBase>> systems;

	//HELPER FUNCTIONS/////////////////////////////////////////////////////////

	//Get the component type's ID number
	//Should it be a new type,
	//(i.e., the first call of getComponentTypeID<T>()),
	//the componentTypeCount increases.
	template <typename componentType>
	size_t getComponentTypeID();

	//Get the system's ID number, works similar to component type ID
	template <typename system>
	size_t getSystemID();

	//Get the component vector of a specified component type
	template <typename componentType>
	std::shared_ptr<componentVector<componentType>> getComponentVector();

	//Find most appropriate unused entityID
	entityID findUnusedEntity();

	//Attempt to find the first entityID with this name.
	//Returns -1 (underflows to 2^32 - 1)
	entityID findFirstEntityIDWithName(std::string const& toFind);

public:
	//Register a component type and its name into the ECS
	template <typename componentType>
	void registerComponentType();

	static ECS& GetInstance(); //singleton get instance 

	//(3 Feb) Several requirements of the ECS such as:
	//        - Component vectors must be packed
	//        - Component vectors must be sorted
	//        - Attempt to reuse entity IDs when possible
	//Mean that when new entities are created during gameplay, there's the
	//chance that the reuse of once deleted entityIDs being inserted into the
	//middle of the component vectors means that all components after get
	//shifted forward, messing up the reference pointer to the vectors since
	//said pointers do not move forward too.
	//(EntityID of the last entity + 1) will be used as the ID for new entities
	//in play mode
	//HACK entity ID reuse will be disabled in play mode instead
	bool disableEntityIDReuse{ false };

	//ECS PRE-USE//////////////////////////////////////////////////////////////

	//Reserves this much space in all component vectors
	//Best to call before first use of ECS architecture
	void reserveAll(size_t newCapacity);

	//EDITOR SELECTION/////////////////////////////////////////////////////////
	
	//Get whether an entity is presently selected in the editor
	bool isAnEntitySelected() const;

	//Lose focus on entity in inspector
	void loseEntitySelection();

	//Get which entityID is presently selected in the editor
	entityID whichEntityIsSelected() const;
	

	//ENTITY SAVING////////////////////////////////////////////////////////////

	//Get the ID of which entity is presently being saved into a JSON file
	entityID whichEntityIsBeingSaved() const;

	//SYSTEM MANIPULATION//////////////////////////////////////////////////////

	//Register a new system into the ECS and return 
	template <typename system>
	std::shared_ptr<system> registerSystem(bool requireComponentMatch = false);

	//Make it such that the system requires this component in an entity
	//for the entity to be considered if TRUE is input
	//Make system not require if FALSE is input
	//The system must already be registered or else an exception will be thrown
	template <typename system, typename componentType>
	void setSystemComponentRequirement(bool setToRequire = true);

	//ENTITY MANIPULATION//////////////////////////////////////////////////////

	//Check if entity has component type
	template <typename componentType>
	bool entityHas(entityID entity);

	//Duplicates a given entity a certain number of times
	//Returns a vector of entityIDs that have been cloned
	//(25 Nov) Also duplicate descendants, placing them as descendants of original parent
	std::vector<entityID>
		duplicateEntity(entityID toDuplicate, size_t howMany = 1);

	//Deletes all components related to this entity
	//(25 Nov) and delete the entity's descendants
	//(1 Jan) schedules to delete entity instead so as to prevent
	//         runtime errors in debug mode
	//        previous code was migrated to cleanupECS()
	void deleteEntity(entityID toDelete);

	//Get signature of an entity
	//this function should not allow the signature to be modified
	//The function returns an all false bitset if entity cannot be found
	std::vector<bool> getSignatureOf(entityID entity) const;

	//Get name of entity
	//Returns an empty string if no such entity exists
	std::string getNameOfEntity(entityID of);

	//Rename entity of ID
	void renameEntity(entityID toRename, std::string const& newName);

	//Get the map to names of all entities
	//Unmodifiable from here
	const std::map<entityID, std::string>& getEntityNames() const;

	//(1 Jan) Cleanups the ECS. Only to be called at the end of every game loop
	//Deletes all entities that are marked for deletion during the loop
	void cleanupECS();

	//ENTITY PARENTAGE/////////////////////////////////////////////////////////

	//Make entity not have a parent anymore (if it has one)
	void unparent(entityID child);
	
	//Set entity's parent to be of another entity.
	//Trying to set the entity's parent to be itself will make the entity
	//not have a parent anymore.
	//Should the entity already have a previous parent, sever its ties
	//to that previous parent by unlinking them in map and multimap
	void setEntityParent(entityID child, entityID parent);

	//Does the entity have a parent?
	bool doesEntityHaveParent(entityID child) const;

	//Does the entity have a parent?
	//Overload with a reference to the parent's ID if found
	bool doesEntityHaveParent(entityID child, entityID& parent) const;

	//Does the entity have children?
	bool doesEntityHaveChildren(entityID parent) const;

	//Does the entity have children?
	//Overload with a reference to vector of childrens' IDs if found
	//Function emplaces back to children found
	bool doesEntityHaveChildren(entityID parent, std::vector<entityID>& children) const;

	//SERIALIZATION////////////////////////////////////////////////////////////

	//Read from a specified JSON file and create an entity out of it
	//Output will be the new entity's ID
	//(17 Oct) Able to supply name of new entity
	//         If no name is supplied, default "entity<ID#>" will be used
	entityID createEntityFromFile
	(const std::string& filePath, std::string const& newEntityName = "");

	//(6 Nov)
	//Write to a specified JSON file the details of a single entity
	void writeEntityToFile
	(entityID entityToSave, const std::string& targetFile);

	//(7 Nov)
	//Write the details of every entity present in the level into a JSON file
	//(25 Nov)
	//Saving now takes into account parentage
	void saveLevel(const std::string& targetFile);

	//(7 Nov)
	//Load a level and all its entities from a JSON file
	//(25 Nov)
	//Loading now takes into account parentage
	void loadLevel(const std::string& sourceFile);

	//INDIVIDUAL COMPONENT MANIPULATION////////////////////////////////////////

	template <typename componentType>
	componentType& getComponent(entityID ofEntity);

	//TODO for easier integration with C# as suggested by TA Jun Yi (19 Nov)
	//componentType can be stored as an enum
	//Default-initialises a component in an entity, based on componentType ID
	//void addComponent(entityID toEntity, uint32_t componentType, std::string const& entityName = "");

	//Add a component of a certain type to an entity
	//The component may be added to a new entity or an existing one
	//(17 Oct) Able to supply name of entity.
	//         Renames the entity when a name is supplied.
	//         If a name is not supplied,
	//         Leaves name as is if adding component to existing entity
	//         Gives default name "entity<ID#>" to new entity.
	template <typename componentType>
	void addComponent
	(entityID toEntity, 
		componentType const& withComponent,
		std::string const& entityName = "");

	//Add component to a new entity
	//ECS will automatically decide the new entity's ID and return that
	//(16 Oct) Able to supply name of new entity.
	//         Not inputting it results in new entity having default name of
	//         "entity<ID#>", where "<ID#>" represents new entity's ID.
	template <typename componentType>
	entityID addComponent
	(componentType const& withComponent, 
		std::string const& newEntityName = "");

	//Remove a component of a certain type from an entity
	//Returns true if it was the last component removed, therefore deleting
	//the entity. Returns false if ther are still components remaining
	template <typename componentType>
	bool removeComponent(entityID fromEntity);

	//ECS POST-USE/////////////////////////////////////////////////////////////

	//Just clears all entities in the ECS presently
	void clearAllEntities();

	//Call upon restarting the whole ECS before use again
	//Also calls clearAllEntities()
	void resetECS();
};

//Macro directive to get the ECS faster
#define ecs ECS::GetInstance()

//Function template definitions here. For non-template defs, go to ECS.cpp
#include "ECS.tpp"

#endif