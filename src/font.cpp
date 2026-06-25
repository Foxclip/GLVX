#include "glvis/font.h"
#include "glvis/glvis_common.h"
#include "glvis/utils.h"
#include <vector>
#include <cmath>
#include <glad/glad.h>

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

const Texture& Font::getAtlas() const {
    return atlas;
}

void Font::loadFont(const std::filesystem::path& filename, unsigned int character_size) {
    this->character_size = character_size;
    if (!is_library_initialized) {
        FREETYPE_CALL(FT_Init_FreeType(&library), []() { return "Failed to initialize FreeType library"; });
        is_library_initialized = true;
    }
    FT_Done_Face(face);
    face = nullptr;
    characters.clear();

    FREETYPE_CALL(FT_New_Face(library, filename.string().c_str(), 0, &face), []() { return "Failed to load font file"; });
    FREETYPE_CALL(FT_Set_Pixel_Sizes(face, 0, character_size), []() { return "Failed to set font size"; });

    // Pass 1: measure glyphs and compute total area
    int totalArea = 0;
    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(face, c, FT_LOAD_RENDER),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        unsigned int width = face->glyph->bitmap.width;
        unsigned int height = face->glyph->bitmap.rows;
        FT_Pos advance = face->glyph->advance.x;
        Character& ch = characters[c];
        ch.x = face->glyph->bitmap_left;
        ch.height = face->glyph->bitmap_top;
        ch.advance = advance / 64;
        ch.width = static_cast<int>(width);
        ch.glyph_height = static_cast<int>(height);

        if (width > 0 && height > 0 && face->glyph->bitmap.buffer) {
            totalArea += width * height;
        }
    }

    // Compute atlas dimensions
    int side = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(totalArea))));
    int pow2 = 1;
    while (pow2 < side) pow2 *= 2;
    if (pow2 == 0) pow2 = 1;
    int atlasWidth = pow2;
    int atlasHeight = pow2;

    std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);

    // Pass 2: render glyphs directly into atlas and compute UVs
    int currentX = 0;
    int currentY = 0;
    int rowHeight = 0;
    float invW = 1.0f / static_cast<float>(atlasWidth);
    float invH = 1.0f / static_cast<float>(atlasHeight);

    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(face, c, FT_LOAD_RENDER),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        int width = static_cast<int>(face->glyph->bitmap.width);
        int height = static_cast<int>(face->glyph->bitmap.rows);

        if (currentX + width > atlasWidth) {
            currentX = 0;
            currentY += rowHeight + 1;
            rowHeight = 0;
        }

        if (width > 0 && height > 0 && face->glyph->bitmap.buffer) {
            blit_bitmap(
                face->glyph->bitmap.buffer, face->glyph->bitmap.pitch,
                atlasData.data(), atlasWidth,
                currentX, currentY, width, height
            );
        }

        if (height > rowHeight) rowHeight = height;

        Character& ch = characters[c];
        ch.uv_top_left = Vector2f(static_cast<float>(currentX) * invW, static_cast<float>(currentY + height) * invH);
        ch.uv_bottom_right = Vector2f(static_cast<float>(currentX + width) * invW, static_cast<float>(currentY) * invH);

        currentX += width + 1;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    atlas.create(atlasWidth, atlasHeight, atlasData.data(), 1, true);
}

}
