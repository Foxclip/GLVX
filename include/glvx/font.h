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
    Font(const std::filesystem::path& filename, unsigned int character_size = FONT_DEFAULT_SIZE, bool use_subpixel = false);
    void openFromFile(
        const std::filesystem::path& filename,
        unsigned int character_size = FONT_DEFAULT_SIZE,
        bool use_subpixel = false
    );
    int getCharacterSize() const;
    int getBaselineY() const;
    int getLineHeight() const;
    bool isSubpixel() const;
    Character& getCharacter(unsigned char c);
    const Texture& getAtlas() const;
    int getKerning(unsigned char left, unsigned char right) const;

private:
    friend class TextTestsModule;
    static bool m_is_library_initialized;
    static FT_Library m_library;
    Texture m_atlas;
    FT_Face m_face = nullptr;
    std::map<unsigned char, Character> m_characters;
    std::map<std::pair<unsigned char, unsigned char>, int> m_kerning;
    unsigned int m_character_size = FONT_DEFAULT_SIZE;
    bool m_use_subpixel = false;

    void loadFont(const std::filesystem::path& filename, unsigned int size, bool use_subpixel);

};

}
