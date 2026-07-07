#pragma once

#include <filesystem>
#include <map>
#include <utility>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glvis/texture.h"
#include "glvis/vector.h"

namespace glvis {

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
    Font(const std::filesystem::path& filename, unsigned int character_size = FONT_DEFAULT_SIZE, bool useSubpixel = false);
    void openFromFile(
        const std::filesystem::path& filename,
        unsigned int character_size = FONT_DEFAULT_SIZE,
        bool useSubpixel = false
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
    static bool is_library_initialized;
    static FT_Library library;
    Texture atlas;
    FT_Face face = nullptr;
    std::map<unsigned char, Character> characters;
    std::map<std::pair<unsigned char, unsigned char>, int> kerning;
    unsigned int character_size = FONT_DEFAULT_SIZE;
    bool _useSubpixel = false;

    void loadFont(const std::filesystem::path& filename, unsigned int size, bool useSubpixel);

};

}
