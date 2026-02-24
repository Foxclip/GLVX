#include "glvis/font.h"
#include "glvis/glvis_common.h"

namespace glvis {

bool Font::is_library_initialized = false;
FT_Library Font::library = nullptr;

#define FREETYPE_CALL(func, message) \
    { \
        FT_Error error = func; \
        if (error != 0) { \
            throw std::runtime_error( \
                std::string(message) \
                + ", error code: " \
                + std::to_string(error) \
            ); \
        } \
    }


Font::Font(const std::filesystem::path& filename) {
    loadFont(filename);
}

Font::~Font() {
    if (face) {
        FT_Done_Face(face);
    }
}

void Font::openFromFile(const std::filesystem::path& filename) {
    loadFont(filename);
}

void Font::loadFont(const std::filesystem::path& filename) {
    if (!is_library_initialized) {
        FREETYPE_CALL(FT_Init_FreeType(&library), "Failed to initialize FreeType library");
        is_library_initialized = true;
    }
    if (face) {
        FREETYPE_CALL(FT_Done_Face(face), "Failed to unload font");
    }
    characters.clear();

    std::string filename_str = filename.string();
    const char* filename_cstr = filename_str.c_str();
    FREETYPE_CALL(FT_New_Face(library, filename_cstr, 0, &face), "Failed to load font file");
    FREETYPE_CALL(FT_Set_Pixel_Sizes(face, 0, 48), "Failed to set font size");
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(face, c, FT_LOAD_RENDER),
            "Failed to load character: "
                + std::to_string(c)
                + " ( " + std::string(1, c) + ")"
        );
        unsigned char* buffer = face->glyph->bitmap.buffer;
        unsigned int width = face->glyph->bitmap.width;
        unsigned int height = face->glyph->bitmap.rows;
        int pitch = face->glyph->bitmap.pitch;
        FT_Pos advance = face->glyph->advance.x;
        if (!buffer || width == 0 || height == 0) {
            characters[c] = { Texture(), 0, 0, advance / 64 };
        } else {
            characters[c] = {
                Texture(buffer, width, height, 1),
                face->glyph->bitmap_left,
                face->glyph->bitmap_top,
                advance / 64
            };
        }
    }
}

}
