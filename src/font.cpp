#include "glvis/font.h"
#include "glvis/glvis_common.h"
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

struct GlyphBitmap {
    unsigned char* data;
    int width;
    int height;
};

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
    if (face) {
        FREETYPE_CALL(FT_Done_Face(face), []() { return "Failed to unload font"; });
    }
    characters.clear();

    std::string filename_str = filename.string();
    const char* filename_cstr = filename_str.c_str();
    FREETYPE_CALL(FT_New_Face(library, filename_cstr, 0, &face), []() { return "Failed to load font file"; });
    FREETYPE_CALL(FT_Set_Pixel_Sizes(face, 0, character_size), []() { return "Failed to set font size"; });

    std::map<unsigned char, GlyphBitmap> glyphBitmaps;
    unsigned int maxGlyphWidth = 0;
    unsigned int maxGlyphHeight = 0;

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
            unsigned char* copied = new unsigned char[width * height];
            std::memcpy(copied, face->glyph->bitmap.buffer, width * height);
            glyphBitmaps[c] = { copied, static_cast<int>(width), static_cast<int>(height) };
            if (width > maxGlyphWidth) maxGlyphWidth = static_cast<int>(width);
            if (height > maxGlyphHeight) maxGlyphHeight = static_cast<int>(height);
        }
    }

    int atlasWidth = 0;
    int atlasHeight = 0;
    {
        int totalArea = 0;
        for (auto& pair : glyphBitmaps) {
            totalArea += pair.second.width * pair.second.height;
        }
        int side = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(totalArea))));
        int pow2 = 1;
        while (pow2 < side) pow2 *= 2;
        if (pow2 == 0) pow2 = 1;
        atlasWidth = pow2;
        atlasHeight = pow2;
    }

    std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);
    std::map<unsigned char, std::pair<int, int>> glyphPositions;

    int currentX = 0;
    int currentY = 0;
    int rowHeight = 0;

    for (auto& pair : glyphBitmaps) {
        unsigned char c = pair.first;
        const GlyphBitmap& gb = pair.second;

        if (currentX + gb.width > atlasWidth) {
            currentX = 0;
            currentY += rowHeight + 1;
            rowHeight = 0;
        }

        glyphPositions[c] = { currentX, currentY };

        for (int y = 0; y < gb.height; y++) {
            for (int x = 0; x < gb.width; x++) {
                int srcIdx = y * gb.width + x;
                int dstY = currentY + y;
                int dstX = currentX + x;
                if (dstY < atlasHeight && dstX < atlasWidth) {
                    atlasData[dstY * atlasWidth + dstX] = gb.data[srcIdx];
                }
            }
        }

        if (gb.height > rowHeight) rowHeight = gb.height;
        currentX += gb.width + 1;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    atlas.create(atlasWidth, atlasHeight, atlasData.data(), 1, true);

    for (auto& pair : glyphBitmaps) {
        unsigned char c = pair.first;
        const GlyphBitmap& gb = pair.second;
        const std::pair<int, int>& pos = glyphPositions[c];
        Character& ch = characters[c];

        float px = static_cast<float>(pos.first);
        float py = static_cast<float>(pos.second);
        float pxw = static_cast<float>(pos.first + gb.width);
        float pyh = static_cast<float>(pos.second + gb.height);
        float invW = 1.0f / static_cast<float>(atlasWidth);
        float invH = 1.0f / static_cast<float>(atlasHeight);

        ch.uv_top_left = Vector2f(px * invW, pyh * invH);
        ch.uv_bottom_right = Vector2f(pxw * invW, py * invH);
    }

    for (auto& pair : glyphBitmaps) {
        delete[] pair.second.data;
    }
}

}
