#pragma once

#include <Arduino.h>

class ShinonomeFont {
public:
    static void drawText(int x, int y, const char* text, uint16_t color);

private:
    static uint32_t decodeUtf8(const char*& text);
    static int findGlyph(uint32_t codepoint);
    static uint8_t getGlyphWidth(int glyphIndex);
    static void drawGlyph(int x, int y, int glyphIndex, uint16_t color);
};