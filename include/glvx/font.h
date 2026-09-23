#pragma once

#include <filesystem>
#include <map>
#include <utility>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glvx/texture.h"
#include "glvx/vector.h"

namespace glvx {

const unsigned int FONT_DEFAULT_SIZE = 30;

struct Character {
    Vector2f uv_top_left;
    Vector2f uv_bottom_right;
    int width;
    int glyph_height;
    int x;
    int top;
    int advance;
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
        std::map<unsigned char, Character> m_characters;
        std::map<std::pair<unsigned char, unsigned char>, int> m_kerning;
        int m_line_height = 0;
        int m_ascender = 0;
        bool m_metadata_loaded = false;
        bool m_rasterized = false;
    };
    friend class TextTestsModule;
    static bool m_is_library_initialized;
    static FT_Library m_library;
    FT_Face m_face = nullptr;
    std::map<unsigned int, SizePage> m_sizes;
    bool m_use_subpixel = false;

    SizePage& loadPage(unsigned int character_size);
    SizePage& loadMetadata(unsigned int character_size);
    void rasterizePage(SizePage& page);
};

}
