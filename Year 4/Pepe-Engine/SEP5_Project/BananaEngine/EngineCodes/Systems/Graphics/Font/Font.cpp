#pragma once
#include "pch.h"
#include "Font.h"
#include <Serialisation/BananaSerialisation.h>
#include "../../../AssetManager/AssetManager.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<Font>(BE::RegistrationNameCreator<Font>())
	.constructor()()
	.method(assetConstructMethodName, &Font::ConstructFromPointer)()
	.method(serializeAssetMethodName, &Font::Serialise)();
	registration::class_<FontInitialiser>(BE::RegistrationNameCreator<FontInitialiser>())
	.constructor()()
	.property(filePathName, &FontInitialiser::filePath)
		;
}
namespace BE
{
	Font::Font(const std::string& filePath)
	{
		FontInitialiser TextIni{ filePath};
		try
		{
			this->Construct(TextIni);
		}
		catch (const std::runtime_error&)
		{
			throw;
		}
	}

	Font::Font(Font&& _rhs) noexcept
	{
		fontFace = _rhs.fontFace;
		textureAtlas = _rhs.textureAtlas;
		glyphs = _rhs.glyphs;
		fontName = _rhs.fontName;
		fontStyle = _rhs.fontStyle;
	}

	Font& Font::operator=(Font&& _rhs) noexcept
	{
		fontFace = _rhs.fontFace;
		textureAtlas = _rhs.textureAtlas;
		glyphs = _rhs.glyphs;
		fontName = _rhs.fontName;
		fontStyle = _rhs.fontStyle;
		return *this;
	}

	int Font::getNumberOfGlyphs()
	{ 
		return static_cast<int>(fontFace->num_glyphs); 
	}
	std::shared_ptr<Glyph> Font::getGlyph(uint16_t c)
	{
		auto foundIt = glyphs.find(c);
		if (foundIt != glyphs.end()) {
			return foundIt->second;
		}

		const auto glyphIndex = FT_Get_Char_Index(fontFace, c);
		const auto glyph = std::make_shared<Glyph>(fontFace, glyphIndex, textureAtlas);

		glyphs[c] = glyph;
		return glyph;
	}
	glm::vec2 Font::MeasureTextDimensions(const std::string& text)
	{
		int width = 0;
		int height = 0;

		for (auto c : text) {
			auto glyph = getGlyph(c);
			width += glyph->getAdvance();
			height = std::max(height, glyph->getTop() + glyph->getHeight());
		}

		return glm::vec2{ width, height };
	}
	std::string& Font::getFontName()
	{
		return fontName;
	}
	std::string& Font::getFontStyle()
	{
		return fontStyle;
	}
	void Font::Construct(const FontInitialiser& _data)
	{
		FT_Face face;
		auto error = FT_New_Face(AssetMgr->getFontLibrary(), _data.filePath.c_str(), 0, &face);
		if (!error) {
			error = FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(loadFontSize));
			if (error) {
				std::cout << "Couldn't set pixel sizes\n";
				//throw std::runtime_error("couldn't set pixel sizes");
			}
			fontFace = face;
			textureAtlas = AssetMgr->getTextureAtlas();;
			fontName = face->family_name;
			fontStyle = face->style_name;
		}
		else
		{
			std::cout << "Couldn't load font\n";
			//throw std::runtime_error("couldn't load font");
		}
	}
	void Font::ConstructFromPointer(FontInitialiser* _data)
	{
		Construct(*_data);
	}
	FontInitialiser::FontInitialiser(const std::string& in) : 
		filePath{ in }
	{
	}
}