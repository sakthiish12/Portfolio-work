#pragma once
#include <pch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "TextureAtlas.h"

namespace BE
{
	struct FontTexture;
	class TextureAtlas;

	class Glyph
	{
	public:
		Glyph(FT_Face face, FT_UInt i, std::shared_ptr<TextureAtlas> ta);

		int getLeft();
		int getTop();
		int getWidth();
		int getHeight();
		int getAdvance();
		std::shared_ptr<FontTexture> getTexture();

	protected:
		FT_Face fontFace;
		int glyphIndex{};
		int bitmapLeft{}, bitmapTop{};
		int width{}, height{}, advance{};

		std::shared_ptr<FontTexture> texture{};

		void createTextureFromBitmap(std::shared_ptr<TextureAtlas>& ta);
	};
}