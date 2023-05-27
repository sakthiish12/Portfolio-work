#include "pch.h"
#include "UIFontText.h"
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	UIFontText::UIFontText(CanvasID _cid, UIElementID _uid) : UIElement(UIElementType::fontText, _cid, _uid)
	{
		m_uiElementName = "New Font Text";
		m_text.reserve(100);
	}

	UIFontText::~UIFontText()
	{

	}

	void UIFontText::InitUI()
	{
		UIElement::InitUI();
	}

	void UIFontText::UpdateUI(double dt)
	{
		//If UI Element is inactive, then just return
		if (!GetUIElementFlag(UIElementFlags::isActive))
			return;

		UIElement::UpdateUI(dt);
	}

	void UIFontText::UninitUI()
	{
		UIElement::UninitUI();
	}

	void UIFontText::Serialise()
	{
		UIElement::Serialise();
		// Shaun: serialise derived class only
	}

	void UIFontText::Deserialise()
	{
		UIElement::Deserialise();
		// Shaun: serialise derived class only
	}

	void UIFontText::SetFont(AssetID _aID)
	{
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) /*&& AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<FontFile>()*/), 
			"Attempting to assign non-existing asset ID (%llu) to UI Element, or the asset ID is not a font file", _aID);
		m_fontFileID = _aID;
		setHasTexturesChanged(true);
	}

	void UIFontText::AppendInt(int _value, size_t _minWidth, char _extraAppendChar, bool _alignLeft)
	{
		//Convert to string
		std::string toAppend = std::to_string(_value);

		//If the size of the string to append is less than the min width
		if (toAppend.size() < _minWidth)
		{
			//Append additional character to fill up the width
			if (_alignLeft)
				m_text += toAppend + std::string(_minWidth - toAppend.size(), _extraAppendChar);
			else
				m_text += std::string(_minWidth - toAppend.size(), _extraAppendChar) + toAppend;
		}
		else
		{
			m_text += toAppend;
		}
	}

	void UIFontText::AppendFloatAsInt(float _value, size_t _minWidth, char _extraAppendChar, bool _alignLeft)
	{
		AppendInt((int)_value, _minWidth, _extraAppendChar, _alignLeft);
	}

	void UIFontText::AppendFloat(float _value, size_t _precision, size_t _minWidth, char _extraAppendChar, bool _alignLeft)
	{
		//Convert to string
		std::string toAppend = std::to_string(_value);

		//Strip off everything after the required precsion
		size_t eraseStart = toAppend.find_first_of('.') + _precision + 1;

		//If erasing start from within the string, erase everythign after the eraseStart
		if (eraseStart < toAppend.size())
			toAppend.erase(eraseStart, toAppend.size() - eraseStart);

		//If the size of the string to append is less than the min width
		if (toAppend.size() < _minWidth)
		{
			//Append additional character to fill up the width
			if (_alignLeft)
				m_text += toAppend + std::string(_minWidth - toAppend.size(), _extraAppendChar);
			else
				m_text += std::string(_minWidth - toAppend.size(), _extraAppendChar) + toAppend;
		}
		else
		{
			m_text += toAppend;
		}
	}

	void UIFontText::AppendString(const char* _str, size_t _minWidth, char _extraAppendChar, bool _alignLeft)
	{
		//Convert to string
		std::string toAppend{ _str };

		//If the size of the string to append is less than the min width
		if (toAppend.size() < _minWidth)
		{
			//Append additional character to fill up the width
			if (_alignLeft)
				m_text += toAppend + std::string(_minWidth - toAppend.size(), _extraAppendChar);
			else
				m_text += std::string(_minWidth - toAppend.size(), _extraAppendChar) + toAppend;
		}
		else
		{
			m_text += toAppend;
		}
	}

	void UIFontText::SetChar(char _c, size_t index)
	{
		//If out of index
		if (index >= m_text.size())
		{
			//Then just append the character
			while (index > m_text.size())
			{
				m_text.push_back(' ');
			}
			//Then place the character there
			m_text.push_back(_c);
		}
		else
		{
			//Otherwise just overwrite
			m_text[index] = _c;
		}
	}

	char UIFontText::GetChar(size_t index) const
	{
		if (index < m_text.size())
			return m_text[index];
		else
			return '\0';
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<UIFontText>(RegistrationNameCreator<UIFontText>())
		.constructor<CanvasID, UIElementID>()
		.property("Text", &UIFontText::GetText, &UIFontText::SetText2)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Font File ID", &UIFontText::GetFont, &UIFontText::SetFont)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Alignment", &UIFontText::GetAlignment, &UIFontText::SetAlignment)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Size", &UIFontText::GetSize, &UIFontText::SetSize)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Colour", &UIFontText::GetColour, &UIFontText::SetColour)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}