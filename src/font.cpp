#include "glvis/font.h"
#include "glvis/glvis_common.h"

namespace glvis {

bool Font::is_library_initialized = false;
FT_Library Font::library = nullptr;

#define FREETYPE_CALL(func, messageGetter) \
    { \
        FT_Error error = func; \
        if (error != 0) { \
            throw std::runtime_error( \
                std::string(messageGetter()) \
                + ", error code: " \
                + std::to_string(error) \
            ); \
        } \
    }


Font::Font(const std::filesystem::path& filename, unsigned int character_size) {
    loadFont(filename, character_size);
}

Font::~Font() {
    if (face) {
        FT_Done_Face(face);
    }
}

void Font::openFromFile(const std::filesystem::path& filename, unsigned int character_size) {
    loadFont(filename, character_size);
}

int Font::getCharacterSize() const {
    return character_size;
}

int Font::getBaselineY() const {
    if (!face) {
        return 0;
    }
    return face->ascender / 64;
}

Character& Font::getCharacter(unsigned char c) {
    return characters[c];
}

void Font::loadFont(const std::filesystem::path& filename, unsigned int character_size) {
    this->character_size = character_size;
    if (!is_library_initialized) {
        FREETYPE_CALL(FT_Init_FreeType(&library), []() { return "Failed to initialize FreeType library"; });
        is_library_initialized = true;
    }
    if (face) {
        FREETYPE_CALL(FT_Done_Face(face), []() { return "Failed to unload font"; });
    }
    characters.clear();

    std::string filename_str = filename.string();
    const char* filename_cstr = filename_str.c_str();
    FREETYPE_CALL(FT_New_Face(library, filename_cstr, 0, &face), []() { return "Failed to load font file"; });
    FREETYPE_CALL(FT_Set_Pixel_Sizes(face, 0, character_size), []() { return "Failed to set font size"; });
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(face, c, FT_LOAD_RENDER),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        unsigned char* buffer = face->glyph->bitmap.buffer;
        unsigned int width = face->glyph->bitmap.width;
        unsigned int height = face->glyph->bitmap.rows;
        FT_Pos advance = face->glyph->advance.x;
        Character& ch = characters[c];
        ch.x = face->glyph->bitmap_left;
        ch.height = face->glyph->bitmap_top;
        ch.advance = advance / 64;
        if (buffer && width > 0 && height > 0) {
            ch.texture.create(width, height, buffer, 1, true);
        }
    }
}

}
