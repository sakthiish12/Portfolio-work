#include "pch.h"
#include "UIImage.h"
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	UIImage::UIImage(CanvasID _cid, UIElementID _uid) : UIElement(UIElementType::image, _cid, _uid)
	{
		m_uiElementName = "New Image";
	}

	UIImage::~UIImage()
	{
	}

	void UIImage::InitUI()
	{
		UIElement::InitUI();
	}

	void UIImage::UpdateUI(double dt)
	{
		//If UI Element is inactive, then just return
		if (!GetUIElementFlag(UIElementFlags::isActive))
			return;

		UIElement::UpdateUI(dt);
	}

	void UIImage::UninitUI()
	{
		UIElement::UninitUI();
	}

	void UIImage::SetTexture(AssetID _aID)
	{
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()), 
			"Attempting to assign non-existing asset ID (%llu) to UI Element, or the asset ID is not a texture", _aID);
		m_imageID = _aID; 
		setHasTexturesChanged(true);
	}

	void UIImage::Serialise()
	{
		UIElement::Serialise();
		// Shaun: serialise derived class only
	}

	void UIImage::Deserialise()
	{
		UIElement::Deserialise();
		// Shaun: serialise derived class only
	}

	void UIImage::SetUIRotDeg(float _newRotation)
	{
		SetUIRotRad(BEMath::DegToRad(_newRotation));
	}

	void UIImage::SetUIRotRad(float _newRotation)
	{
		m_uiRotationRadian = BEMath::Wrap(0.0f, (float)BEMath::PI_TIMES_2, _newRotation);
		//UpdateReadOnlyTransform();
	}

}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;

	registration::class_<UIImage>(RegistrationNameCreator<UIImage>())
		.constructor<CanvasID, UIElementID>()
		.property("Image Texture ID", &UIImage::GetTexture, &UIImage::SetTexture)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		).property("Rotation", &UIImage::GetUIRot, &UIImage::SetUIRotRad)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}