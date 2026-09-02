from pathlib import Path

FULL_BDF = Path("fonts/shnmk16.bdf")
HALF_BDF = Path("fonts/shnm8x16r.bdf")

OUT_H = Path("include/ShinonomeFontData.h")
OUT_CPP = Path("src/ShinonomeFontData.cpp")


def jis0208_to_unicode(code):
    hi = ((code >> 8) & 0xFF) | 0x80
    lo = (code & 0xFF) | 0x80

    return ord(bytes([hi, lo]).decode("euc_jp"))


def jis0201_to_unicode(code):
    if 0x20 <= code <= 0x7E:
        return code

    if 0xA1 <= code <= 0xDF:
        return ord(bytes([code]).decode("shift_jis"))

    return None


def read_bdf(path, width, converter):
    lines = path.read_text(encoding="ascii").splitlines()

    glyphs = []

    encoding = None
    bitmap = None

    for line in lines:
        if line.startswith("STARTCHAR"):
            encoding = None
            bitmap = None

        elif line.startswith("ENCODING"):
            encoding = int(line.split()[1])

        elif line == "BITMAP":
            bitmap = []

        elif line == "ENDCHAR":
            if encoding is None or bitmap is None:
                continue

            codepoint = converter(encoding)

            if codepoint is None:
                continue

            data = []

            for row in bitmap:
                value = int(row, 16)

                if width == 8:
                    data.append(value)
                else:
                    data.append((value >> 8) & 0xFF)
                    data.append(value & 0xFF)

            glyphs.append((codepoint, width, data))

            bitmap = None

        elif bitmap is not None:
            bitmap.append(line)

    return glyphs


def main():
    glyphs = []
    glyphs += read_bdf(HALF_BDF, 8, jis0201_to_unicode)
    glyphs += read_bdf(FULL_BDF, 16, jis0208_to_unicode)

    # Unicodeが重複した場合は先に入った半角側を優先
    unique = {}

    for glyph in glyphs:
        codepoint = glyph[0]

        if codepoint not in unique:
            unique[codepoint] = glyph

    glyphs = sorted(unique.values(), key=lambda g: g[0])

    OUT_H.write_text(
"""\
#pragma once

#include <Arduino.h>

struct ShinonomeGlyphIndex {
    uint32_t codepoint;
    uint32_t bitmapOffset;
    uint8_t width;
};

extern const uint8_t shinonomeBitmap[];
extern const ShinonomeGlyphIndex shinonomeIndex[];
extern const uint16_t SHINONOME_GLYPH_COUNT;

constexpr int SHINONOME_HEIGHT = 16;
""",
        encoding="utf-8",
    )

    with OUT_CPP.open("w", encoding="utf-8") as f:
        f.write('#include "ShinonomeFontData.h"\n\n')
        f.write("const uint8_t shinonomeBitmap[] PROGMEM = {\n")

        offset = 0
        indices = []

        for codepoint, width, bitmap in glyphs:
            # 何らかの文字で囲むこと(\がMutli Line Comment判定となるため)
            f.write(f"    // U+{codepoint:04X} [{chr(codepoint)}]\n    ")
            f.write(", ".join(f"0x{b:02X}" for b in bitmap))
            f.write(",\n")

            indices.append((codepoint, offset, width))
            offset += len(bitmap)

        f.write("};\n\n")
        f.write("const ShinonomeGlyphIndex " "shinonomeIndex[] PROGMEM = {\n")

        for codepoint, offset, width in indices:
            f.write(f"    {{ 0x{codepoint:04X}, " f"{offset}, {width} }},\n")

        f.write("};\n\n")
        f.write(f"const uint16_t SHINONOME_GLYPH_COUNT = " f"{len(glyphs)};\n")

    print(f"{len(glyphs)} glyphs generated")


if __name__ == "__main__":
    main()
