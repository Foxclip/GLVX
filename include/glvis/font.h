#pragma once

#include <filesystem>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glvis/texture.h"

namespace glvis {

struct Character {
    Texture texture;
    int x, y;
    int advance;
};

class Font {
public:
    Font() = default;
    ~Font();
    Font(const std::filesystem::path& filename);
    void openFromFile(const std::filesystem::path& filename);

private:
    friend class TextTestsModule;
    static bool is_library_initialized;
    static FT_Library library;
    FT_Face face = nullptr;
    std::map<unsigned char, Character> characters;

    void loadFont(const std::filesystem::path& filename);

};

}
