#pragma once

#include <filesystem>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glvis/texture.h"

namespace glvis {

const unsigned int FONT_DEFAULT_SIZE = 30;

struct Character {
    Texture texture;
    int x, y;
    int advance;
};

class Font {
public:
    Font() = default;
    ~Font();
    Font(const std::filesystem::path& filename, unsigned int character_size = FONT_DEFAULT_SIZE);
    void openFromFile(
        const std::filesystem::path& filename,
        unsigned int character_size = FONT_DEFAULT_SIZE
    );
    int getCharacterSize() const;
    int getBaselineY() const;
    Character& getCharacter(unsigned char c);

private:
    friend class TextTestsModule;
    static bool is_library_initialized;
    static FT_Library library;
    FT_Face face = nullptr;
    std::map<unsigned char, Character> characters;
    unsigned int character_size = FONT_DEFAULT_SIZE;

    void loadFont(const std::filesystem::path& filename, unsigned int size);

};

}
