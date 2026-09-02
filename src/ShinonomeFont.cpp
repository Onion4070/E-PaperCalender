#include "ShinonomeFont.h"

#include "GUI_Paint.h"
#include "ShinonomeFontData.h"

uint32_t ShinonomeFont::decodeUtf8(const char*& text) {
    const uint8_t* s = (const uint8_t*)(text);

    uint32_t codepoint;

    // ASCII
    if (s[0] < 0x80) {
        codepoint = s[0];
        text += 1;
    }

    // 2 byte UTF-8
    else if ((s[0] & 0xE0) == 0xC0) {
        codepoint = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        text += 2;
    }

    // 3 byte UTF-8
    else if ((s[0] & 0xF0) == 0xE0) {
        codepoint = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        text += 3;
    }

    // 4 byte UTF-8
    else if ((s[0] & 0xF8) == 0xF0) {
        codepoint = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
                    ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        text += 4;
    }

    // 不正なUTF-8
    else {
        codepoint = '?';
        text += 1;
    }

    return codepoint;
}

int ShinonomeFont::findGlyph(uint32_t codepoint) {
    int left = 0;
    int right = SHINONOME_GLYPH_COUNT - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        uint32_t current = pgm_read_dword(&shinonomeIndex[mid].codepoint);

        if (current == codepoint) {
            return mid;
        }

        if (current < codepoint) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    return -1;
}

uint8_t ShinonomeFont::getGlyphWidth(int glyphIndex) {
    return pgm_read_byte(&shinonomeIndex[glyphIndex].width);
}

void ShinonomeFont::drawGlyph(int x, int y, int glyphIndex, uint16_t color) {
    uint32_t offset = pgm_read_dword(&shinonomeIndex[glyphIndex].bitmapOffset);
    uint8_t width = getGlyphWidth(glyphIndex);

    // 半角 8x16
    if (width == 8) {
        for (int row = 0; row < 16; row++) {
            uint8_t bits = pgm_read_byte(&shinonomeBitmap[offset + row]);

            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    Paint_SetPixel(x + col, y + row, color);
                }
            }
        }
    }

    // 全角 16x16
    else if (width == 16) {
        for (int row = 0; row < 16; row++) {
            uint8_t high = pgm_read_byte(&shinonomeBitmap[offset + row * 2]);
            uint8_t low = pgm_read_byte(&shinonomeBitmap[offset + row * 2 + 1]);

            uint16_t bits = (static_cast<uint16_t>(high) << 8) | low;

            for (int col = 0; col < 16; col++) {
                if (bits & (0x8000 >> col)) {
                    Paint_SetPixel(x + col, y + row, color);
                }
            }
        }
    }
}

void ShinonomeFont::drawText(int x, int y, const char* text, uint16_t color) {
    while (*text != '\0') {
        uint32_t codepoint = decodeUtf8(text);

        int glyphIndex = findGlyph(codepoint);

        // 該当文字なし
        if (glyphIndex < 0) {
            continue;
        }

        drawGlyph(x, y, glyphIndex, color);
        x += getGlyphWidth(glyphIndex);
    }
}