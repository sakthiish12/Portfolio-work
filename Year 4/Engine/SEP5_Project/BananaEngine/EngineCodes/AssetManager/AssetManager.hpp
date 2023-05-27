#pragma once
namespace BE
{
	template <typename AssetType, typename... CtorArgs>
	AssetID AssetManager::LoadAsset(AssetID _assetID, const std::string& _filePath, size_t _assetFlags, CtorArgs&&... _args)
	{
		//Converts AssetType to its Asset Type ID
		AssetTypeID aType = GetAssetTypeID<AssetType>();
		AssertCore(m_assetTypesList.count(aType) != 0, "Asset Type \"%s\" was not registered", typeid(AssetType).name());

		//Convert filepath to using /
		std::string fp{ _filePath };
		ConvertFilePathSlash(fp);

		//See if the asset ID already exists
		if (IsAssetDataLoaded(_assetID))
		{
			LogCoreError("Duplicate Asset ID %llu found, asset not created", *_assetID);
			return _assetID;
		}

		//See if the file path has already been loaded
		if (IsAssetDataLoaded(fp))
		{
			LogCoreError("Already loaded the asset \"%s\"", _filePath.c_str());
			return m_fileName_to_assetID.at(fp);
		}

		try
		{
			//Attempt to create the asset data
			AssetType data(fp, std::forward<CtorArgs>(_args)... );

			//Update the mapping
			m_assetID_to_fileName.emplace(_assetID, fp);
			m_fileName_to_assetID.emplace(fp, _assetID);
			m_assetID_to_type.emplace(_assetID, std::make_pair(aType, _assetFlags));

			//Move data into the m_data container
			GetAssetDataContainer<AssetType>()->InsertData(_assetID, std::move(data));

			return _assetID;
		}
		//If asset loading fail, throw an exception in AssetType's constructor
		catch (const std::exception& e)
		{
			LogCoreError("Failed to create the asset \"%s\": %s.", fp.c_str(), e.what());
			return NULL_ASSET;
		}

	}

	template <typename AssetType, typename... CtorArgs>
	AssetID AssetManager::LoadAsset(const std::string& _filePath, size_t _assetFlags, CtorArgs&&... _args)
	{
		//Converts AssetType to its Asset Type ID
		AssetTypeID aType = GetAssetTypeID<AssetType>();
		AssertCore(m_assetTypesList.count(aType) != 0, "Asset Type \"%s\" was not registered", typeid(AssetType).name());

		//Generate a random ID
		AssetID aID{};
		return LoadAsset<AssetType>(aID, _filePath, _assetFlags, std::forward<CtorArgs>(_args)...);
	}

	template <typename AssetType>
	AssetType& AssetManager::GetAssetData(AssetID _assetID)
	{
		return GetAssetDataContainer<AssetType>()->GetData(_assetID);
	}

	template <typename AssetType>
	AssetType& AssetManager::GetAssetData(const std::string& _fileName)
	{
		return GetAssetData<AssetType>(GetAssetID(_fileName));
	}

	template <typename AssetType>
	bool AssetManager::IsAssetDataLoadedForType(AssetID _assetID) const
	{
		//Check if the asset ID even exists in the global asset list first
		if (IsAssetDataLoaded(_assetID))
		{
			//Converts AssetType to its Asset Type ID
			AssetTypeID aType = GetAssetTypeID<AssetType>();
			AssertCore(m_assetTypesList.count(aType) != 0, "Asset Type \"%s\" was not registered", typeid(AssetType).name());

			//Get the container that the asset data is stored in
			AssetDataContainer<AssetType> const * const dataCont = GetAssetDataContainer<AssetType>();
			return dataCont->m_assetIDToIndexMap.count(_assetID) != 0;
		}
		else
			return false;

	}

	template <typename AssetType>
	bool AssetManager::IsAssetDataLoadedForType(const std::string& _fileName) const
	{
		//Check if the file name even exists in the global asset list first
		if (IsAssetDataLoaded(_fileName))
		{
			std::string fp = _fileName;
			ConvertFilePathSlash(fp);

			//Get the asset ID for this file name
			AssetID aID = m_fileName_to_assetID.at(fp);
			return IsAssetDataLoadedForType<AssetType>(aID);
		}
		else
			return false;
	}

	template <typename AssetType>
	AssetDataContainer<AssetType>* AssetManager::GetAssetDataContainer()
	{
		AssetTypeID aType = GetAssetTypeID<AssetType>();
		AssertCore(m_assetTypesList.count(aType) != 0, "Asset Type \"%s\" was not registered", typeid(AssetType).name());
		return dynamic_cast<AssetDataContainer<AssetType>*>(m_assetData.at(aType));
	}

	template <typename AssetType>
	AssetDataContainer<AssetType> const* AssetManager::GetAssetDataContainer() const
	{
		AssetTypeID aType = GetAssetTypeID<AssetType>();
		AssertCore(m_assetTypesList.count(aType) != 0, "Asset Type \"%s\" was not registered", typeid(AssetType).name());
		return dynamic_cast<AssetDataContainer<AssetType> const * const>(m_assetData.at(aType));
	}

	template <typename AssetType, typename AssetHelper>
	typename AssetManager::AssetTypeID AssetManager::RegisterAssetType(std::vector<std::string>&& _fileExtensions)
	{
		static_assert(std::is_move_assignable<AssetType>::value, "Static Assertion failed: Asset Type must be move assignment - T& operator=(T&&)");
		static_assert(std::is_move_constructible<AssetType>::value, "Static Assertion failed: Asset Type must be move constructible - T(T&&)");
		static_assert(std::is_base_of<IAsset, AssetType>::value, "Static Assertion failed: Asset Type must inherit IAsset");
		static_assert(std::is_default_constructible<AssetType>::value, "Static Assertion failed: Asset Type must be default constructible - T(T&&)");

		AssetTypeID aType = GetAssetTypeID<AssetType>();
		AssertCore(m_assetTypesList.count(aType) == 0, "Asset Type \"%s\" was already registered", typeid(AssetType).name());

		//Truncating the namespace and class/struct from the type name
		std::string aName{ typeid(AssetType).name() };
		size_t i = aName.find_last_of(" :");

		//Store the asset ID - asset type name, and assetID - asset data array mapping
		const auto& [it, res] = m_assetTypesList.emplace(aType, aName.substr(i + 1, aName.size() - i - 1));
		m_assetTypesList_reverse.emplace(it->second, it->first);

		//Store file extensions
		m_fileExtensions.emplace(aType, std::move(_fileExtensions));

		//Create a new container
		m_assetData.emplace(aType, new AssetDataContainer<AssetType>);
		m_assetID_to_helper.emplace(aType, RegistrationNameCreator<AssetHelper>());

		UNUSED(res);
		return aType;
	}

	template <typename AssetType>
	typename AssetManager::AssetTypeID AssetManager::GetAssetTypeID() const
	{
		static AssetTypeID newID = m_nextID++;
		return newID;
	}
}