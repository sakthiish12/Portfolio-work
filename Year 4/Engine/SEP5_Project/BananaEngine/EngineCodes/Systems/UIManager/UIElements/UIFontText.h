#pragma once
#ifndef UIFONTTEXT_H
#define UIFONTTEXT_H
#include "../UIElement.h"

namespace BE
{
	enum class UIFontTextAlignment
	{
		left,
		center,
		right
	};

	class UIFontText : public UIElement
	{
		std::string m_text{ "Text" };										//What text to display?
		AssetID m_fontFileID{ BEID{0} };									//Which font is used? (Store the .ttf file in asset manager)
		std::uint32_t m_size{1};												//The size of the text
		glm::vec3 m_colours{1.0f,1.0f,1.0f};												//Colour of the text
		UIFontTextAlignment m_alignment{ UIFontTextAlignment::center };		//Alignment of the text

	public:
		UIFontText(CanvasID _cid, UIElementID _uid);
		~UIFontText();

		UIELEMENT_FUNCTIONS;

		//Text property
		inline const std::string& GetText() const					{ return m_text; }
		inline const char* GetTextCStr() const						{ return m_text.c_str(); }
		inline void SetText(std::string&& _newText)					{ m_text = std::move(_newText); }
		inline void SetText2(const std::string& _newText) { m_text =_newText; }
		inline void SetTextCStr(const char* _newText)				{ m_text = _newText; }
		inline bool IsTextEmpty() const								{ return m_text.empty(); }
		inline size_t GetLength() const								{ return m_text.size(); }

		//Font file property
		inline AssetID GetFont() const								{ return m_fontFileID; }
		void SetFont(AssetID _newFont);		
		inline bool HasFont() const									{ return m_fontFileID != NULL_ASSET; }

		inline void SetAlignment(UIFontTextAlignment _newAlign)		{ m_alignment = _newAlign; }
		inline UIFontTextAlignment GetAlignment() const				{ return m_alignment; }

		//Size property
		inline std::uint32_t GetSize() const						{ return m_size; }
		inline void SetSize(std::uint32_t _newSize)					{ m_size = _newSize; }

		//Colour properties
		inline const glm::vec3& GetColour() const					{ return m_colours; }
		inline void SetColour(const glm::vec3& _newColour)			{ m_colours = _newColour; }

		//Modifications
		void AppendInt(int _value, size_t _minWidth = 0, char _extraAppendChar = ' ', bool _alignLeft = false);
		void AppendFloatAsInt(float _value, size_t _minWidth = 0, char _extraAppendChar = ' ', bool _alignLeft = false);
		void AppendFloat(float _value, size_t _precision = 3, size_t _minWidth = 0, char _extraAppendChar = ' ', bool _alignLeft = false);
		void AppendString(const char* _str, size_t _minWidth = 0, char _extraAppendChar = ' ', bool _alignLeft = false);
		void SetChar(char _c, size_t index);
		char GetChar(size_t index) const;

		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement);
	};
}
#endif
