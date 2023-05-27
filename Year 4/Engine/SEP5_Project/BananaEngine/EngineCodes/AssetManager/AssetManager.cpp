#include "pch.h"
#include <EventDispatcher/EventDispatcher.h>
#include "AssetManager.h"

#include <Serialisation/BananaSerialisation.h>
#include <Debugger/StopWatch.h>
#include "../Systems/RendererSystem/RendererSystem.h"

#pragma warning ( push )
#pragma warning ( disable: 4840 )
namespace BE
{
	AssetManager::AssetManager()
	{
		RegisterAssetType<AudioClip, AudioClipInitialiser>({ "wav", "mp3" });
		RegisterAssetType<BeModel, ModelFileInitialiser>({ "fbx","bananaModel" });
		RegisterAssetType<Texture, TextureInitialiser>({ "png", "dds" });
		RegisterAssetType<PrefabAsset, PrefabAssetInitializer>({ "json","prefab" });
		RegisterAssetType<Font, FontInitialiser>({ "ttf" });


		//Shaun - disabled unloading asset from scene to scene since we are going to have few scenes currently
		scanDirectoryForConversion("../Assets", RegenerationAssetType::both, true); //@donavern
		//EventMgr->SubscribeWPrio(UnloadSceneEvent, this, AssetManager::OnSceneUnload, 0);

		LogCore("Created AssetManager System");
	}

	AssetManager::~AssetManager()
	{
		//Free all the containers
		for (const auto& container : m_assetData)
			delete container.second;

		//This should call the asset type's destructor

		LogCore("Destroyed AssetManager System");
	}

	AssetID AssetManager::LoadAsset(const std::string& _assetType, AssetID _assetID, const std::string& _filePath, size_t _assetFlags)
	{
		AssetTypeID aTypeID = m_assetTypesList_reverse.at(_assetType);
		AssertCore(m_assetTypesList.count(aTypeID) != 0, "Asset Type \"%s\" was not registered", _assetType.c_str());

		//Convert filepath to using /
		std::string fp{ _filePath };
		ConvertFilePathSlash(fp);

		//See if the asset ID already exists
		if (IsAssetDataLoaded(_assetID))
		{
			LogCoreError("Duplicate Asset ID %llu found, asset not created", _assetID);
			return _assetID;
		}

		//See if the file path has already been loaded
		if (IsAssetDataLoaded(fp))
		{
			LogCoreError("Already loaded the asset \"%s\"", fp.c_str());
			return m_fileName_to_assetID.at(fp);
		}

		try
		{
			rttr::type assetType = rttr::type::get_by_name(m_assetTypesList[aTypeID]);
			rttr::variant assetSharedPtr = assetType.create();
			AssertCore(assetSharedPtr.is_valid(), "Cannot default create instance of asset");
			rttr::method assetConstructMethod = assetSharedPtr.extract_wrapped_value().get_type().get_method(Serialisation::assetConstructMethodName);
			rttr::variant initializerStructShared = rttr::type::get_by_name(m_assetID_to_helper[aTypeID]).create();
			rttr::variant initializerStructPtr = initializerStructShared.extract_wrapped_value();
			rttr::property initializerStructFileName = initializerStructPtr.get_type().get_property(Serialisation::filePathName.c_str());
			AssertCore(initializerStructFileName.is_valid(), "Cannot get file path property in intializer class %s", initializerStructFileName.get_type().get_name());
			AssertCore(assetConstructMethod.is_valid(), "Cannot get construct method from type %s", assetSharedPtr.get_type().get_name());
			AssertCore(assetConstructMethod.get_parameter_infos().begin()->get_type() == initializerStructPtr.get_type(), "Construct method incorrectly implemented");
			initializerStructFileName.set_value(initializerStructPtr, fp);
			assetConstructMethod.invoke(assetSharedPtr.extract_wrapped_value(), initializerStructPtr);
			//Attempt to create the asset data
			//m_assetData.at(aTypeID)->InsertData(_assetID,Ass);
			//m_assetData.at(aTypeID)->GetDataNT(_assetID);
			m_assetData.at(aTypeID)->InsertDataByBaseClass(_assetID, std::move(*assetSharedPtr.extract_wrapped_value().get_value<IAsset*>()));
			//Update the mapping
			m_assetID_to_fileName.emplace(_assetID, fp);
			m_fileName_to_assetID.emplace(fp, _assetID);
			m_assetID_to_type.emplace(_assetID, std::make_pair(aTypeID, _assetFlags));

			return _assetID;
		}
		//If asset loading fail, throw an exception in AssetType's constructor
		catch (const std::exception& e)
		{
			LogCoreError("Failed to create the asset \"%s\": %s.", _filePath.c_str(), e.what());
			return NULL_ASSET;
		}
	}

	AssetID AssetManager::LoadAsset(const std::string& _filePath, size_t _assetFlags)
	{
		//Determine the extension of the file
		size_t where = _filePath.find_last_of('.') + 1;
		std::string extension = _filePath.substr(where, _filePath.size() - where);

		//For each file type
		for (const auto& [aTypeID, extensionList] : m_fileExtensions)
		{
			//If found the file extension
			if (std::find(extensionList.begin(), extensionList.end(), extension) != extensionList.end())
			{
				LogCore("Detected \"%s\" is a \"%s\".", _filePath.c_str(), m_assetTypesList.at(aTypeID).c_str());

				//Overload 3 - generate a random ID, or returns 0 if loading failed
				BEID result = LoadAsset(m_assetTypesList.at(aTypeID), AssetID{}, _filePath, _assetFlags);
				return result;
			}
		}

		LogCoreError("Unknown file extension \"%s\" detected. Go to AssetManager's constructor to register this file extension if needed.", extension.c_str());
		return AssetID{ 0 };
	}

	void AssetManager::UnloadAsset(AssetID _assetID, bool _forceRemove)
	{
		//If asset doesn't exist, then don't do anything
		if (!IsAssetDataLoaded(_assetID))
		{
			LogCoreWarning("AssetID %llu was not loaded, cannot unload.", _assetID);
			return;
		}

		bool inUse = false;

		/*
			Expand other type of asset check
		*/

		//If this is an audio clip
		if (GetAssetTypeIDOf(_assetID) == GetAssetTypeID<AudioClip>())
		{
			//Check through all entities to see if any entities are still using the audio source
			const auto& entities = ECS->GetAllEntities();
			for (const EntityID& e : entities)
			{
				//If entity has audio source component
				if (ECS->HasComponent<AudioSource>(e) && ECS->GetComponent<AudioSource>(e).HasAudioLinked())
				{
					//If going to force remove the asset, then unlink it
					if (_forceRemove)
						ECS->GetComponent<AudioSource>(e).RemoveAllAudio();
					//If not going to force remove the asset, then stop searching because this asset is in use.
					else
					{
						inUse = true;
						break;
					}
				}
			}
		}
		//else if other types of asset

		//If the asset is flagged as alwaysLoaded, then don't remove it
		if (HasFlag(_assetID, AssetFlag::alwaysLoaded))
			return;

		//Get the type of asset is it from
		AssetTypeID aType = m_assetID_to_type.at(_assetID).first;

		//Remove assetID's data from the container
		m_assetData.at(aType)->RemoveData(_assetID);

		//Remove assetID from the mappings
		const std::string& fileName = m_assetID_to_fileName.at(_assetID);
		m_fileName_to_assetID.erase(fileName);
		m_assetID_to_fileName.erase(_assetID);
		m_assetID_to_type.erase(_assetID);
	}

	void AssetManager::UnloadAllAssets(bool _forceRemove)
	{
		regeneratingAssets = true;

		auto begin = m_assetID_to_fileName.begin();
		auto end = m_assetID_to_fileName.end();

		while (begin != end)
		{
			UnloadAsset((begin++)->first, _forceRemove);
		}
	}

	void AssetManager::UnloadUnusedAssets()
	{
		//Unload all assets not being used by any entity or UI
		this->UnloadAllAssets(false);
	}

	bool AssetManager::IsAssetDataLoaded(AssetID _assetID) const
	{
		return m_assetID_to_fileName.count(_assetID) != 0;
	}

	bool AssetManager::IsAssetDataLoaded(const std::string& _fileName) const
	{
		std::string fp = _fileName;
		ConvertFilePathSlash(fp);
		return m_fileName_to_assetID.count(fp);
	}

	bool AssetManager::HasFlag(AssetID _assetID, AssetFlag _f) const
	{
		AssertCore(m_assetID_to_type.count(_assetID) != 0, "Asset %llu was not loaded", *_assetID);
		return m_assetID_to_type.at(_assetID).second.test((size_t)_f);
	}

	void AssetManager::SetFlag(AssetID _assetID, AssetFlag _f, bool _b)
	{
		AssertCore(m_assetID_to_type.count(_assetID) != 0, "Asset %llu was not loaded", *_assetID);
		m_assetID_to_type.at(_assetID).second.set((size_t)_f, _b);
	}

	AssetID AssetManager::GetAssetID(const std::string& _fileName) const
	{
		//Convert to / slash
		std::string fp = _fileName;
		ConvertFilePathSlash(fp);

		//If file name is not found, then return 0
		if (m_fileName_to_assetID.count(fp) == 0)
		{
			LogCoreError("No asset with file name \"%s\" is loaded, returning NULL_ASSET instead", _fileName.c_str());
			return NULL_ASSET;
		}
		else
			return m_fileName_to_assetID.at(fp);
	}

	const std::string& AssetManager::GetAssetFileName(AssetID _assetID) const
	{
		//If file name is not found, then return 0
		if (m_assetID_to_fileName.count(_assetID) == 0)
		{
			//LogCoreError("No asset with ID %llu is loaded, returning empty string instead", *_assetID);
			static std::string errString{};
			return errString;
		}
		else
			return m_assetID_to_fileName.at(_assetID);
	}

	std::string AssetManager::GetAssetTruncatedFileName(AssetID _assetID) const
	{
		std::string result = GetAssetFileName(_assetID);
		TruncateFilePath(result);
		return result;
	}

	const std::string& AssetManager::GetAssetTypeOf(AssetID _assetID) const
	{
		return m_assetTypesList.at(this->GetAssetTypeIDOf(_assetID));
	}

	typename AssetManager::AssetTypeID AssetManager::GetAssetTypeIDOf(AssetID _assetID) const
	{
		AssertCore(m_assetID_to_type.count(_assetID) != 0, "AssetID %llu does not exist", *_assetID);
		return m_assetID_to_type.at(_assetID).first;
	}

	void AssetManager::Serialise(const std::string& output)
	{
		StopWatch watch("AssetManager::Serialise");
		using namespace rapidjson;
		using namespace rttr;
		using namespace BE::Serialisation;
		//Create a document to store the json
		std::ofstream ofs(output, std::ofstream::out | std::ofstream::trunc);
		Document d;
		//Set the root of the json and create temporary variables
		Value& rootValue = d.SetObject();
		Value assetArray, entityObj, entityMember, entityMemberKey;
#ifdef DONT_USE_ARRAY_FOR_LEVELFILES
		assetArray.SetObject();
#else
		assetArray.SetArray();
#endif
		variant mapVariant(m_assetID_to_fileName);
		auto view = mapVariant.create_associative_view();
		std::stringstream ss;
		for (const auto& curr : view)
		{
			rapidjson::Value arrMember, memberKey, memberValue, intializerValue;
			arrMember.SetObject();
			rttr::variant keyUnwrapped = curr.first.get_type().is_wrapper() ? curr.first.extract_wrapped_value() : curr.first;
			AssertCore(keyUnwrapped.get_type() == type::get<BEID>(), "Key value for asset is not a id");
			memberKey.SetUint64(*keyUnwrapped.get_value<BEID>());
			arrMember.AddMember("Key", memberKey, d.GetAllocator());
			memberValue.SetObject();
			Value path;
			rttr::variant valueUnwrapped = curr.second.get_type().is_wrapper() ? curr.second.extract_wrapped_value() : curr.second;
			AssertCore(valueUnwrapped.get_type() == type::get<std::string>(), "Value for asset map is not a string");
			path.SetUint64(m_assetID_to_type[keyUnwrapped.get_value<BEID>()].second.to_ullong());
			memberValue.AddMember("Flag", path, d.GetAllocator());
			type intitializerType = type::get_by_name(m_assetID_to_helper[GetAssetTypeIDOf(keyUnwrapped.get_value<BEID>())]);
			const rttr::type& assetType = type::get(m_assetData[m_assetID_to_type[keyUnwrapped.get_value<BEID>()].first]->GetDataNT(keyUnwrapped.get_value<BEID>()));
			rttr::instance assetInstance(m_assetData[m_assetID_to_type[keyUnwrapped.get_value<BEID>()].first]->GetDataNT(keyUnwrapped.get_value<BEID>()));
			AssertCore(assetInstance.get_derived_type() == assetType, "Failed to convert asset reference to type, is currently of %s type", assetInstance.get_type().get_name());
			method serialiseMethod = assetType.get_method(serializeAssetMethodName);
			AssertCore(serialiseMethod.is_valid(), "No valid serialise method was found");
			variant inst = serialiseMethod.invoke(assetInstance);
			AssertCore(inst.get_type() == intitializerType, "Serialisation method did not return correct struct according to registration");
			intitializerType.set_property_value(filePathName, inst, valueUnwrapped.get_value<std::string>());
			intializerValue.SetObject();
			SerializeToValueBase(inst, intializerValue, d);
			rapidjson::Value intializerTypeName;
			intializerTypeName.SetString(intitializerType.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(intitializerType.get_name().length()), d.GetAllocator());
			memberValue.AddMember(intializerTypeName, intializerValue, d.GetAllocator());
			arrMember.AddMember("Value", memberValue, d.GetAllocator());
#ifdef DONT_USE_ARRAY_FOR_LEVELFILES
			ss.str(std::string());
			ss << *keyUnwrapped.get_value<BEID>();
			rapidjson::Value assetIdString;
			assetIdString.SetString(ss.str().c_str(), static_cast<rapidjson::SizeType>(ss.str().length()), d.GetAllocator());
			assetArray.AddMember(assetIdString, arrMember, d.GetAllocator());
#else
			assetArray.PushBack(arrMember, d.GetAllocator());
#endif
		}
		rootValue.AddMember("AssetList", assetArray, d.GetAllocator());
		OStreamWrapper osw(ofs);
		//Create a writer to write the document to the output stream
		PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		d.Accept(writer);
		ofs.close();

	}

	void AssetManager::Deserialise(const std::string& fileName)
	{
		StopWatch watch("AssetManager::Deserialise");
		using namespace rapidjson;
		using namespace rttr;
		rapidjson::Document doc;
		//Try to open the file and read into a string steam
		std::ifstream ifs(fileName);
		if (ifs.fail())
			LogCoreError("Could not open file : %s", fileName.c_str());
		std::stringstream ss(std::string(std::istreambuf_iterator<char>(ifs), {}));
		if (ss.str().length() == 0) // New file detected
			return;
		//Parse the inputted string indo the rapidjson document
		doc.Parse(ss.str().c_str());
		size_t counter = 0;
		//Validate the json
		rapidjson::Value& v = doc["AssetList"];
		if (v.IsArray())
		{
			//Call load entity for each entity found inside the json
			for (rapidjson::Value::ConstValueIterator curr = v.GetArray().Begin(); curr != v.GetArray().End(); ++curr)
			{
				counter++;
				if (!curr->IsObject())
					LogCoreError("Error loading entity in %s", fileName.c_str());
				AssertCore(curr->HasMember("Key"), "No member key was found");
				BEID assetId((*curr)["Key"].GetUint64());
				AssertCore(curr->HasMember("Value"), "No member value was found");
				const std::string* initializerTypeNamePtr = nullptr;
				const AssetTypeID* assetTypeIDPtr = nullptr;
				for (const auto& pair : m_assetID_to_helper)
				{
					if (curr[0]["Value"].HasMember(pair.second.c_str()))
					{
						initializerTypeNamePtr = &pair.second;
						assetTypeIDPtr = &pair.first;
						break;
					}
				}
				AssertCore(initializerTypeNamePtr != nullptr, "Initializer type read not found");
				AssertCore(assetTypeIDPtr != nullptr, "assetTypeIDPtr not found");
				const rttr::type& initializerType = type::get_by_name(*initializerTypeNamePtr);
				AssertCore(initializerType.is_valid(), "Invalid initializer type read");
				rttr::variant initializerVariant = initializerType.create();
				rttr::instance initializerInst = initializerVariant;
				Serialisation::LoadObjectRecur(curr[0]["Value"][initializerTypeNamePtr[0].c_str()], initializerType, BEID(0), initializerInst);
				property filePathProp = initializerType.get_property(Serialisation::filePathName);
				AssertCore(filePathProp.is_valid() && filePathProp.get_type() == type::get<std::string>(), "Cannot find path within initializer");
				std::string filePath = filePathProp.get_value(initializerInst).get_value<std::string>();
				const AssetTypeID assetTypeID = *assetTypeIDPtr;
				AssertCore(curr[0]["Value"].HasMember("Flag"), "Flag for this asset not found");
				AssetFlags assetFlag = curr[0]["Value"]["Flag"].GetUint64();
				rttr::variant assetSharedPtr = type::get_by_name(m_assetTypesList[*assetTypeIDPtr]).create();
				AssertCore(assetSharedPtr.is_valid(), "Cannot default create instance of asset");
				method assetConstructMethod = assetSharedPtr.extract_wrapped_value().get_type().get_method(Serialisation::assetConstructMethodName);
				variant initializerStructPtr = initializerVariant.extract_wrapped_value();
				AssertCore(assetConstructMethod.is_valid(), "Cannot get construct method from type %s", assetSharedPtr.get_type().get_name());
				AssertCore(assetConstructMethod.get_parameter_infos().begin()->get_type() == initializerStructPtr.get_type(), "Construct method incorrectly implemented");
				assetConstructMethod.invoke(assetSharedPtr.extract_wrapped_value(), initializerStructPtr);
				if (IsAssetDataLoaded(assetId))
				{
					LogCoreError("Duplicate Asset ID %llu found, asset not created", *assetId);
					continue;
				}
				if (IsAssetDataLoaded(filePath))
				{
					LogCoreError("Already loaded the asset %s", filePath);
					continue;
				}

				try
				{
					//Update the mapping
					m_assetID_to_fileName.emplace(assetId, filePath);
					m_fileName_to_assetID.emplace(filePath, assetId);
					m_assetID_to_type.emplace(assetId, std::make_pair(assetTypeID, assetFlag));

					//Move data into the m_data container
					//GetAssetDataContainer<AssetType>()->InsertData(_assetID, std::move(data));
					m_assetData.at(assetTypeID)->InsertDataByBaseClass(assetId, std::move(*assetSharedPtr.extract_wrapped_value().get_value<IAsset*>()));

				}
				//If asset loading fail, throw an exception in AssetType's constructor
				catch (const std::exception& e)
				{
					LogCoreError("Failed to create the asset \"%s\": %s.", filePath, e.what());
				}
				//LoadEntity(*curr);
			}
		}
		else if (v.IsObject())
		{
			//Call load entity for each entity found inside the json
			for (auto itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr)
			{
				rapidjson::Value* curr = &itr->value;
				counter++;
				if (!curr->IsObject())
					LogCoreError("Error loading entity in %s", fileName.c_str());
				AssertCore(curr->HasMember("Key"), "No member key was found");
				BEID assetId((*curr)["Key"].GetUint64());
				AssertCore(curr->HasMember("Value"), "No member value was found");
				const std::string* initializerTypeNamePtr = nullptr;
				const AssetTypeID* assetTypeIDPtr = nullptr;
				for (const auto& pair : m_assetID_to_helper)
				{
					if (curr[0]["Value"].HasMember(pair.second.c_str()))
					{
						initializerTypeNamePtr = &pair.second;
						assetTypeIDPtr = &pair.first;
						break;
					}
				}
				AssertCore(initializerTypeNamePtr != nullptr, "Initializer type read not found");
				AssertCore(assetTypeIDPtr != nullptr, "assetTypeIDPtr not found");
				const rttr::type& initializerType = type::get_by_name(*initializerTypeNamePtr);
				AssertCore(initializerType.is_valid(), "Invalid initializer type read");
				rttr::variant initializerVariant = initializerType.create();
				rttr::instance initializerInst = initializerVariant;
				Serialisation::LoadObjectRecur(curr[0]["Value"][initializerTypeNamePtr[0].c_str()], initializerType, BEID(0), initializerInst);
				property filePathProp = initializerType.get_property(Serialisation::filePathName);
				AssertCore(filePathProp.is_valid() && filePathProp.get_type() == type::get<std::string>(), "Cannot find path within initializer");
				std::string filePath = filePathProp.get_value(initializerInst).get_value<std::string>();
				const AssetTypeID assetTypeID = *assetTypeIDPtr;
				AssertCore(curr[0]["Value"].HasMember("Flag"), "Flag for this asset not found");
				AssetFlags assetFlag = curr[0]["Value"]["Flag"].GetUint64();
				rttr::variant assetSharedPtr = type::get_by_name(m_assetTypesList[*assetTypeIDPtr]).create();
				AssertCore(assetSharedPtr.is_valid(), "Cannot default create instance of asset");
				method assetConstructMethod = assetSharedPtr.extract_wrapped_value().get_type().get_method(Serialisation::assetConstructMethodName);
				variant initializerStructPtr = initializerVariant.extract_wrapped_value();
				AssertCore(assetConstructMethod.is_valid(), "Cannot get construct method from type %s", assetSharedPtr.get_type().get_name());
				AssertCore(assetConstructMethod.get_parameter_infos().begin()->get_type() == initializerStructPtr.get_type(), "Construct method incorrectly implemented");
				assetConstructMethod.invoke(assetSharedPtr.extract_wrapped_value(), initializerStructPtr);
				if (IsAssetDataLoaded(assetId))
				{
					LogCoreError("Duplicate Asset ID %llu found, asset not created", *assetId);
					continue;
				}
				if (IsAssetDataLoaded(filePath))
				{
					LogCoreError("Already loaded the asset %s", filePath);
					continue;
				}

				try
				{
					//Update the mapping
					m_assetID_to_fileName.emplace(assetId, filePath);
					m_fileName_to_assetID.emplace(filePath, assetId);
					m_assetID_to_type.emplace(assetId, std::make_pair(assetTypeID, assetFlag));

					//Move data into the m_data container
					//GetAssetDataContainer<AssetType>()->InsertData(_assetID, std::move(data));
					m_assetData.at(assetTypeID)->InsertDataByBaseClass(assetId, std::move(*assetSharedPtr.extract_wrapped_value().get_value<IAsset*>()));

				}
				//If asset loading fail, throw an exception in AssetType's constructor
				catch (const std::exception& e)
				{
					LogCoreError("Failed to create the asset \"%s\": %s.", filePath, e.what());
				}
				//LoadEntity(*curr);
			}
		}
		else
		{
			AssertCore(false, "Cannot load this asset entity file");
		}

		regeneratingAssets = false;
		LogCore("%d assets were loaded", counter);
	}

	void AssetManager::OnSceneUnload(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::unloadScene, "Wrong event, expected UnloadSceneEvent, passed in \"%s\" instead.", e.GetEventName());

		//Unload assets from previous scene
		UnloadAllAssets(true);
	}

	Image AssetManager::loadImage(const std::string& filePath, ImageChannelsFormat format)
	{
		Image newImage{};
		newImage.imageData = SOIL_load_image(filePath.c_str(), &newImage.width, &newImage.height, &newImage.channels, static_cast<int>(format));
		newImage.filePath = filePath;

		if (newImage.imageData == nullptr)
		{
			AssertCore("Failed to load image from %s", filePath.c_str());
		}
		return newImage;
	}

	void AssetManager::SaveImageAsDDS(Image& image)
	{
		//this snippet of code is commneted out as we are directly saving the dds in the same directory

		//std::string newfilePath{ image.filePath.substr(0, image.filePath.find_first_of(".")) };
		//newfilePath += ".dds";

		//if (std::filesystem::is_directory(outputFolder) == false)
		//{
		//	std::filesystem::create_directory(outputFolder);
		//}

		////Find the folder and sub folders to place it in
		//newfilePath.replace(newfilePath.find(conversionFolder.string()), conversionFolder.string().length(), outputFolder.string());

		////Make sure all subfolders exist
		//std::string tempPath = newfilePath;
		//tempPath = tempPath.substr(0, tempPath.find_last_of("\\"));

		//if (std::filesystem::is_directory(tempPath) == false)
		//{
		//	std::filesystem::create_directories(tempPath);
		//}

		std::string newfilePath{ image.filePath };
		newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_last_of("."), ".dds");
		int result = save_image_as_DDS(newfilePath.c_str(), image.width, image.height, image.channels, image.imageData);
		if (result == 0)
		{
			AssertCore("Failed to save image %s", image.filePath.c_str());
		}
	}

	void AssetManager::DestroyImage(Image& image)
	{
		SOIL_free_image_data(image.imageData);
		image.imageData = nullptr;
	}

	void AssetManager::scanDirectory(const std::filesystem::path& filePath)
	{
		for (auto const& dir_entry : std::filesystem::directory_iterator{ filePath })
		{
			//Find the last path etc
			std::string lastDirectory = dir_entry.path().string();
			lastDirectory = lastDirectory.substr(lastDirectory.find_last_of("\\") + 1);

			//If the path has no file extension, it is a folder
			if (lastDirectory.find(".") == std::string::npos)
			{
				scanDirectory(dir_entry.path());
			}
			else //it is a image or model
			{
				//Find the type of extension
				std::string extensionType = lastDirectory.substr(lastDirectory.find_first_of(".") + 1);
				std::transform(extensionType.begin(), extensionType.end(), extensionType.begin(),
					[](char c) { return static_cast<char>(std::tolower(c)); });

				if (std::strcmp(extensionType.c_str(), "fbx") == 0 || std::strcmp(extensionType.c_str(), "bananaModel") == 0)
				{
					filePaths.emplace_back(dir_entry.path().string());
				}
				else if (std::strcmp(extensionType.c_str(), "dds") == 0)
				{
					filePaths.insert(filePaths.begin(), dir_entry.path().string());
				}
			}
		}
	}

	void AssetManager::scanDirectoryForConversion(const std::filesystem::path& filePath, RegenerationAssetType type, bool firstLoad)
	{
		for (auto const& dir_entry : std::filesystem::directory_iterator{ filePath })
		{
			//Find the last path etc
			std::string lastDirectory = dir_entry.path().string();
			lastDirectory = lastDirectory.substr(lastDirectory.find_last_of("\\") + 1);

			//If the path has no file extension, it is a folder
			if (lastDirectory.find(".") == std::string::npos)
			{
				scanDirectoryForConversion(dir_entry.path(), type, firstLoad);
			}
			else //it is a image or model
			{
				//Find the type of extension
				std::string extensionType = lastDirectory.substr(lastDirectory.find_last_of(".") + 1);
				std::transform(extensionType.begin(), extensionType.end(), extensionType.begin(),
					[](char c) { return static_cast<char>(std::tolower(c)); });


				if (std::strcmp(extensionType.c_str(), "fbx") == 0 && type != RegenerationAssetType::textures)
				{
					std::string convertedFile = dir_entry.path().string().replace(dir_entry.path().string().find(".fbx"), dir_entry.path().string().find(".fbx") + 3, ".bananaModel");

					if (std::filesystem::exists(convertedFile) == false || !firstLoad)
						convertModel(dir_entry.path().string());
				}
				else if (std::strcmp(extensionType.c_str(), "png") == 0 && type != RegenerationAssetType::models)
				{
					std::string convertedFile = dir_entry.path().string().replace(dir_entry.path().string().find(".png"), dir_entry.path().string().find(".png") + 3, ".dds");

					if (std::filesystem::exists(convertedFile) == false || !firstLoad)
						convertImage(dir_entry.path().string(), ImageChannelsFormat::normal);
				}
			}
		}
	}

	void AssetManager::regenerateThisAsset(const std::string& filePath)
	{
		std::string convertedFile = filePath;
		std::string extensionType = filePath.substr(filePath.find_last_of(".") + 1);
		std::transform(extensionType.begin(), extensionType.end(), extensionType.begin(),
			[](char c) { return static_cast<char>(std::tolower(c)); });

		if (std::strcmp(extensionType.c_str(), "fbx") == 0)
		{
			convertModel(filePath);
		}
		else if (std::strcmp(extensionType.c_str(), "bananamodel") == 0)
		{
			convertedFile = convertedFile.replace(convertedFile.find(".bananaModel"), convertedFile.find(".bananaModel") + 11, ".fbx");
			convertModel(convertedFile);
		}
		else if (std::strcmp(extensionType.c_str(), "png") == 0)
		{
			convertImage(filePath, ImageChannelsFormat::normal);
		}
		else if (std::strcmp(extensionType.c_str(), "dds") == 0)
		{
			convertedFile = convertedFile.replace(convertedFile.find(".dds"), convertedFile.find(".dds") + 3, ".png");
			convertImage(convertedFile, ImageChannelsFormat::normal);
		}
	}

	void AssetManager::LoadAllScannedAssets()
	{
		//find all textures
		std::vector<std::string> allTextures;
		std::vector<std::string> allModels;

		for (std::string& s : filePaths)
		{
			if (s.find(".dds") != std::string::npos)
			{
				allTextures.emplace_back(s);
			}
			else if (s.find(".fbx") != std::string::npos || s.find(".bananaModel") != std::string::npos)
			{
				allModels.emplace_back(s);
			}
		}

		//Split all textures into own set
		std::map<std::string, std::vector<std::string>> folderToTextureMap; //folder name, filepath

		for (int i{}; i < allTextures.size(); ++i)
		{
			std::string s = allTextures[i].substr(allTextures[i].find("Textures\\"), allTextures[i].find_last_of("\\") - allTextures[i].find("Textures\\"));

			//remove double forward slash
			while (allTextures[i].find("\\") != std::string::npos)
			{
				allTextures[i].replace(allTextures[i].find("\\"), 1, "/");
			}

			//if folder exists
			if (folderToTextureMap.find(s) != folderToTextureMap.end())
			{
				folderToTextureMap.find(s)->second.emplace_back(allTextures[i]);
			}
			else //if folder doesn't exist
			{
				folderToTextureMap[s] = std::vector<std::string>{};
				folderToTextureMap[s].emplace_back(allTextures[i]);
			}
		}

		//Load all textures
		//LHS string is Textures\\folder\\folder etc, RHS strings are converted to normal path
		for (std::map<std::string, std::vector<std::string>>::iterator i{ folderToTextureMap.begin() }; i != folderToTextureMap.end(); i++)
		{
			for (std::string const& s : i->second)
			{
				AssetMgr->LoadAsset<Texture>(s, 1);
			}
		}

		//Load all models
		for (std::vector<std::string>::iterator i{ allModels.begin() }; i != allModels.end(); i++)
		{
			std::string s = *i;

			//first go through all filepaths and replace the double forward slash
			while (s.find("\\") != std::string::npos)
			{
				s.replace(s.find("\\"), 1, "/");
			}

			AssetMgr->LoadAsset<BeModel>(s, 1);
		}

		allTextures.clear();
		allModels.clear();
		filePaths.clear();
	}

	void AssetManager::convertImage(const std::string& filePath, ImageChannelsFormat format)
	{
		Image newImage = loadImage(filePath, format);
		SaveImageAsDDS(newImage);
		DestroyImage(newImage);
		//std::remove(filePath.c_str());
	}

	void AssetManager::convertModel(const std::string& filePath)
	{
		BeModel newModel(filePath, true);
		std::string newfilePath{ filePath };
		newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_first_of("."), ".bananaModel");

		std::ofstream bananaModel;
		bananaModel.open(newfilePath, std::ofstream::out | std::ofstream::binary);

		//C++ CANT PROPERLY DUMP VECTORS AHHHHH @donavern
		//and string btw, do it manually
		//DUMP IT
		newModel.write(bananaModel);
		bananaModel.close();
		//std::remove(filePath.c_str());
	}

	BeDevice* AssetManager::getDevice()
	{
		return deviceToUse;
	}

	void AssetManager::setDevice(BeDevice* d)
	{
		deviceToUse = d;
	}
	void AssetManager::setRegeneratingAssets(bool b)
	{
		regeneratingAssets = b;
	}

	bool AssetManager::getRegeneratingAssets()
	{
		return regeneratingAssets;
	}

	void AssetManager::setWaitingToRegenerateAssets(bool b)
	{
		waitingToRegenerateAssets = b;
	}

	bool AssetManager::getWaitingToRegenerateAssets()
	{
		return waitingToRegenerateAssets;
	}

	void AssetManager::setClearToRegenerateAssets(bool b)
	{
		clearToRegenerateAssets = b;
	}

	bool AssetManager::getClearToRegenerateAssets()
	{
		return clearToRegenerateAssets;
	}
	void AssetManager::initializeFontStuff(uint32_t width, uint32_t height, VkDescriptorSetLayout layout, std::unique_ptr<BeDescriptorPool>& pool)
	{
		textureAtlas = std::make_shared<TextureAtlas>();
		textureAtlas->Initialize(deviceToUse, 2048, 2048, pool, layout);

		auto error = FT_Init_FreeType(&fontLibrary);
		if (error)
		{
			throw std::runtime_error("Failed to intiailize font freetype library");
		}
	}
	void AssetManager::shutdownFontStuff()
	{
		textureAtlas->Shutdown();
	}
	FT_Library& AssetManager::getFontLibrary()
	{
		return fontLibrary;
	}
	std::shared_ptr<TextureAtlas> AssetManager::getTextureAtlas()
	{
		return textureAtlas;
	}
}
#pragma warning (pop)