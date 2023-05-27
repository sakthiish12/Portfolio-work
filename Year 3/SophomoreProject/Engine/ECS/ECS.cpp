// /******************************************************************************/
// /*!
// \file         ECS.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        Definitions of non-template functions for ECS
// \lines        455 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Definition of non-template ECS functions

#include <sstream>
#include "ECS.h"
#include "../Editor/Editor.h"
#include "../Graphics/Source/EditorCamera.h"

//TODO Replace std::cout with logger interaction
#define ECSDEBUG 0

ECS& ECS::GetInstance()
{
	//(25 Oct) Was using old-style new, that resulted in leaks due to no delete
	//         replaced to become static variable
	//https://www.cplusplus.com/forum/general/37113/#msg200952

	static ECS ecsInstance;
	return ecsInstance;
}

entityID ECS::findUnusedEntity()
{
	//If deleted entities set not empty, use the first ID in it
	if (!deletedEntities.empty() && !disableEntityIDReuse)
	{
		entityID toReturn{ *(deletedEntities.begin()) };
#if ECSDEBUG
		std::cout << "[ECS] Entity " << toReturn
			<< " found as unused entity" << std::endl;
#endif
		return toReturn;

	}

	//Deleted entities set empty
	//If the entitySignatures map is empty, return 0 because no entity
	//has been created yet
	if (entitySignatures.empty())
	{
#if ECSDEBUG
		std::cout << "[ECS] Entity 0 found as unused entity" << std::endl;
#endif
		return 0;
	}
	else
	{
		entityID toReturn;
		//The very unlikely case where we run out of entity IDs to use
		if ((*(std::prev(entitySignatures.end()))).first ==
			UINT_MAX)
		{
			if (disableEntityIDReuse)
				throw std::runtime_error("Maximum ID reached");
			else
			{
				//O(n) search for the first unused entity ID
				toReturn = 0;
				if (entitySignatures.find(toReturn) == entitySignatures.end())
					return toReturn;
				else
					++toReturn;
			}
		}

		//At least one entity has been created in the ECS so far
		//And the deletedEntities set is empty
		//Use the ID of the last entity in the map + 1.
		toReturn = (*(std::prev(entitySignatures.end()))).first + 1;
#if ECSDEBUG
		std::cout << "[ECS] Entity " << toReturn
			<< " found as unused entity" << std::endl;
#endif
		return toReturn;
	}
}

entityID ECS::findFirstEntityIDWithName(std::string const& name)
{
	entityID found = static_cast<entityID>(-1); //Not found
	for (auto& n : entityNames)
	{
		if (name == n.second)
		{
			found = n.first;
#if ECSDEBUG
			std::cout << "[ECS] Name " << name << " found in entity " <<
				found << std::endl;
#endif
			break;
		}
	}
	if (found == -1)
	{
#if ECSDEBUG
		std::cout << "[ECS] Unable to find entity named " << name << std::endl;
#endif
	}

	return found;
}

void ECS::reserveAll(size_t newCapacity)
{
	for (auto& cv : componentVectors)
	{
		cv->reserve(newCapacity);
	}
#if ECSDEBUG
	std::cout << "[ECS] Component Vectors reserved to contain for " <<
		newCapacity << " entities." << std::endl;
#endif
}

bool ECS::isAnEntitySelected() const
{
	return isEntitySelected;
}

void ECS::loseEntitySelection()
{
	isEntitySelected = false;
}

entityID ECS::whichEntityIsSelected() const
{
	return selectedEntityID;
}

entityID ECS::whichEntityIsBeingSaved() const
{
	return savedEntityID;
}

std::vector<entityID>
ECS::duplicateEntity(entityID toDuplicate, size_t howMany)
{
#if ECSDEBUG
	std::cout << "[ECS] Attempting to duplicate entity " <<
		toDuplicate << " " << howMany << " times" << std::endl;
#endif
	//Vector to return
	std::vector<entityID> duplicatedEntitiesVector{};

	//Abort if toDuplicate is an unused entity
	auto toFind{ entitySignatures.find(toDuplicate) };
	if (toFind == entitySignatures.end() ||
		std::count((*toFind).second.begin(),
			(*toFind).second.end(), true) == 0)
	{
#if ECSDEBUG
		std::cout << "[ECS] Cannot duplicate unused entity "
			<< toDuplicate << std::endl;
#endif
		//Return an empty vector to signify no entity cloned
		return duplicatedEntitiesVector;
	}

	//For every duplicate that needs to be made
	for (size_t duplicate{ 0 }; duplicate < howMany; ++duplicate)
	{
		//Choose an entityID
		auto newEntityID{ findUnusedEntity() };
		//Entity was picked from the deletedEntities pool, delete that
		if (!disableEntityIDReuse && !deletedEntities.empty())
		{
			deletedEntities.erase(deletedEntities.begin());
#if ECSDEBUG
			std::cout << "[ECS] Removed entity " << newEntityID
				<< " from deletedEntities" << std::endl;
#endif
		}
		//Push this entityID into the duplicatedEntitiesVector
		duplicatedEntitiesVector.emplace_back(newEntityID);

		//Clone bitset
		entitySignatures[newEntityID] = entitySignatures[toDuplicate];

		//Register duplicated entity under all systems original entity
		//registered under
		for (auto& s : systems)
		{
			if (s->checkCompatibility(entitySignatures[toDuplicate]))
				s->registerEntity(newEntityID);
			else
				s->unregisterEntity(newEntityID);
		}

		//For all components the original entity had, duplicate them
		for (size_t c{ 0 };
			c < entitySignatures[toDuplicate].size();
			++c)
		{
			if (entitySignatures[toDuplicate][c] == true)
			{
				//Duplicate component
				componentVectors[c]
					->duplicateComponent(toDuplicate, newEntityID);
			}
		}

		//(3 Jan) Duplicate(s) now has the same name as the original copy.
		renameEntity(newEntityID, getNameOfEntity(toDuplicate));

		//Give the duplicate a name based off the original copy.
		/*size_t baseDuplicate{ 0 };
		size_t multiplier{ 1 };
		size_t numberTagLength{ 2 };
		//If there is a bracketed number at the end of the entity (i.e. 'entityDuplicate(19)'), name the number accordingly (i.e. duplicate should have name 'entityDuplicate(20)')
		if (std::isdigit(*(std::prev(std::prev(entityNames[toDuplicate].end())))) &&
			*(std::prev(entityNames[toDuplicate].end())) == ')')
		{
			std::string::iterator stringIterator{ std::prev(entityNames[toDuplicate].end()) };
			while (std::isdigit(*(std::prev(stringIterator))))
			{
				stringIterator = std::prev(stringIterator);
				baseDuplicate += (static_cast<int>(*stringIterator) - '0') * multiplier;
				multiplier *= 10;
				++numberTagLength;
			}
		}
		std::string duplicateEntityName{ entityNames[toDuplicate] };
		duplicateEntityName.erase(duplicateEntityName.end() - numberTagLength, duplicateEntityName.end());
		duplicateEntityName += "(";
		duplicateEntityName += std::to_string(duplicate + baseDuplicate + 1);
		duplicateEntityName += ")";
		renameEntity(newEntityID, duplicateEntityName);*/

		//Only handle for children once the duplicate parent is properly there

		//(27 Nov) Should the original entity have a parent,
		//the duplicate entity should also have the same parent
		entityID originalParent{};
		if (doesEntityHaveParent(toDuplicate, originalParent))
		{
			setEntityParent(newEntityID, originalParent);
		}

		//(25 Nov) Call to duplicate descendants too
		//They will be descendants of the duplicated parent
		std::vector<entityID> children{};
		if (doesEntityHaveChildren(toDuplicate, children))
			for (entityID e : children)
			{
				//std::cout << "Duplicating Child: " << e << std::endl;
				//Concat vectors
				//https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
				std::vector<entityID> newChild{ duplicateEntity(e, 1) };
				//std::cout << "Child is " << *(newChild.begin()) << std::endl;
				duplicatedEntitiesVector.insert(duplicatedEntitiesVector.end(), newChild.begin(), newChild.end());

				//Set parentage of the duplicated child properly
				setEntityParent(*(newChild.begin()), newEntityID);
			}
	}
#if ECSDEBUG
	std::cout << "[ECS] New duplicate IDS: ";
	std::copy(duplicatedEntitiesVector.begin(),
		duplicatedEntitiesVector.end(),
		std::ostream_iterator<entityID>(std::cout, " "));
	std::cout << std::endl;
#endif
	return duplicatedEntitiesVector;
}

//(1 Jan) changed to only add entity to entitiesToDelete vector
void ECS::deleteEntity(entityID toDelete)
{
	entitiesToDelete.emplace_back(toDelete);
}

std::vector<bool> ECS::getSignatureOf(entityID entity) const
{
	std::vector<bool> foundSignature{};
	auto toFind = entitySignatures.find(entity);
	if (toFind != entitySignatures.end())
	{
		foundSignature = (*toFind).second;
#if ECSDEBUG
		std::cout << "[ECS] Found signature of entity "
			<< entity << std::endl;
		std::copy(foundSignature.begin(),
			foundSignature.end(),
			std::ostream_iterator<entityID>(std::cout));
		std::cout << std::endl;
#endif
	}
	else
	{
#if ECSDEBUG
		std::cout << "[ECS] Entity " << entity << " does not exist." <<
			" Cannot get signature of it" << std::endl;
#endif
	}
	return foundSignature;
}

std::string ECS::getNameOfEntity(entityID entity)
{
	if (entityNames.find(entity) != entityNames.end())
		return entityNames.find(entity)->second;
	else
		return "";
}

void ECS::renameEntity(entityID entity, std::string const& newName)
{
	//By right, the function should be like this,
	//but because of how certain functions calling it are structured,
	//it results in errornous data
	/*if (entityNames.find(entity) != entityNames.end())
		entityNames.find(entity)->second = newName;*/
	
	entityNames[entity] = newName;
}

const std::map<entityID, std::string>& ECS::getEntityNames() const
{
	return entityNames;
}

void ECS::cleanupECS()
{
	for (entityID e : entitiesToDelete)
	{
		//Unregister all systems entity is registered to
		for (auto& s : systems)
			s->unregisterEntity(e);

		//Remove entity from the entities list
		entitySignatures.erase(e);

		//Remove entity name
		entityNames.erase(e);

		//Delete map element to parent
		unparent(e);

		//Recursively delete all descendants
		std::vector<entityID> children{};
		if (doesEntityHaveChildren(e, children))
			for (entityID c : children)
				deleteEntity(c);

		//Delete all components related to this entity
		for (auto& cv : componentVectors)
			cv->removeComponent(e);

		//Add deleted entity to deleted entity set
		if (!disableEntityIDReuse)
			deletedEntities.insert(e);
#if ECSDEBUG
		std::cout << "[ECS] Entity " << e << " deleted" << std::endl;
#endif
	}

	entitiesToDelete.clear();
}

entityID ECS::createEntityFromFile
//TODO, give the new entity a name based off its filename
(const std::string& filePath, std::string const& newEntityName)
{
	//Select entity ID of an entity with no components
	entityID newEntity{findUnusedEntity()};

	//Read data from the JSON file
	SerializeManager sm;
	std::stringstream fileContents;
	sm.DeserializeFromFile(filePath, fileContents);
	rapidjson::Document doc;
	if (doc.Parse(fileContents.str().c_str()).HasParseError())
		PartyAssert(false, "Cannot parse JSON properly");

	//Attempt to find within the JSON every registered component's name
	//and try to deserialise from there
	for (auto& s : componentSerializers)
	{
		if (s->OnDeserialize(doc))
		{
			s->addComponent(newEntity);
		}
	}

	//(5 Feb) If in editor mode, try to make the entity appear where the camera
	//        presently is. Does not apply to children entities (!isAnEntitySelected)
	if (!Editor::GetInstance().isPlaying &&
		entityHas<transform2D>(newEntity) &&
		!isAnEntitySelected())
	{
		auto& t = getComponent<transform2D>(newEntity);
		t.pos.x = EditorCamera::GetInstance().GetPosition().x;
		t.pos.y = EditorCamera::GetInstance().GetPosition().y;
	}

	//Rename this entity to be of the provided name
	if (!newEntityName.empty())
		renameEntity(newEntity, newEntityName);
	else
		renameEntity(newEntity, "entity(" + std::to_string(newEntity) + ")");

	return newEntity;
}

//(7 Nov)
//Load a whole level
void ECS::loadLevel(const std::string& sourceFile)
{
	//Deselect presently selected entity
	isEntitySelected = false;

	//Clear all entities before loading
	clearAllEntities();

	//clearing all the undo 
	UiLayer::GetInstance().undoRedoIndex = -1;
	UiLayer::GetInstance().undoRedoVec.clear();

	//Read data from the JSON file
	SerializeManager sm;
	std::stringstream fileContents;
	sm.DeserializeFromFile(sourceFile, fileContents);
	rapidjson::Document doc;
	if (doc.Parse(fileContents.str().c_str()).HasParseError())
		PartyAssert(false, "Cannot parse JSON properly");
	
	//(6 Dec) Take note that at times, the child entity might precede the
	//parent entity in the JSON file (usually, because of how the JSON level
	//file is saved, this means the child entity has a lower ID than its parent)
	//Because of how setEntityParent() only works if both entities already exist,
	//this leads to faulty level loading because the parent does not exist yet.
	//A change should be made that the entity's depth (i.e. how many ancestors it has)
	//should be recorded too. Loading should now work by first loading in parentless
	//entities (i.e., depth of 0), then their children (depth of 1), and so
	//on. Deepest depth is determined by the first run through the entity list,
	//then it will repeat running through again for as how deep it goes
	//A top to bottom method
	size_t maxDepth{ 0 };
	for (size_t currentDepth{ 0 }; currentDepth <= maxDepth; ++currentDepth)
	{
		//For every object (representing an entity with key of entity's ID)
		//https://stackoverflow.com/a/34215036
		for (auto e{ doc.MemberBegin() }; e < doc.MemberEnd(); ++e)
		{
//Undefine wingdi.h preprocessor define so as to access rapidjson GetObject()
//https://stackoverflow.com/a/27064722
#undef GetObject
			//Consider depth. (Is an if statement for backward compatability with old json files)
			//Skip loading entity that does not match current depth
			//But still record if its depth exceeds maxDepth and update if so
			if (e->value.HasMember("Depth"))
			{
				size_t depthRead{ e->value.GetObject()["Depth"].GetUint() };
				if (depthRead > maxDepth) maxDepth = depthRead;
				if (depthRead != currentDepth) continue;
			}
			//Target entity to load to
			entityID loadTo = static_cast<entityID>(std::stoul(e->name.GetString()));

			//Entity Name
			std::string entityName = e->value.GetObject()["Name"].GetString();
			//Rename the entity. Moved here so parentage loading works
			//Thanks Wilfred
			renameEntity(loadTo, entityName);

			//(25 Nov) Entity parent, if it has one
			if (e->value.HasMember("Parent"))
			{
				entityID parentID = e->value.GetObject()["Parent"].GetUint();
				setEntityParent(loadTo, parentID);
			}
			//Component Data
			auto& data = e->value.GetObject()["Data"];
			//Load object into entity
			for (auto& s : componentSerializers)
			{
				if (s->OnDeserialize(data))
					s->addComponent(loadTo);
			}
			//Redefine preprocessor
#define	GetObject GetObjectW
		}
	}
}

//(6 Nov)
void ECS::writeEntityToFile
(entityID entityToSave, const std::string& targetFile)
{
	//Set the ID of the entity that is presently being saved
	savedEntityID = entityToSave;

	//Setting up the materials for serialization
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject(); //File level braces
	for (auto& s : componentSerializers)
	{
		//Save for every component type registered in the ECS
		s->OnSerialize(&writer);
	}
	writer.EndObject(); //End of file level braces

	//Open the file and write to it
	std::ofstream f(targetFile);
	f << sb.GetString();
	f.flush();
	f.close();
}

//(6 Dec) Recursive helper for determining depth
size_t findDepth(entityID ofEntity, size_t currentDepth = 0)
{
	entityID parent{ 0 };
	if (ecs.doesEntityHaveParent(ofEntity, parent))
		return findDepth(parent, ++currentDepth);
	else
		return currentDepth;
}

//(7 Nov)
void ECS::saveLevel(const std::string& targetFile)
{
	//Setting up the materials for serialization
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject(); //File level braces
	for (auto& e : entityNames)
	{
		writer.Key(std::to_string(e.first).c_str());
		{
			writer.StartObject();
			//Entity name
			writer.Key("Name");
			writer.String(e.second.c_str());

			//Entity's parent (if any)
			entityID parentID{ 0 };
			if (doesEntityHaveParent(e.first, parentID))
			{
				writer.Key("Parent");
				writer.Uint(parentID);
			}

			//Parentage depth (6 Dec), see loadLevel()
			writer.Key("Depth");
			writer.Uint(static_cast<unsigned int>(findDepth(e.first)));

			//Component data
			writer.Key("Data");
			{
				writer.StartObject();
				//Set ID of entity to save
				savedEntityID = e.first;
				for (auto& s : componentSerializers)
				{
					s->OnSerialize(&writer);
				}
				writer.EndObject();
			}
			writer.EndObject();
		}
	}
	writer.EndObject(); //End of file level braces

	//Open the file and write to it
	std::ofstream f(targetFile);
	f << sb.GetString();
	f.flush();
	f.close();
}

//(24 Nov)
void ECS::unparent(entityID child)
{
	auto toFindIterator{ entityParents.find(child) };
	//If the entity has a parent
	if (toFindIterator != entityParents.end())
	{
		entityID parentID{ entityParents[child] };
		entityParents.erase(child); //Erase data about entity's parent

		//Erase data about parent's child
		//Erasing a certain KV pair (even if the key has multiple values) in multimaps
		//https://stackoverflow.com/questions/3952476/how-to-remove-a-specific-pair-from-a-c-multimap
		auto iteratorPair{ entityChildren.equal_range(parentID) };
		for (auto i{ iteratorPair.first }; i != iteratorPair.second; ++i)
		{
			if (i->second == child)
			{
				entityChildren.erase(i);
				break;
			}
		}
	}
}

//(24 Nov)
void ECS::setEntityParent(entityID child, entityID parent)
{
	//Do not carry out parentage setting if either child or parent IDs
	//are not presently in the ECS
	if (entityNames.find(child) != entityNames.end() && entityNames.find(parent) != entityNames.end())
	{
		unparent(child);
		//To prevent an ouroboros effect that forces the engine into an inf loop,
		//if a parentage 'swap' is to be done, unparent the new parent,
		//which was previously the child of the new parent
		//i.e.
		//Changing from (0 is parent of 1) to (1 is parent of 0)
		//means unparenting 1 too
		entityID oldChildParent{ 0 };
		if (doesEntityHaveParent(parent, oldChildParent))
			if (oldChildParent == child)
				unparent(parent);

		if (child != parent)
		{
			entityParents[child] = parent; //Parent data
			//Children data
			entityChildren.emplace(std::make_pair(parent, child));
		}
	}
}

//(24 Nov)
bool ECS::doesEntityHaveParent(entityID child) const
{
	auto toFindIterator{ entityParents.find(child) };
	return (toFindIterator != entityParents.end());
}

//(24 Nov)
bool ECS::doesEntityHaveParent(entityID child, entityID& parent) const
{
	auto toFindIterator{ entityParents.find(child) };
	if (toFindIterator != entityParents.end())
	{
		parent = toFindIterator->second;
		return true;
	}
	return false;
}

//(24 Nov)
bool ECS::doesEntityHaveChildren(entityID parent) const
{
	auto toFindIterator{ entityChildren.find(parent) };
	return (toFindIterator != entityChildren.end());
}

//(25 Nov)
bool ECS::doesEntityHaveChildren(entityID parent, std::vector<entityID>& children) const
{
	auto iteratorPair{ entityChildren.equal_range(parent) };
	if (iteratorPair.first == entityChildren.end() ||
		iteratorPair.first == iteratorPair.second) //Cannot find children
		return false;
	//Found children, push children ID into vector
	for (auto i{ iteratorPair.first }; i != iteratorPair.second; ++i)
		children.push_back(i->second);
	return true;
}

void ECS::clearAllEntities()
{
	//Remove selection because no entities will remain after this
	isEntitySelected = false;

	//Unregister all entities from all systems
	for (auto& s : systems)
		s->unregisterAllEntities();

	//Clear all entity signatures
	entitySignatures.clear();

	//Clear all entity names
	entityNames.clear();

	//Clear parentage
	entityParents.clear();
	entityChildren.clear();

	//Delete all components for all entities
	for (auto& cv : componentVectors)
		cv->clearArray();

	//But clear deletedEntities set
	deletedEntities.clear();
}

//(Oct 8) Changed code to delete all entities
//instead of removing all component types and system types
//Presently is similar to a mass calling of ECS::deleteEntity()
//(Oct 14) Added function
void ECS::resetECS()
{
	//componentTypeCount = 0;
	//componentTypeNames.clear();
	//componentSerializers.clear();
	//componentVectors.clear();
	
	clearAllEntities();

	//HACK Band-aid solution to prevent exception by system re-registration
	// 	   Mainly to prevent ECS.registerSystem() from throwing exception
	// 	   due to re-registering already registered systems.
	// 	   Allows such that states can actually be revisited during execution.
	// 	   This is done so as to not change ECS.registerSystem()
	//	   Not sure if this will cause exception to be thrown due to shared_ptr
	systemCount = 0;
	systems.clear();
#if ECSDEBUG
	std::cout << "[ECS] ECS reset" << std::endl;
#endif
}