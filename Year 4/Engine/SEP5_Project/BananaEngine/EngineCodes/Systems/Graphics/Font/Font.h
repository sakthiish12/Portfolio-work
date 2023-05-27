#pragma once
#include <pch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Glyph.h"
#include "../Be_frame_info.hpp"
#include "../../../AssetManager/AssetTypes/IAsset.h"

namespace BE
{
	class TextureAtlas;

	struct FontInitialiser
	{
		std::string filePath;
		FontInitialiser() = default;
		FontInitialiser(const FontInitialiser& in) = default;
		FontInitialiser(const std::string& in);
		RTTR_ENABLE()
	};

	class Font : public IAsset
	{
	public:
		Font() = default;
		Font(const std::string& filePath);
		Font(Font&& _rhs) noexcept;
		Font& operator=(Font&& _rhs) noexcept;

		int getNumberOfGlyphs();
		std::shared_ptr<Glyph> getGlyph(uint16_t c);
		glm::vec2 MeasureTextDimensions(const std::string& text);
		std::string& getFontName();
		std::string& getFontStyle();

		void Construct(const FontInitialiser& _data);
		void ConstructFromPointer(FontInitialiser* _data);
		FontInitialiser Serialise()
		{
			return FontInitialiser();
		}
	protected:
		FT_Face fontFace;
		std::shared_ptr<TextureAtlas> textureAtlas;
		std::map<uint16_t, std::shared_ptr<Glyph>> glyphs;
		std::string fontName{}, fontStyle{}; //Eg Roboto/Arial, Regular/Bold
		RTTR_ENABLE(IAsset)
	};
}