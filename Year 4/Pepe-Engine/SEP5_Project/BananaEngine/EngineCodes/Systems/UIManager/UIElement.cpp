#include "pch.h"
#include "UIElement.h"
#include <Serialisation/BananaSerialisation.h>
#include <Engine/GameEngine.h>

namespace BE
{
	UIElement::UIElement(UIElementType _type, CanvasID _cid, UIElementID _uid) : m_type(_type), m_canvasID(_cid), m_uiID(_uid)
	{
		AssertCore(_type != UIElementType::none, "Cannot create UI Element with \"None\" type");
		m_uiFlags.set((size_t)UIElementFlags::isActive, true);
	}

	UIElement::~UIElement()
	{

	}

	void UIElement::InitUI()
	{
		//Initialise this UI Element
		for (const ScriptPtr& script : m_script.GetAllScripts())
			script->Init();
	}

	void UIElement::UpdateUI(double dt)
	{
		//Call update for this UI Element
		for (const ScriptPtr& script : m_script.GetAllScripts())
			script->Update(dt);
	}

	void UIElement::UninitUI()
	{
		//Clean up the UI Element
		for (const ScriptPtr& script : m_script.GetAllScripts())
			script->Cleanup();
	}

	void UIElement::SetUIHorizontalAlignment(UIHorizontalAlignment _newAlign)
	{
		m_horiAlign = _newAlign;
		UpdateReadOnlyTransform();
	}

	void UIElement::SetUIVerticalAlignment(UIVerticalAlignment _newAlign)
	{
		m_vertAlign = _newAlign;
		UpdateReadOnlyTransform();
	}

	void UIElement::SetUIPos(const Vec3f& _newPos)
	{
		m_uiPos = _newPos;
		UpdateReadOnlyTransform();
	}

	void UIElement::SetUIScale(const Vec3f& _newScale)
	{
		m_uiScale = _newScale;
		UpdateReadOnlyTransform();
	}

	void UIElement::SetUIElementFlag_AsUInt(std::uint32_t _flags)
	{
		for (size_t i = 0; i < m_uiFlags.size(); ++i)
		{
			SetUIElementFlag((UIElementFlags)i, _flags & (1 << i));
		}
	}

	void UIElement::UpdateReadOnlyTransform()
	{
		const Vec3f& offset = UI->GetCanvas(m_canvasID)->GetUIOffset();

		//Determine x position after alignment
		switch (m_horiAlign)
		{
		case UIHorizontalAlignment::left:
		{
			/*
				+-----------+
				X			|
				+-----------+
			*/
			m_drawPos.x = m_uiPos.x + offset.x + m_uiScale.x / 2.0f;
			break;
		}
		case UIHorizontalAlignment::right:
		{
			/*
				+-----------+
				|			X
				+-----------+
			*/
			m_drawPos.x = m_uiPos.x + offset.x - m_uiScale.x / 2.0f;
			break;
		}
		default:
		{
			/*
				+-----------+
				|	  X 	|
				+-----------+
			*/
			//No need to adjust for middle alignment
			m_drawPos.x = m_uiPos.x + offset.x;
			break;
		}
		}

		//Determine y position after alignment
		switch (m_vertAlign)
		{
		case UIVerticalAlignment::bottom:
		{
			/*
				+-----------+
				|			|
				+-----X-----+
			*/
			m_drawPos.y = m_uiPos.y + offset.y - m_uiScale.y / 2.0f;
			break;
		}
		case UIVerticalAlignment::top:
		{
			/*
				+-----X-----+
				|			|
				+-----------+
			*/
			m_drawPos.y = m_uiPos.y + offset.y + m_uiScale.y / 2.0f;
			break;
		}
		default:
		{
			/*
				+-----------+
				|	  X 	|
				+-----------+
			*/
			//No need to adjust for middle alignment
			m_drawPos.y = m_uiPos.y + offset.y;
			break;
		}
		}

		
#ifdef USE_EDITOR
		//Compiled with editor: Set screen pos and scale based on Editor's screen
		const auto& [editorWidth, editorHeight] = Debug::GetInstance()->GetEditorWindowSize();

		m_screenPos.x = m_drawPos.x * editorWidth;
		m_screenPos.y = m_drawPos.y * editorHeight;

		m_screenScale.x = m_uiScale.x * editorWidth;
		m_screenScale.y = m_uiScale.y * editorHeight;
#else
		//Compiled without editor: Set screen pos and scale based on GLFW's screen
		m_screenPos.x = m_drawPos.x * Engine->GetWindowWidth();
		m_screenPos.y = m_drawPos.y * Engine->GetWindowHeight();

		m_screenScale.x = m_uiScale.x * Engine->GetWindowWidth();
		m_screenScale.y = m_uiScale.y * Engine->GetWindowHeight();
#endif
	}

	void UIElement::Serialise()
	{
		// Shaun: serialise base class
	}

	void UIElement::Deserialise()
	{
		// Shaun: deserialise base class
	}
	std::vector<VkDescriptorSet>& UIElement::getDescriptorSets()
	{
		return currentSet;
	}
	std::vector<VkWriteDescriptorSet>& UIElement::getWrites()
	{
		return writes;
	}
	bool UIElement::getHasTexturesChanged() const
	{
		return hasTexturesChanged;
	}
	float UIElement::getBrightnessFactor() const
	{
		return brightnessFactor;
	}
	float UIElement::getOpacity() const
	{
		return opacity;
	}
	bool UIElement::getHasInitialized() const
	{
		return hasInitialized;
	}
	const glm::vec4& UIElement::getColorIntensityFactor() const
	{
		return colorIntensityFactor;
	}
	const glm::vec4& UIElement::getFlickerColor() const
	{
		return flickerColor;
	}
	bool UIElement::getUseCustomColor() const
	{
		return useCustomColorAndOpacity;
	}
	void UIElement::setHasTexturesChanged(bool b)
	{
		hasTexturesChanged = b;
	}
	void UIElement::setBrightnessFactor(float f)
	{
		brightnessFactor = f;
	}
	void UIElement::setOpacity(float f)
	{
		opacity = f;
	}
	void UIElement::setHasInitialized(bool b)
	{
		hasInitialized = b;
	}
	void UIElement::setColorIntensityFactor(const glm::vec4& v)
	{
		colorIntensityFactor = v;
	}
	void UIElement::setFlickerColor(const glm::vec4& v)
	{
		flickerColor = v;
	}
	void UIElement::setUseCustomColor(bool b)
	{
		useCustomColorAndOpacity = b;
	}
	void UIElement::InitializeForRendering(BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, VkDescriptorImageInfo& blankImage,BEID textureToUse,
	BEID textureToUse2,BEID textureToUse3)
	{
		for (int i{}; i < BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			currentSet.emplace_back(VK_NULL_HANDLE);
			BE::BeDescriptorWriter(setLayout, pool)
				.writeImage(1, textureToUse ? &AssetMgr->GetAssetData<Texture>(textureToUse).getDescriptorImageInfo() : &blankImage)
				.writeImage(2, textureToUse2 ? &AssetMgr->GetAssetData<Texture>(textureToUse2).getDescriptorImageInfo() : &blankImage)
				.writeImage(3, textureToUse3 ? &AssetMgr->GetAssetData<Texture>(textureToUse3).getDescriptorImageInfo() : &blankImage)
				.build(currentSet[i]);
		}
	}
	void UIElement::addTexturesToUpdateList(BeDescriptorSetLayout& setLayout, VkDescriptorImageInfo& blankImage, BEID textureToUse,
		BEID textureToUse2, BEID textureToUse3)
	{
		for (int i{}; i < currentSet.size(); ++i)
		{
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorType = setLayout.getBindings()[1].descriptorType;
			write.dstBinding = 1;
			if (textureToUse != 0)
				write.pImageInfo = &AssetMgr->GetAssetData<Texture>(textureToUse).getDescriptorImageInfo(); //this
			else
				write.pImageInfo = &blankImage;
			write.descriptorCount = 1;
			write.dstSet = currentSet[i];
			writes.emplace_back(write); //this

			write.descriptorType = setLayout.getBindings()[2].descriptorType;
			write.dstBinding = 2;
			if (textureToUse2 != 0)
				write.pImageInfo = &AssetMgr->GetAssetData<Texture>(textureToUse2).getDescriptorImageInfo(); //this
			else
				write.pImageInfo = &blankImage;
			writes.emplace_back(write); //this

			write.descriptorType = setLayout.getBindings()[3].descriptorType;
			write.dstBinding = 3;
			if (textureToUse3 != 0)
				write.pImageInfo = &AssetMgr->GetAssetData<Texture>(textureToUse3).getDescriptorImageInfo(); //this
			else
				write.pImageInfo = &blankImage;
			writes.emplace_back(write); //this
		}
	}
	void UIElement::InitializeForRenderingText(BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, VkDescriptorImageInfo& blankImage, VkDescriptorImageInfo& glyphTexture)
	{
		for (int i{}; i < BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			currentSet.emplace_back(VK_NULL_HANDLE);
			BE::BeDescriptorWriter(setLayout, pool)
				.writeImage(1, &blankImage)
				.writeImage(2, &blankImage)
				.writeImage(3, &blankImage)
				.build(currentSet[i]);
		}
	}
	void UIElement::updateDescriptorSet(BeDevice& device)
	{
		vkUpdateDescriptorSets(device.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr); //this
		writes.clear();
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	rttr::registration::enumeration<UIElementType>(RegistrationNameCreator<UIElementType>())
		(
			value("UIButton", UIElementType::button),
			value("UIFontText", UIElementType::fontText),
			value("UIImage", UIElementType::image),
			value("UIProgressBar", UIElementType::progressBar),
			value("UISlider", UIElementType::slider),
			value("UIToggle", UIElementType::toggle)
			);
	rttr::registration::enumeration<UIHorizontalAlignment>(RegistrationNameCreator<UIHorizontalAlignment>())
		(
			value("left", UIHorizontalAlignment::left),
			value("middle", UIHorizontalAlignment::middle),
			value("right", UIHorizontalAlignment::right)
			);
	rttr::registration::enumeration<UIVerticalAlignment>(RegistrationNameCreator<UIVerticalAlignment>())
		(
			value("top", UIVerticalAlignment::top),
			value("middle", UIVerticalAlignment::middle),
			value("bottom", UIVerticalAlignment::bottom)
			);
	registration::class_<UIElement>(RegistrationNameCreator<UIElement>())
		.property("Sort Layer", &UIElement::GetSortLayer, &UIElement::SetSortLayer)
		(
			//Use UI Canvas's Set Sort Layer to set this value
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Horizontal Alignment", &UIElement::GetUIHorizontalAlignment, &UIElement::SetUIHorizontalAlignment)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Vertical Alignment", &UIElement::GetUIVerticalAlignment, &UIElement::SetUIVerticalAlignment)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Position", &UIElement::GetUIPos, &UIElement::SetUIPos)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Scale", &UIElement::GetUIScale, &UIElement::SetUIScale)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Flags", &UIElement::GetUIElementFlag_AsUInt, &UIElement::SetUIElementFlag_AsUInt)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Name", &UIElement::GetName, &UIElement::SetName)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("UIScripts", &UIElement::m_script)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Brightness", &UIElement::getBrightnessFactor, &UIElement::setBrightnessFactor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("color", &UIElement::getFlickerColor, &UIElement::setFlickerColor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Opacity", &UIElement::getOpacity, &UIElement::setOpacity)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("RGBintensity", &UIElement::getColorIntensityFactor, &UIElement::setColorIntensityFactor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}