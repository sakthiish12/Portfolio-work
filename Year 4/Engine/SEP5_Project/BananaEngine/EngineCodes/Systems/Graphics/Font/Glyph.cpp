#pragma once
#include "pch.h"
#include "Glyph.h"

namespace BE
{
	Glyph::Glyph(FT_Face face, FT_UInt i, std::shared_ptr<TextureAtlas> ta)
	{
		fontFace = face;
		glyphIndex = i;

		auto error = FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_NO_HINTING);
		if (error) {
			throw std::runtime_error("failed to load glyph");
		}

		FT_GlyphSlot glyphSlot = fontFace->glyph;
		error = FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
		if (error) {
			throw std::runtime_error("failed to render glyph");
		}

		bitmapLeft = glyphSlot->bitmap_left;
		bitmapTop = glyphSlot->bitmap_top;
		width = static_cast<int>(glyphSlot->metrics.width / 64);
		height = static_cast<int>(glyphSlot->metrics.height / 64);
		advance = static_cast<int>(glyphSlot->advance.x / 64);

		createTextureFromBitmap(ta);
	}
	int Glyph::getLeft()
	{
		return bitmapLeft;
	}
	int Glyph::getTop()
	{
		return bitmapTop;
	}
	int Glyph::getWidth()
	{
		return width;
	}
	int Glyph::getHeight()
	{
		return height;
	}
	int Glyph::getAdvance()
	{
		return advance;
	}
	std::shared_ptr<FontTexture> Glyph::getTexture()
	{
		return texture;
	}

	void Glyph::createTextureFromBitmap(std::shared_ptr<TextureAtlas>& ta)
	{
		FT_GlyphSlot glyphSlot = fontFace->glyph;

		if (glyphSlot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY || glyphSlot->bitmap.num_grays != 256) {
			throw std::runtime_error("unsupported pixel mode");
		}

		const auto width2 = glyphSlot->bitmap.width;
		const auto height2 = glyphSlot->bitmap.rows;
		const auto bufferSize = width2 * height2 * 4;

		if (bufferSize == 0) {
			return;
		}

		std::vector<uint8_t> buffer(bufferSize);

		uint8_t* src = glyphSlot->bitmap.buffer;
		uint8_t* startOfLine = src;
		int dst = 0;

		for (int y = 0; y < static_cast<int>(height2); ++y) {
			src = startOfLine;
			for (int x = 0; x < static_cast<int>(width2); ++x) {
				const auto value = *src;
				src++;

				buffer[dst++] = 0xff;
				buffer[dst++] = 0xff;
				buffer[dst++] = 0xff;
				buffer[dst++] = value;
			}
			startOfLine += glyphSlot->bitmap.pitch;
		}
		
		texture = ta->addToTextureAtlas(width2, height2, buffer.data());
	}
}