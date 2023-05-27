#include "pch.h"
#include "PrefabAsset.h"
#include <Systems/ECS/ECS.h>
#include <Serialisation/BananaSerialisation.h>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<PrefabAsset>(BE::RegistrationNameCreator<PrefabAsset>())
	.constructor()()
	.method(assetConstructMethodName, &PrefabAsset::ConstructFromPtr)()
	.method(serializeAssetMethodName, &PrefabAsset::Serialise)();
	registration::class_<PrefabAssetInitializer>(BE::RegistrationNameCreator<PrefabAssetInitializer>())
	.constructor()()
	.property(filePathName, &PrefabAssetInitializer::m_filePath);
}
namespace BE
{
	PrefabAsset::PrefabAsset(const std::string& _file)
	{
#if 0
		m_prefabComponents.reserve(10);
#endif
		PrefabAssetInitializer pai{ _file };

		try
		{
			this->Construct(pai);
		}
		catch (const std::runtime_error&)
		{
			throw;
		}
	}

	PrefabAsset::PrefabAsset(PrefabAsset&& _rhs) noexcept
	{
#if 0
		m_prefabComponents = std::move(_rhs.m_prefabComponents);
#endif
		m_fileName = std::move(_rhs.m_fileName);
	}

	PrefabAsset& PrefabAsset::operator=(PrefabAsset&& _rhs) noexcept
	{
#if 0
		m_prefabComponents = std::move(_rhs.m_prefabComponents);
#endif
		m_fileName = std::move(_rhs.m_fileName);
		return *this;
	}

	//Construct the data for non-templated load asset
	void PrefabAsset::Construct(const PrefabAssetInitializer& _data)
	{
		m_fileName = _data.m_filePath;

#if 0
		rapidjson::Document doc;
		//Try to open the file and read into a string steam
		std::ifstream ifs(_data.m_filePath);
		if (ifs.fail())
		{
			LogCoreError("Could not open file: %s", _data.m_filePath.c_str());
			throw std::runtime_error("Could not open the file " + _data.m_filePath);
		}
		std::stringstream ss(std::string(std::istreambuf_iterator<char>(ifs), {}));
		if (ss.str().length() == 0) // New file detected
			return;
		//Parse the input string into the rapidjson document
		doc.Parse(ss.str().c_str());

		rapidjson::Value& v = doc["prefab"];

		if (!v.IsObject())
		{
			LogCoreError("Could not find \"Prefab\" key in the JSON file");
			throw std::runtime_error("Could not find \"Prefab\" key in the JSON file");
		}

		//For each registered component check if the prefab has that component
		const auto& map = ECS->GetAllComponentTypes();
		for (const auto& component : map)
		{
			//If has the component
			if (v.HasMember(component.second.c_str()))
			{
				//Insert the data 
				rttr::variant tempVal = rttr::type::get_by_name(component.second.c_str()).create();
				m_prefabComponents[ECS->GetComponentID(component.second)] = tempVal.get_value<std::shared_ptr<IECSComponent>>();
				rttr::instance inst = *m_prefabComponents[ECS->GetComponentID(component.second)];
				rttr::type tempType = rttr::type::get_by_name(component.second.c_str());

				Serialisation::LoadObjectRecur(v[component.second.c_str()], tempType, BEID(0), inst);
			}
		}
#endif
;
	}

	void PrefabAsset::ConstructFromPtr(PrefabAssetInitializer* _data)
	{
		Construct(*_data);
	}

	PrefabAssetInitializer PrefabAsset::Serialise()
	{
#if 0
		//Create a output stream to override the output file
		std::ofstream ofs(m_fileName, std::ofstream::out | std::ofstream::trunc);

		//Create a document
		rapidjson::Document doc;
		rapidjson::Value& rootValue = doc.SetObject();
		rapidjson::Value entityObj, entityMember, entityMemberKey;

		entityObj.SetObject();
		//Save each of the component data
		for (const auto& [compID, data] : m_prefabComponents)
		{
			const std::string& compName = ECS->GetAllComponentTypes().at(compID);
			entityMember.SetObject();
			entityMemberKey.SetString(compName.c_str(), static_cast<rapidjson::SizeType>(compName.length()), doc.GetAllocator());
			Serialisation::SerializeToValueBase(rttr::instance(*data), entityMember, doc);
			entityObj.AddMember(entityMemberKey, entityMember, doc.GetAllocator());
		}
		rootValue.AddMember("prefab", entityObj, doc.GetAllocator());

		rapidjson::OStreamWrapper osw(ofs);
		//Create a writer to write the document to the output stream
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		doc.Accept(writer);
		ofs.close();
#endif
		return PrefabAssetInitializer{m_fileName};
	}



#if 0
	void PrefabAsset::UpdateComponentData(ComponentID _cid, const IECSComponent& _data)
	{
		//If contains the data, then just update it
		if (m_prefabComponents.contains(_cid))
		{
			*(m_prefabComponents.at(_cid)) = _data;
		}
		else
		{
			m_prefabComponents[_cid] = std::make_shared<IECSComponent>(_data);
		}

	}

	void PrefabAsset::UpdateComponentData(const std::string& _componentName, const IECSComponent& _data)
	{
		UpdateComponentData(ECS->GetComponentID(_componentName), _data);
	}

	
	void PrefabAsset::RemoveComponentData(ComponentID _cid)
	{
		m_prefabComponents.erase(_cid);
	}

	void PrefabAsset::RemoveComponentData(const std::string& _componentName)
	{
		RemoveComponentData(ECS->GetComponentID(_componentName));
	}

	bool PrefabAsset::HasPrefabComponent(ComponentID _cid) const
	{
		return m_prefabComponents.contains(_cid);
	}

	bool PrefabAsset::HasPrefabComponent(const std::string& _componentName) const
	{
		return HasPrefabComponent(ECS->GetComponentID(_componentName));
	}

	std::weak_ptr<IECSComponent> PrefabAsset::GetPrefabComponent(ComponentID _cid) const
	{
		//If found the data, return that value
		if (HasPrefabComponent(_cid))
			return m_prefabComponents.at(_cid);
		//Return an empty weak ptr if not found
		else
			return std::weak_ptr<IECSComponent>{};
	}

	std::weak_ptr<IECSComponent> PrefabAsset::GetPrefabComponent(const std::string& _componentName) const
	{
		return GetPrefabComponent(ECS->GetComponentID(_componentName));
	}
#endif
}

