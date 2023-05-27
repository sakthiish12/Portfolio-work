#include "pch.h"
#include "ECS.h"
#include "../SceneManager/SceneManager.h"
#include <Serialisation/BananaSerialisation.h>
#include <AssetManager/AssetManager.h>
#include <Components/ParticleSystem.h>

namespace BE
{
	ECSCoordinator::ECSCoordinator()
		: m_entityMgr{ ECSEntityMgr::CreateInstance() },
		  m_componentMgr{ ECSComponentMgr::CreateInstance() },
		  m_trackerMgr{ ECSTrackerMgr::CreateInstance() }
	{
		m_entitiesToDestroy.reserve(MAX_ENTITIES);

		//Register all required components
		RegisterComponent<Transform>();	
		RegisterComponent<EntityInfo>();
		RegisterComponent<Renderer>();
		RegisterComponent<AudioSource>();
		RegisterComponent<AudioListener>();
		RegisterComponent<AnimationComponent>();
		RegisterComponent<Physics>();
		RegisterComponent<GameLogic>();
		RegisterComponent<lightComponent>();
		RegisterComponent<ParticleSystem>();

		//Register all required systems here
		CompBitset cbs;
		cbs.set(GetComponentID<Transform>());
		cbs.set(GetComponentID<Physics>());
		RegisterTracker<PhysicsTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<AnimationComponent>());
		RegisterTracker<AnimationComponentTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<Renderer>());
		RegisterTracker<RendererTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<lightComponent>());
		RegisterTracker<lightComponentTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<Transform>());
		cbs.set(GetComponentID<AudioListener>());
		RegisterTracker<AudioListenerTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<GameLogic>());
		RegisterTracker<GameLogicTracker>(cbs);

		cbs.reset();
		cbs.set(GetComponentID<ParticleSystem>());
		RegisterTracker < ParticleSystemTracker>(cbs);

		//Subscribe to events
		EventMgr->SubscribeWPrio(UnloadSceneEvent, this, ECSCoordinator::OnSceneUnload, 1);
		EventMgr->SubscribeWPrio(SceneRestartingEvent, this, ECSCoordinator::OnSceneUnload, 1);

		LogCore("Created ECS System");
	}

	ECSCoordinator::~ECSCoordinator()
	{
		LogCore("Destroyed ECS System");
	}

	EntityID ECSCoordinator::CreateEntity(EntityID _eid)
	{
		//In case there's a change in ID during the creation process
		EntityID id = m_entityMgr->CreateEntity(_eid);

		//Everything requires transform and entity info component
		this->AddComponent<Transform>(id);
		this->AddComponent<EntityInfo>(id);
		this->GetComponent<EntityInfo>(id).SetFlag(EntityFlag::active, true);

		//std::cout << id << std::endl;
		return id;
	}

	void ECSCoordinator::DestroyEntity(EntityID _eid)
	{
		//Set "destroy flag" to true, this is to signal the ECS that during it's update, remove this entity completely.
		this->GetComponent<EntityInfo>(_eid).SetFlag(EntityFlag::destroy, true);
		m_entitiesToDestroy.push_back(_eid);

		//Run the same process for the children.
		//Destroy the last child by popping back - as the GetChildre()'s size will decrease while children are destroyed
		while(ECS->GetComponent<Transform>(_eid).GetChildrenCount() > 0)
		{
			DestroyEntity(ECS->GetComponent<Transform>(_eid).GetChildren().back());
		}

		//If the deleted entity has parent, detach it
		if (ECS->GetComponent<Transform>(_eid).HasParent())
		{
			EntityID parentID = ECS->GetComponent<Transform>(_eid).GetParent();
			ECS->GetComponent<Transform>(parentID).DetachChild(_eid);
		}
	}

	void ECSCoordinator::EraseEntity(EntityID _eid)
	{
		//Notify that an entity is going to be destroyed
		EntityDestroyedEvent ed(_eid);
		EventMgr->Dispatch(ed);

		m_trackerMgr->OnEntityDestroyed(_eid);
		m_componentMgr->OnEntityDestroyed(_eid);
		m_entityMgr->DestroyEntity(_eid);
	}

	EntityID ECSCoordinator::CloneEntity(EntityID _eid, bool _firstRecursion)
	{
		EntityID newID = this->CreateEntity();
		
		const auto& componentMap = ECS->GetAllComponentTypes();

		for (const auto& componentPair : componentMap)
		{
			if (ECS->HasComponent(componentPair.second, _eid))
			{
				rttr::instance sourceInst = ECS->GetComponent(componentPair.second, _eid);
				if(componentPair.second != RegistrationNameCreator<Transform>() && componentPair.second != RegistrationNameCreator<EntityInfo>())
					ECS->AddComponent(componentPair.second, newID);
				rttr::instance dstInst = ECS->GetComponent(componentPair.second, newID);
				rttr::type componentType = rttr::type::get_by_name(componentPair.second);
				CloneObject(sourceInst, dstInst, componentType);
			}
		}

		//Also clone the children!
		const auto& children = m_componentMgr->GetComponent<Transform>(_eid).GetChildren();

		//For each children in children list
		for (EntityID child : children)
		{
			//Clone that child
			EntityID clonedChild = CloneEntity(child, false);

			//Insert the cloned child child to the cloned entity
			m_componentMgr->GetComponent<Transform>(newID).AttachChild(clonedChild);
		}

		//If the cloned object has parent, then attach to its parent
		if (_firstRecursion && ECS->GetComponent<Transform>(_eid).HasParent())
			ECS->GetComponent<Transform>(newID).AttachParent(ECS->GetComponent<Transform>(_eid).GetParent());

		return newID;
	}


	


	void ECSCoordinator::DestroyTag(const std::string& _tag)
	{
		//For each entity, remove the tag from the tag list
		for (EntityID e : m_entityMgr->GetAllEntities())
			m_componentMgr->GetComponent<EntityInfo>(e).RemoveTag(_tag);

		//Destroy the tag from the tag collection
		m_entityMgr->DestroyTag(_tag);
	}

	std::vector<EntityID> ECSCoordinator::FindEntitiesBysubstr(const std::string& _substr, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<EntityID> result;
		result.reserve(128);

		for (EntityID id : m_entityMgr->GetAllEntities())
		{
			//Limit the result to this number of entities
			if (result.size() >= count)
				break;

			const EntityInfo& info = m_componentMgr->GetComponent<EntityInfo>(id);

			//If entity's name contains _substr, and
			//either the isActive is not specfied, or it matches the requested active status
			if (info.GetName().find(_substr) < info.GetName().size() && (!_isActive || info.HasFlag(EntityFlag::active) == *_isActive))
				result.push_back(id);
		}

		return result;
	}

	std::vector<EntityID> ECSCoordinator::FindEntitiesByName(const std::string& _name, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<EntityID> result;
		result.reserve(128);

		for (EntityID id : m_entityMgr->GetAllEntities())
		{
			//Limit the result to this number of entities
			if (result.size() >= count)
				break;

			const EntityInfo& info = m_componentMgr->GetComponent<EntityInfo>(id);

			//If entity's name contains _substr, and
			//either the isActive is not specfied, or it matches the requested active status
			if (info.GetName() == _name && (!_isActive || info.HasFlag(EntityFlag::active) == *_isActive))
				result.push_back(id);
		}

		return result;
	}

	std::vector<EntityID> ECSCoordinator::FindEntitiesByTag(const std::string& _tag, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<EntityID> result;
		result.reserve(128);

		for (EntityID id : m_entityMgr->GetAllEntities())
		{
			//Limit the result to this number of entities
			if (result.size() >= count)
				break;

			const EntityInfo& info = m_componentMgr->GetComponent<EntityInfo>(id);

			//If entity has the required tag, and
			//either the isActive is not specfied, or it matches the requested active status
			if (info.HasTag(_tag) && (!_isActive || info.HasFlag(EntityFlag::active) == *_isActive))
				result.push_back(id);
		}

		return result;
	}

	std::vector<EntityID> ECSCoordinator::FindEntitiesByCondition(bool (*condition)(EntityID), size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<EntityID> result;
		result.reserve(128);

		for (EntityID id : m_entityMgr->GetAllEntities())
		{
			//Limit the result to this number of entities
			if (result.size() >= count)
				break;

			const EntityInfo& info = m_componentMgr->GetComponent<EntityInfo>(id);

			//If entity has fulfills the condition, and
			//either the isActive is not specfied, or it matches the requested active status
			if (condition(id) && (!_isActive || info.HasFlag(EntityFlag::active) == *_isActive))
				result.push_back(id);
		}

		return result;
	}

	EntityID ECSCoordinator::SpawnPrefab(BEID _prefabAssetFileID, EntityID _eid)
	{
		_eid = this->CreateEntity(_eid);

		//Go through all the components in the prefab file
		const std::string& prefabFileName = AssetMgr->GetAssetFileName(_prefabAssetFileID);
		
		AssertCore(AssetMgr->GetAssetTypeID<PrefabAsset>() == AssetMgr->GetAssetTypeIDOf(_prefabAssetFileID),
			"Given Prefab File ID is not a Prefab Asset");

		//Open rapidjson doc
		rapidjson::Document doc;
		//Try to open the file and read into a string steam
		std::ifstream ifs(prefabFileName);
		if (ifs.fail())
		{
			LogCoreError("Could not open file : %s", prefabFileName.c_str());
			return NULL_ENTITY;
		}
		std::stringstream ss(std::string(std::istreambuf_iterator<char>(ifs), {}));
		if (ss.str().length() == 0) // New file detected
			return NULL_ENTITY;

		rapidjson::ParseResult result = doc.Parse<0>(ss.str().c_str());
		AssertCore(result, "Error with parsing file \"%s\"! Parse error: %d (%zu)", prefabFileName.c_str(), (int)result.Code(), result.Offset());
		ifs.close();

		rapidjson::Value& v = doc["prefab"];

		if (!v.IsObject())
		{
			LogCoreError("Could not find entity list : %s", prefabFileName.c_str());
			return NULL_ENTITY;
		}

		const auto& map = ECS->GetAllComponentTypes();
		//For each registered component check if the entity has that component
		for (const auto& component : map)
		{
			if (v.HasMember(component.second.c_str()))
			{
				if (component.first != ECS->GetComponentID<Transform>() && component.first != ECS->GetComponentID<EntityInfo>())
					ECS->AddComponent(component.second, _eid);
				rttr::instance inst = ECS->GetComponent(component.second.c_str(), _eid);
				const rapidjson::Value& componentValue = v[component.second.c_str()];
				Serialisation::LoadObjectRecur(componentValue, rttr::type::get_by_name(component.second.c_str()), _eid, inst);
			}
		}
#if 0

		//Load each component in the prefab
		for (const auto& [cid, data] : componentsToLoad)
		{
			const std::string& compName = m_componentMgr->GetAllComponentTypes().at(cid);

			//No need to add component for transform and entityinfo
			if (cid != GetComponentID<Transform>() && cid != GetComponentID<EntityInfo>())
			{
				this->AddComponent(compName, _eid);
			}

			//copy data
			rttr::instance sourceInst = *(data);
			rttr::instance dstInst = ECS->GetComponent(compName, _eid);
			rttr::type componentType = rttr::type::get_by_name(compName);
			CloneObject(sourceInst, dstInst, componentType);
		}
#endif

		return _eid;
	}

	void ECSCoordinator::SavePrefab(EntityID _eid, const std::string& _fileName)
	{
		//Create a output stream to override the output file
		std::ofstream ofs(_fileName, std::ofstream::out | std::ofstream::trunc);

		//Create a document
		rapidjson::Document doc;
		rapidjson::Value& rootValue = doc.SetObject();
		rapidjson::Value entityObj, entityMember, entityMemberKey;

		entityObj.SetObject();
		//Save each of the component data
		for (const auto& [cid, cName] : ECS->GetAllComponentTypes())
		{
			//If does not have the component
			if (!ECS->HasComponent(cName, _eid))
				continue;


			entityMember.SetObject();
			entityMemberKey.SetString(cName.c_str(), static_cast<rapidjson::SizeType>(cName.length()), doc.GetAllocator());
			Serialisation::SerializeToValueBase(rttr::instance(ECS->GetComponent(cName, _eid)), entityMember, doc);
			entityObj.AddMember(entityMemberKey, entityMember, doc.GetAllocator());
		}
		rootValue.AddMember("prefab", entityObj, doc.GetAllocator());

		rapidjson::OStreamWrapper osw(ofs);
		//Create a writer to write the document to the output stream
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		doc.Accept(writer);
		ofs.close();
	}



	//Add a component to an entity - non-templated
	void ECSCoordinator::AddComponent(const std::string& _componentName, EntityID _eid)
	{
		//Update the entity's component bitset
		m_entityMgr->AddComponent(_eid, m_componentMgr->GetComponentID(_componentName));

		//Put the actual data into the component array
		m_componentMgr->AddComponentNT(_componentName, _eid);

		//Update all the trackers to keep track of this entity
		m_trackerMgr->OnEntityComponentChanged(_eid, m_entityMgr->GetEntityComponentBitset(_eid));
	}

	//Removes a component from an entity - non-templated
	void ECSCoordinator::RemoveComponent(const std::string& _componentName, EntityID _eid)
	{
		if (_componentName == "Transform")
		{
			LogCoreError("Cannot remove Transform component");
		}
		else if (_componentName == "EntityInfo")
		{
			LogCoreError("Cannot remove EntityInfo component");
		}
		else
		{
			//Update the entity's component bitset
			m_entityMgr->RemoveComponent(_eid, m_componentMgr->GetComponentID(_componentName));

			//Remove the actual data from the component array
			m_componentMgr->RemoveComponentNT(_componentName, _eid);

			//Update all the trackers to keep track of this entity
			m_trackerMgr->OnEntityComponentChanged(_eid, m_entityMgr->GetEntityComponentBitset(_eid));
		}
	}



	void ECSCoordinator::Init()
	{

	}

	void ECSCoordinator::Awake()
	{
		
	}

	void ECSCoordinator::Update(double dt)
	{
		UNUSED(dt);

		//At end of frame, destroy all entities that are tagged to be removed.
		//Destroy backwards as GetAllEntities().size() will decrease over time.
		for (size_t i = 0; i < m_entitiesToDestroy.size(); ++i)
			EraseEntity(m_entitiesToDestroy[i]);

		m_entitiesToDestroy.clear();

#if 0
		for (int i = static_cast<int>(GetAllEntities().size()) - 1; i >= 0; --i)
		{
			if (GetComponent<EntityInfo>(GetAllEntities()[i]).HasFlag(EntityFlag::destroy))
				EraseEntity(GetAllEntities()[i]);
		}
#endif
	}

	void ECSCoordinator::Reload()
	{
		for (int i = static_cast<int>(GetAllEntities().size()) - 1; i >= 0; --i)
			EraseEntity(GetAllEntities()[i]);

		//Clear the tags
		m_entityMgr->DestroyAllTags();
	}

	void ECSCoordinator::Shutdown()
	{
		Reload();
	}

	void ECSCoordinator::OnSceneUnload(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::unloadScene || e.GetEventType() == EventType::restartScene, 
			"Wrong event, expected UnloadSceneEvent or SceneRestartingEvent, passed in %s instead.", e.GetEventName());

		//Destroy all entities backwards
		for (int i = static_cast<int>(GetAllEntities().size()) - 1; i >= 0; --i)
			EraseEntity(GetAllEntities()[i]);
	}

	void CloneObject(const rttr::instance& source, rttr::instance dst, const rttr::type& type)
	{

		for (const auto& prop : type.get_properties())
		{
			if (prop.get_metadata(BE::Serialisation::MetaDataKey::Copy).is_valid()
				&& prop.get_metadata(BE::Serialisation::MetaDataKey::Copy) == false)
			{
				continue;
			}
			if (prop.get_type().is_arithmetic() || prop.get_type() == rttr::type::get<std::string>() || prop.get_type().is_enumeration())
			{
				prop.set_value(dst, prop.get_value(source));
			}
			else if (prop.get_type().is_sequential_container())
			{
				rttr::variant srcVar = prop.get_value(source);
				rttr::variant dstVar = prop.get_value(dst);
				auto sourceVarArr = srcVar.create_sequential_view();
				auto dstVarArr = dstVar.create_sequential_view();
				if (sourceVarArr.is_dynamic())
					dstVarArr.set_size(sourceVarArr.get_size());
				for (size_t i = 0; i < sourceVarArr.get_size(); i++)
				{
					rttr::variant dstArrMemVar = dstVarArr.get_value(i).extract_wrapped_value();
					if (sourceVarArr.get_value_type().is_arithmetic() || sourceVarArr.get_value_type() == rttr::type::get<std::string>() || sourceVarArr.get_value_type().is_enumeration())
					{
						//if (sourceVarArr.get_value_type() == rttr::type::get<std::string>())
						//{
						//	rttr::variant temp1 = sourceVarArr.get_value(i);
						//	std::cout << temp1.get_type().get_name() << "\n";
						//	std::cout << temp1.extract_wrapped_value().get_value<std::string>() << "\n";
						//}
						dstArrMemVar = sourceVarArr.get_value(i).extract_wrapped_value();
					}
					else
					{
						CloneObject(sourceVarArr.get_value(i), dstArrMemVar, sourceVarArr.get_value_type());
					}
					dstVarArr.set_value(i, dstArrMemVar);
				}
				prop.set_value(dst, dstVar);
			}
			else if (prop.get_name() == "Scripts Data")
			{
				rttr::variant srcVar = prop.get_value(source);
				rttr::variant dstVar = prop.get_value(dst);
				auto sourceVarMap = srcVar.create_associative_view();
				auto dstVarMap = dstVar.create_associative_view();
				for (const auto& sourcePairVar : sourceVarMap)
				{
					rttr::variant dstKeyVar = sourceVarMap.begin().get_key().extract_wrapped_value();
					rttr::variant dstValueVar = sourceVarMap.begin().get_value().extract_wrapped_value();
					CloneObject(sourcePairVar.first.extract_wrapped_value(), dstKeyVar, dstKeyVar.get_type());
					auto itr = dstVarMap.insert(dstKeyVar, dstValueVar);
					CloneObject(sourcePairVar.second.extract_wrapped_value(), itr.first.get_value().extract_wrapped_value(), rttr::type::get_by_name(dstKeyVar.get_value<std::string>()));
				}
				prop.set_value(dst, dstVar);
			}
			else if (prop.get_type().is_associative_container())
			{
				rttr::variant srcVar = prop.get_value(source);
				rttr::variant dstVar = prop.get_value(dst);
				auto sourceVarMap = srcVar.create_associative_view();
				auto dstVarMap = dstVar.create_associative_view();
				for (const auto& sourcePairVar : sourceVarMap)
				{
					rttr::variant dstKeyVar = sourceVarMap.begin().get_key().extract_wrapped_value();
					rttr::variant dstValueVar = sourceVarMap.begin().get_value().extract_wrapped_value();
					CloneObject(sourcePairVar.first.extract_wrapped_value(), dstKeyVar, dstKeyVar.get_type());
					CloneObject(sourcePairVar.second.extract_wrapped_value(), dstValueVar, dstValueVar.get_type());
					dstVarMap.insert(dstKeyVar, dstValueVar);
				}
				prop.set_value(dst, dstVar);
			}
			else if (prop.get_type().get_properties().size() != 0)
			{
				rttr::variant dstVar = prop.get_value(dst);
				CloneObject(prop.get_value(source), dstVar, prop.get_type());
				prop.set_value(dst, dstVar);
			}
			else
				LogError("Copying property with unknown type given name %s", type.get_name());
		}
	}


}
