#pragma once
#ifndef	ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <Precompiled/pch.h>
#include <Utilities/Singleton/Singleton.h>
#include <Utilities/BEID/BEID.h>

#include "AssetData.h"

//Different file types here
#include "AssetTypes/AudioClip.h"
#include "AssetTypes/Be_model.hpp"
#include "AssetTypes/Animation.h"
#include "AssetTypes/Texture.h"
#include "AssetTypes/PrefabAsset.h"
#include "../Systems/Graphics/Font/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#define loadFontSize 96

namespace BE
{
	enum class AssetFlag
	{
		alwaysLoaded = 0,	//Asset will always be loaded, regardless of forced removed or not. Only removed when engine is shutdown

		last
	};

	enum class ImageChannelsFormat
	{
		normal,
		greyscale,
		greyscaleAlpha,
		RGB,
		RGBA
	};

	enum class RegenerationAssetType
	{
		textures,
		models,
		both
	};

	struct Image
	{
		std::string filePath{};
		int width{}, height{}, channels{};
		unsigned char* imageData{};
		int size{};
	};

	struct bananaModelStruct
	{
		std::vector<BoneInfo> m_BoneInfoMap;
		std::vector<Mesh> allMeshes;
		std::vector<Animation> animationDetails;
	};

	const std::filesystem::path outputFolder{ "../Assets" };
	const std::filesystem::path conversionFolder{ "../Assets" };

	//Asset manager
	class AssetManager : public Singleton<AssetManager>
	{
		/*
			Asset types tracking - Manages the types of IDs there are
		*/
		//An ID representing the asset file type
		using AssetTypeID = size_t;

		//Asset flags attached to the asset ID
		using AssetFlags = std::bitset<(size_t)AssetFlag::last>;

		//For registering asset type
		mutable AssetTypeID m_nextID = 0;

		//The types of assets in use
		std::unordered_map<AssetTypeID, std::string> m_assetTypesList{};
		std::unordered_map<std::string, AssetTypeID> m_assetTypesList_reverse{};

		//Asset Type ID associated to the Given helper type
		std::unordered_map<AssetTypeID, std::string> m_assetID_to_helper{};

		//Maps asset type to a set of file extensions (eg: jpg, png, fbx, wav)
		std::unordered_map<AssetTypeID, std::vector<std::string>> m_fileExtensions{};

		/*
			Asset ID tracking - Manages the Asset IDs that are loaded into the engine
		*/

		//Asset's file name associated to Asset ID
		std::unordered_map<AssetID, std::string> m_assetID_to_fileName{};

		//Asset ID associated to Asset's file name
		std::unordered_map<std::string, AssetID> m_fileName_to_assetID{};

		//Asset Type ID associated to the Asset ID and AssetFlag
		std::unordered_map<AssetID, std::pair<AssetTypeID, AssetFlags>> m_assetID_to_type{};

		/*
			Asset data management - Manages the actual asset data
		*/
		std::unordered_map<AssetTypeID, IAssetDataContainer*> m_assetData;

		//Constructor and Destructor
		AssetManager();
		~AssetManager();



	public:
		/*
			Loading/Unloading assets
		*/
		//Loads asset into the asset manager with a known asset ID.
		template <typename AssetType, typename... CtorArgs>
		AssetID LoadAsset(AssetID _assetID, const std::string& _filePath, size_t _assetFlags, CtorArgs&&... _args);

		//Loads asset into the asset manager with a randomly generated asset ID.
		template <typename AssetType, typename... CtorArgs>
		AssetID LoadAsset(const std::string& _filePath, size_t _assetFlags, CtorArgs&&... _args);

		//Load asset into asset manager - non templated, known ID - for deserialisation purpose
		AssetID LoadAsset(const std::string& _assetType, AssetID _assetID, const std::string& _filePath, size_t _assetFlags = 0);

		//Load asset without knowing the type, randomly generate an ID - for editor use
		AssetID LoadAsset(const std::string& _filePath, size_t _assetFlags = 0);

		/*
			NOTE: (T here refers to Asset Type)
			- T is an asset type class that wraps the asset data used by the ECS components or UI.
			- T should inherit IAsset and override the Construct function
			- T must have a corresponding Asset File initialisation struct in the same header
			- T must have these constructors
				> Default constructor
				> Non-default constructor that takes in asset's filepath as the first param
				  and any additional arguments for subsequent parameters
				> Constructor that takes in void*, which will be passed in an initialisation struct
			- Do the construction in the Construct function
			- The non-default constructor shall call Construct function to do the construction
			-
			- Do any specialised things (like loading sound data into fmod system) inside T's constructor.
			- If asset loading fails (eg: file not found), throw an std::exception in the constructor, Asset manager will catch it.
			- If the provided AssetID already exists, a new AssetID will be issued.
			- If an asset file has already been loaded, no new loading happens.
			- T should ideally be move constructible/assignable, there will be move construction/assignment used.
			- Make sure to put the correct T value for the type of asset you are loading
		*/

		//Unloads asset from asset manager
		void UnloadAsset(AssetID _assetID, bool _forceRemove = false);

		//Unload all assets
		void UnloadAllAssets(bool _forceRemove = false);

		//Unload assets that are not used by any entity or UI
		void UnloadUnusedAssets();

		/*
			NOTE:
			- Do any specialised things (like releasing fmod sound data) inside T's destructor.
			- _forceRemove means to remove the asset even if any ECS entity/UI are using it
			- Make sure to put the correct T value for the asset type you are unloading
		*/



		/*
			Assets data querying
		*/
		//Get the data of the asset 
		template <typename AssetType>
		AssetType& GetAssetData(AssetID _assetID);

		template <typename AssetType>
		AssetType& GetAssetData(const std::string& _fileName);

		//Check whether the asset is loaded for its type
		template <typename AssetType>
		bool IsAssetDataLoadedForType(AssetID _assetID) const;

		template <typename AssetType>
		bool IsAssetDataLoadedForType(const std::string& _fileName) const;

		//Check whether the asset is loaded for any type - by asset ID
		bool IsAssetDataLoaded(AssetID _assetID) const;

		//Check whether the asset is loaded for any type - by file name
		bool IsAssetDataLoaded(const std::string& _fileName) const;

		//Get and set asset flags
		bool HasFlag(AssetID _assetID, AssetFlag _f) const;
		void SetFlag(AssetID _assetID, AssetFlag _f, bool _b);

		//Get all asset IDs of type
		template <typename AssetType>
		inline const auto& GetAllAssetIDsOfType() const { return GetAssetDataContainer<AssetType>()->GetAllAssetIDs(); }

		//Get all asset IDs of type
		template <typename AssetType>
		inline const auto& GetAllAssetDataOfType() const { return GetAssetDataContainer<AssetType>()->GetAllAssets(); }



		/*
			Assets info querying
		*/
		//Get asset ID using it's file name - compares the full file path
		AssetID GetAssetID(const std::string& _fileName) const;

		//Get asset (full) file path using it's asset ID.
		const std::string& GetAssetFileName(AssetID _assetID) const;

		//Get asset (truncated) file path using it's asset ID
		std::string GetAssetTruncatedFileName(AssetID _assetID) const;

		//Get the type of asset that asset ID is (get as string)
		const std::string& GetAssetTypeOf(AssetID _assetID) const;

		//Get the type of asset that asset ID is (get as ID)
		AssetTypeID GetAssetTypeIDOf(AssetID _assetID) const;



		/*
			Asset manager functions
		*/
		//Register a new type of asset
		template <typename AssetType, typename AssetHelper>
		AssetTypeID RegisterAssetType(std::vector<std::string>&& _fileExtensions);

		//Get the ID of the asset type
		template <typename AssetType>
		AssetTypeID GetAssetTypeID() const;

		//Get asset names
		inline const auto& GetFileTypes() const { return m_assetTypesList; }

		//Get all loaded asset IDs
		inline const auto& GetAllLoadedAssets() const { return m_assetID_to_fileName; }



		/*
			Additional specialised functions - like reloading scripts, convert textures to DDS etc
		*/
		//for loading, no conversion
		void scanDirectory(const std::filesystem::path& filePath);
		void scanDirectoryForConversion(const std::filesystem::path& filePath, RegenerationAssetType type, bool firstLoad);
		void regenerateThisAsset(const std::string& filePath);
		void LoadAllScannedAssets();

		//For conversion
		//SOIL_LOAD_AUTO,L,LA,RGB,RGBA
		//Best to just use SOIL_LOAD_AUTO
		Image loadImage(const std::string& filePath, ImageChannelsFormat format);
		void DestroyImage(Image& image);

		//TGA,BMP,DDS,JPG
		void SaveImageAsDDS(Image& image); //DXT1 if no alpha, DXT5 if alpha channel exists
		void convertImage(const std::string& filePath, ImageChannelsFormat format);
		void convertModel(const std::string& filePath);

		BeDevice* getDevice();
		void setDevice(BeDevice* d);
		void setRegeneratingAssets(bool b);
		bool getRegeneratingAssets();
		void setWaitingToRegenerateAssets(bool b);
		bool getWaitingToRegenerateAssets();
		void setClearToRegenerateAssets(bool b);
		bool getClearToRegenerateAssets();

		//TTF
		void initializeFontStuff(uint32_t width, uint32_t height, VkDescriptorSetLayout layout, std::unique_ptr<BeDescriptorPool>& pool);
		void shutdownFontStuff();
		FT_Library& getFontLibrary();
		std::shared_ptr<TextureAtlas> getTextureAtlas();

		/*
			Serialisation/Deserialisation of asset ID and the associated file name
		*/
		void Serialise(const std::string& output);
		void Deserialise(const std::string& output);

		/*
			Event handling
		*/

		//Unload assets from previous scene, and start loading assets sfor new scene
		void OnSceneUnload(IEvent& e);

	private:
		std::vector<std::string> filePaths{};
		BeDevice* deviceToUse;
		bool regeneratingAssets{};
		bool waitingToRegenerateAssets{};
		bool clearToRegenerateAssets{};

		//For font
		FT_Library fontLibrary{};
		std::shared_ptr<TextureAtlas> textureAtlas;

		//Get all the assets by types
		template <typename AssetType>
		AssetDataContainer<AssetType>* GetAssetDataContainer();

		//Get all the assets by types - const version
		template <typename AssetType>
		AssetDataContainer<AssetType> const* GetAssetDataContainer() const;

		friend class Singleton<AssetManager>;
	};

	#define AssetMgr AssetManager::GetInstance()
}

#include "AssetManager.hpp"

#endif