
#include "pch.h"
#include "UIProgressBar.h"
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	UIProgressBar::UIProgressBar(CanvasID _cid, UIElementID _uid) : UIElement(UIElementType::progressBar, _cid, _uid)
	{
		m_uiElementName = "New Progress Bar";
		for (size_t i = 0; i < m_textures.size(); ++i)
			m_textures[i] = AssetID{ 0 };
	}

	UIProgressBar::~UIProgressBar()
	{

	}

	void UIProgressBar::InitUI()
	{
		UIElement::InitUI();
	}

	void UIProgressBar::UpdateUI(double dt)
	{
		//If UI Element is inactive, then just return
		if (!GetUIElementFlag(UIElementFlags::isActive))
			return;

		//Update bar flickering
		if (m_isFlickering)
		{
			m_flickerTimeElapsed += (float)dt;

			if (m_flickerTimesPerSecond * m_flickerTimeElapsed > 2.0f * (float)BEMath::PI)
				m_flickerTimeElapsed -= (2.0f * (float)BEMath::PI);
		}

		UIElement::UpdateUI(dt);
	}

	void UIProgressBar::UninitUI()
	{
		UIElement::UninitUI();
	}

	void UIProgressBar::Serialise()
	{
		UIElement::Serialise();
		// Shaun: serialise derived class only
	}

	void UIProgressBar::Deserialise()
	{
		UIElement::Deserialise();
		// Shaun: serialise derived class only
	}

	void UIProgressBar::SetFlickering(float _flickerRatePerSecond, float _flickerThreshold)
	{
		m_isFlickering = true;
		m_flickerTimesPerSecond = _flickerRatePerSecond;
		m_flickerThreshold = BEMath::Clamp(0.0f, 1.0f, _flickerThreshold);
	}

	float UIProgressBar::GetFlickerOpacity() const
	{
		//If going to flicker at below the threshold
		if (!m_reverseThreshold)
		{
			//If not flickering just return 1 or not below threshold, also return 1
			if (!m_isFlickering || (m_percentage > m_flickerThreshold))
				return 1.0f;
			else
				return 0.5f * BEMath::CosRad(m_flickerTimesPerSecond * m_flickerTimeElapsed) + 0.5f;
		}
		//If going to flicker at above the threshold
		else
		{
			//If not flickering just return 1 or not below threshold, also return 1
			if (!m_isFlickering || (m_percentage < 1.0f - m_flickerThreshold))
				return 1.0f;
			else
				return 0.5f * BEMath::CosRad(m_flickerTimesPerSecond * m_flickerTimeElapsed) + 0.5f;
		}
	}

	
	//Set the texture ID for the given progress bar parts, set _aID to 0 to remove texture
	void UIProgressBar::SetTexture(UIProgressBarParts _progBarPart, AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()), 
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not a texture", _aID);
		m_textures[(size_t)_progBarPart] = _aID;
		setHasTexturesChanged(true);
	}

}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;

	rttr::registration::enumeration<UIProgressBarType>(RegistrationNameCreator<UIProgressBarType>())
		(
			value("Horizontal", UIProgressBarType::horizontal),
			value("Vertical", UIProgressBarType::vertical),
			value("Circular", UIProgressBarType::circular)
			);

	registration::class_<UIProgressBar>(RegistrationNameCreator<UIProgressBar>())
		.constructor<CanvasID, UIElementID>()
		.property("Texture IDs", &UIProgressBar::m_textures)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Percentage", &UIProgressBar::GetPercentage, &UIProgressBar::SetPercentage)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Flicker", &UIProgressBar::GetIsFlickering, &UIProgressBar::SetIsFlickering)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Flicker Rate", &UIProgressBar::GetFlickerRate, &UIProgressBar::SetFlickerRate)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Flicker Threshold", &UIProgressBar::GetFlickerThreshold, &UIProgressBar::SetFlickerThreshold)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Type", &UIProgressBar::GetProgBarType, &UIProgressBar::SetProgBarType)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Reverse Threshold", &UIProgressBar::GetIsReverseThreshold, &UIProgressBar::SetIsReverseThreshold)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}