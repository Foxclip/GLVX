#pragma once

#include <filesystem>
#include <array>
#include <memory>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glvx/texture.h"
#include "glvx/vector.h"

namespace glvx {

const unsigned int FONT_DEFAULT_SIZE = 30;
// Glyph codes are unsigned char, so the flat per-size tables cover the full
// 0-255 range and are indexed directly (O(1)) instead of using std::map.
const unsigned int FONT_CHARACTER_COUNT = 256;
const unsigned int FONT_KERNING_TABLE_SIZE = FONT_CHARACTER_COUNT * FONT_CHARACTER_COUNT;

struct Character {
    Vector2f uv_top_left;
    Vector2f uv_bottom_right;
    int width;
    int glyph_height;
    int x;
    int top;
    int advance;
    int lsb_delta = 0;
    int rsb_delta = 0;
};

class Font {
public:
    Font() = default;
    ~Font();
    Font(const std::filesystem::path& filename, bool use_subpixel = false);
    void openFromFile(const std::filesystem::path& filename, bool use_subpixel = false);
    bool isSubpixel() const;
    Character& getCharacter(unsigned int character_size, unsigned char c);
    const Texture& getAtlas(unsigned int character_size);
    int getKerning(unsigned int character_size, unsigned char left, unsigned char right);
    int getLineHeight(unsigned int character_size);
    int getBaselineY(unsigned int character_size);

private:
    struct SizePage {
        Texture m_atlas;
        // Indexed directly by character code (unsigned char).
        std::array<Character, FONT_CHARACTER_COUNT> m_characters{};
        // Indexed by left * FONT_CHARACTER_COUNT + right; zero means no kerning.
        std::array<int, FONT_KERNING_TABLE_SIZE> m_kerning{};
        int m_line_height = 0;
        int m_ascender = 0;
        bool m_rasterized = false;
    };
    friend class TextTestsModule;
    static bool m_is_library_initialized;
    static FT_Library m_library;
    FT_Face m_face = nullptr;
    // Indexed by character size; null entries mean "size not loaded yet".
    std::vector<std::unique_ptr<SizePage>> m_sizes;
    bool m_use_subpixel = false;

    SizePage& loadPage(unsigned int character_size);
    SizePage& loadMetadata(unsigned int character_size);
    void rasterizePage(SizePage& page);
};

}
