#pragma once
#include "AssetData.h"

namespace BE
{
	template <typename AssetType>
	AssetDataContainer<AssetType>::AssetDataContainer()
	{
		m_data.reserve(512);
	}

	template <typename AssetType>
	AssetDataContainer<AssetType>::~AssetDataContainer()
	{
		//m_data's stuff should invoke the destructor here
	}




	template <typename AssetType>
	void AssetDataContainer<AssetType>::InsertData(AssetID _aID, AssetType&& _data)
	{
		//Duplicate checks for asset ID and file name is done in Asset Manager's Load Asset function
		//So no need to check here anymore

		m_assetIDToIndexMap[_aID] = m_data.size();
		m_indexToAssetIDMap[m_data.size()] = _aID;
		m_data.push_back(std::move(_data));
	}
	template <typename AssetType>
	void AssetDataContainer<AssetType>::InsertDataNT(AssetID _aID)
	{
		this->InsertData(_aID, AssetType{});
	}

	template<typename AssetType>
	inline void AssetDataContainer<AssetType>::InsertDataByBaseClass(AssetID _aID, IAsset&& data)
	{
		InsertData(_aID,  std::move(dynamic_cast<AssetType&&>(data)));
	}

	template <typename AssetType>
	void AssetDataContainer<AssetType>::RemoveData(AssetID _aID)
	{
		AssertCore(m_assetIDToIndexMap.count(_aID) != 0, "No asset with ID %llu was loaded", *_aID);

		size_t indexToReplace = m_assetIDToIndexMap.at(_aID);
		size_t indexToLast = m_data.size() - 1;
		AssetID lastAssetID = m_indexToAssetIDMap.at(indexToLast);

		//If the data that is going to be removed is not at the last, then swap with the last data
		if (indexToLast != indexToReplace)
			std::swap(m_data[indexToReplace], m_data[indexToLast]);
		
		m_data.pop_back();

		//Update the mapping
		m_assetIDToIndexMap[lastAssetID] = indexToReplace;
		m_indexToAssetIDMap[indexToReplace] = lastAssetID;

		//Erase the mapping for asset that has been deleted
		m_assetIDToIndexMap.erase(_aID);
		m_indexToAssetIDMap.erase(indexToLast);
	}

	template <typename AssetType>
	AssetType& AssetDataContainer<AssetType>::GetData(AssetID _aID)
	{
		AssertCore(m_assetIDToIndexMap.count(_aID) != 0, "No asset with ID %llu was loaded", *_aID);
		return m_data[m_assetIDToIndexMap[_aID]];
	}

	template <typename AssetType>
	IAsset& AssetDataContainer<AssetType>::GetDataNT(AssetID _aID)
	{
		return GetData(_aID);
	}

}