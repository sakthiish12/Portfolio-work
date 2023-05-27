// /******************************************************************************/
// /*!
// \file         ECS.tpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Nov, 2021
// \brief        Definitions of template functions for ECS
// \lines        215 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Author: Ryan Wang
//Definitions of ECS template functions

//TODO Replace std::cout with logger interaction
#define ECSDEBUG 0

template <typename componentType>
void ECS::registerComponentType()
{
	//(26 Oct) Update component type IDs if needed, thanks Chun Ren
	//Not including this line may cause irregularity in component type IDs
	//that will lead to peculiarities and possible exceptions by ECS
	getComponentTypeID<componentType>();

	//Get the name of this new component type
	std::string newComponentType{ typeid(componentType).name() };

	//Make a new component vector
	componentVectors.emplace_back
	(std::make_shared<componentVector<componentType>>());

	size_t newSize = componentVectors.size();

	//Add name of new component type into names
	componentTypeNames.emplace_back(newComponentType);

	//Add data of serializer
	componentSerializers.emplace_back
	(std::make_shared<componentType>());

	//Add data of ImGui
	componentImGuis.emplace_back
	(std::make_shared<componentType>());

	//Lengthen entity signatures, thanks Chun Ren for finding out
	for (auto& p : entitySignatures)
		p.second.resize(newSize);

#if ECSDEBUG
	std::cout << "New vector of component type \"" <<
		newComponentType << "\" pushed\n";
	std::cout << "Current component types: " <<
		componentVectors.size() << std::endl;
#endif
}

template <typename componentType>
size_t ECS::getComponentTypeID()
{
	//Using static variables in templates
	//https://www.geeksforgeeks.org/templates-and-static-variables-in-c/
	static size_t componentID{ componentTypeCount++ };
	return componentID;
}

template <typename system>
size_t ECS::getSystemID()
{
	static size_t systemID{ systemCount++ };
	return systemID;
}

template <typename componentType>
std::shared_ptr<componentVector<componentType>> ECS::getComponentVector()
{
	return std::static_pointer_cast<componentVector<componentType>>
		(componentVectors[getComponentTypeID<componentType>()]);
}

template <typename system>
std::shared_ptr<system> ECS::registerSystem(bool requireComponentMatch)
{
	//Should there not be a system in the architecture yet
		//push into systems vector
	if (getSystemID<system>() >= systems.size())
	{
		auto newSys{ std::make_shared<system>(requireComponentMatch) };
		systems.push_back(newSys);
#if ECSDEBUG
		std::cout << "New system pushed\n";
		std::cout << "Current systems: " << systems.size() << std::endl;
#endif
		return newSys;
	}
	else
	{
		//TODO make this return (something like) instead
		//return systems[getSystemId<system>()];
		//Can't use it right now because systems contains shared_ptr
		//to system_base
		throw std::runtime_error("System already registered");
	}
}

template <typename system, typename componentType>
void ECS::setSystemComponentRequirement(bool setToRequire)
{
	//Should there not be a system in the architecture yet
		//push into systems vector
		//This new system does not require an entity to have only the
		//components required to be registered into the system
	if (getSystemID<system>() >= systems.size())
	{
		//TODO throw exception instead
		auto newSys{ std::make_shared<system>() };
		systems.push_back(newSys);
#if ECSDEBUG
		std::cout << "New system pushed\n";
		std::cout << "Current systems: " << systems.size() << std::endl;
#endif
	}

	//Should there not be a component of this type yet in the architecture,
	//push a vector for a new type of components into the architecture
	if (getComponentTypeID<componentType>() >=
		componentVectors.size())
	{
		registerComponentType<componentType>();
	}

	if (setToRequire)
		systems[getSystemID<system>()]
		->setComponentBit(getComponentTypeID<componentType>());
	else
		systems[getSystemID<system>()]
		->resetComponentBit(getComponentTypeID<componentType>());

	std::shared_ptr<systemBase>
		affectedSystem{ systems[getSystemID<system>()] };

	//Register or unregister entities from the system
	for (auto& e : entitySignatures)
	{
		if (affectedSystem->
			checkCompatibility(entitySignatures[e.first]))
			affectedSystem->registerEntity(e.first);
		else
			affectedSystem->unregisterEntity(e.first);
	}
}

template <typename componentType>
bool ECS::entityHas(entityID entity)
{
#if ECSDEBUG
	//CALLED EVERY FRAME, VERBROSE
	/*std::cout << "[ECS] Checking if entity " << entity <<
		" has component type #" << getComponentTypeID<componentType>() << 
		std::endl;*/
#endif
	//Should there not be a component of this type yet in the architecture,
	//push a vector for a new type of components into the architecture
	if (getComponentTypeID<componentType>() >=
		componentVectors.size())
	{
		registerComponentType<componentType>();
	}
	std::string type
	{ componentTypeNames[getComponentTypeID<componentType>()] };
	bool doesIt {false};
	if (entitySignatures.find(entity) != entitySignatures.end())
		doesIt = entitySignatures.find(entity)->second[getComponentTypeID<componentType>()];
	//{ entitySignatures[entity][getComponentTypeID<componentType>()] };
#if ECSDEBUG
	/*std::cout << "[ECS] Entity " << entity <<
		(doesIt ? " has " : " does not have ") << "component of type \"" <<
		type << "\"" << std::endl;*/
#endif		
	return doesIt;
}

template <typename componentType>
componentType& ECS::getComponent(entityID ofEntity)
{
#if ECSDEBUG
	std::cout << "[ECS] Getting component of entity " <<
		ofEntity << std::endl;
#endif
	return getComponentVector<componentType>()->getComponent
	(ofEntity);
}

template <typename componentType>
void ECS::addComponent
(entityID toEntity,
	componentType const& withComponent,
	std::string const& entityName)
{
	//Should this function add to a previously deleted entity,
		//remove that entity from the deleted entities set
	auto toFind{ deletedEntities.find(toEntity) };
	if (toFind != deletedEntities.end())
	{
		deletedEntities.erase(toFind);
#if ECSDEBUG
		std::cout << "[ECS] Entity " << toEntity << "created."
			<< std::endl;
		std::cout << "[ECS] Entity " << toEntity
			<< " removed from deletedEntities" << std::endl;
#endif
	}
	//Should there not be a component of this type yet in the architecture,
	//push a vector for a new type of components into the architecture
	if (getComponentTypeID<componentType>() >=
		componentVectors.size())
	{
		registerComponentType<componentType>();
	}

#if ECSDEBUG
	std::string componentTypeName
	{ componentTypeNames[getComponentTypeID<componentType>()] };
	std::cout << "[ECS] Adding component " << componentTypeName <<
		" to #" << toEntity << std::endl;
#endif

	//Adds component of type for an entity
	getComponentVector<componentType>()->
		addComponent(toEntity, withComponent);

	//Get a reference to the entity's signature of components
	//(If it's not in the signatures map yet, it'll be initialised
	//as an all false set)
	std::vector<bool>& entitySignature{ entitySignatures[toEntity] };

	//Resize if the signature is not long enough
	size_t componentTypeID{ getComponentTypeID<componentType>() };
	if (entitySignature.size() < componentTypeCount)
		entitySignature.resize(componentTypeCount);

	//Note that the entity contains this component for inclusion in
	//systems if needed
	entitySignature[componentTypeID] = true;

	//Rename entity if name is supplied in parameter
	if (!entityName.empty())
		entityNames[toEntity] = entityName;
	else //Name not supplied
	{
		//Only for new entities, give a default name
		if (entityNames[toEntity].empty())
		{
			entityNames[toEntity] = "entity" + std::to_string(toEntity);
		}
	}

	//Check compatibility of entity with systems
	for (auto& s : systems)
	{
		if (s->checkCompatibility(entitySignature))
			s->registerEntity(toEntity);
		else
			s->unregisterEntity(toEntity);
	}
}

template <typename componentType>
entityID ECS::addComponent
(componentType const& withComponent, std::string const& newEntityName)
{
	//Determine the entity to select.
	//Should deletedEntities be empty, pick the ID of last entity existing
	//and add by 1.
	//Should it not be empty, pick the ID of the first entity in it.
	entityID newEntityID{ findUnusedEntity() };
#if ECSDEBUG
	std::cout << "[ECS] Entity " << newEntityID << " created."
		<< std::endl;
#endif

	//Call addComponent(entityID, componentType&);
	addComponent(newEntityID, withComponent, newEntityName);

	return newEntityID;
}

template <typename componentType>
bool ECS::removeComponent(entityID fromEntity)
{
	//Should there not be a component of this type yet in the architecture,
	//push a vector for a new type of components into the architecture
	if (getComponentTypeID<componentType>() >=
		componentVectors.size())
	{
		registerComponentType<componentType>();
	}

	//Get a reference to the entity's signature of components
	//Update it
	std::vector<bool>& entitySignature{ entitySignatures[fromEntity] };
	//Resize if the signature is not long enough
	size_t componentTypeID = getComponentTypeID<componentType>();
	if (entitySignature.size() <= componentTypeID)
		entitySignature.resize(componentTypeID + 1);
	entitySignature[componentTypeID] = false;

	//Check compatibility of entity with systems
	for (auto& s : systems)
	{
		if (s->checkCompatibility(entitySignature))
			s->registerEntity(fromEntity);
		else
			s->unregisterEntity(fromEntity);
	}

	//Remove component of type from an entity
	getComponentVector<componentType>()->
		removeComponent(fromEntity);

	//Should the signatue be all 0s as a result of this component removal,
	//this entity is effectively deleted
	if (std::count(entitySignature.begin(), entitySignature.end(), true)
		== 0)
	{
		deleteEntity(fromEntity);
		return true;
	}

	return false;
}