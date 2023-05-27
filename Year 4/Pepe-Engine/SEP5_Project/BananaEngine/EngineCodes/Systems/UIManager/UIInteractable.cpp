#include "pch.h"
#include "UIInteractable.h"

#include <AssetManager/AssetManager.h>
#include <Serialisation/BananaSerialisation.h>


namespace BE
{
	UIInteractable::UIInteractable() : m_pitch(1.0f), m_volume(1.0f), m_priority(128), m_currentStatus(UIInteractStatus::normal)
	{
		for (size_t i = 0; i < m_textures.size(); ++i)
			m_textures[i] = AssetID{ 0 };

		for (size_t i = 0; i < m_audioQues.size(); ++i)
			m_audioQues[i] = AssetID{ 0 };
	}

	UIInteractable::~UIInteractable()
	{

	}

	void UIInteractable::SetTexture(UIInteractStatus _interactStatus, AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()), 
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not a texture", _aID);
		m_textures[(size_t)_interactStatus] = _aID;
		SetHasTexturesChanged(true);
	}

	//Set the audio for the given audio que type, set _aID to 0 to remove audio que
	void UIInteractable::SetAudioQue(UIAudioQueType _audioQueType, AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<AudioClip>()), 
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not an audioclip", _aID);
		m_audioQues[(size_t)_audioQueType] = _aID;
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<UIInteractable>(RegistrationNameCreator<UIInteractable>())
		.constructor()
		.property("Textures", &UIInteractable::m_textures)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("AudioQues", &UIInteractable::m_audioQues)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("AudioQue Volume", &UIInteractable::GetVolume, &UIInteractable::SetVolume)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("AudioQue Pitch", &UIInteractable::GetPitch, &UIInteractable::SetPitch)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("AudioQue Priority", &UIInteractable::GetPriority, &UIInteractable::SetPriority)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}