#include "glvx/font.h"
#include "glvx/glvx_common.h"
#include "glvx/utils.h"
#include <vector>
#include <cmath>
#include <glad/glad.h>

namespace glvx {

bool Font::m_is_library_initialized = false;
FT_Library Font::m_library = nullptr;

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

Font::Font(const std::filesystem::path& filename, unsigned int character_size, bool useSubpixel) {
    loadFont(filename, character_size, useSubpixel);
}

Font::~Font() {
    if (m_face) {
        FT_Done_Face(m_face);
    }
}

void Font::openFromFile(const std::filesystem::path& filename, unsigned int character_size, bool useSubpixel) {
    loadFont(filename, character_size, useSubpixel);
}

int Font::getCharacterSize() const {
    return m_character_size;
}

int Font::getBaselineY() const {
    if (!m_face) {
        return 0;
    }
    return m_face->ascender / 64;
}

int Font::getLineHeight() const {
    if (!m_face) {
        return 0;
    }
    return m_face->size->metrics.height / 64;
}

bool Font::isSubpixel() const {
    return m_use_subpixel;
}

Character& Font::getCharacter(unsigned char c) {
    return m_characters[c];
}

const Texture& Font::getAtlas() const {
    return m_atlas;
}

int Font::getKerning(unsigned char left, unsigned char right) const {
    auto it = m_kerning.find({left, right});
    if (it != m_kerning.end()) {
        return it->second;
    }
    return 0;
}

void Font::loadFont(const std::filesystem::path& filename, unsigned int character_size, bool useSubpixel) {
    m_character_size = character_size;
    m_use_subpixel = useSubpixel;
    if (!m_is_library_initialized) {
        FREETYPE_CALL(FT_Init_FreeType(&m_library), []() { return "Failed to initialize FreeType library"; });
        m_is_library_initialized = true;
    }
    FT_Done_Face(m_face);
    m_face = nullptr;
    m_characters.clear();
    m_kerning.clear();

    FREETYPE_CALL(FT_New_Face(m_library, filename.string().c_str(), 0, &m_face), []() { return "Failed to load font file"; });
    FREETYPE_CALL(FT_Set_Pixel_Sizes(m_face, 0, character_size), []() { return "Failed to set font size"; });

    // Load kerning data
    if (FT_HAS_KERNING(m_face)) {
        FT_Vector kernVec;
        for (unsigned char left = 32; left < 126; left++) {
            for (unsigned char right = 33; right < 127; right++) {
                FT_UInt leftGlyph = FT_Get_Char_Index(m_face, left);
                FT_UInt rightGlyph = FT_Get_Char_Index(m_face, right);
                if (leftGlyph && rightGlyph) {
                    FREETYPE_CALL(
                        FT_Get_Kerning(m_face, leftGlyph, rightGlyph, FT_KERNING_DEFAULT, &kernVec),
                        [&]() {
                            return "Failed to get kerning for characters: " + std::to_string(left) + ", " + std::to_string(right);
                        }
                    );
                    int kerningValue = kernVec.x / 64;
                    if (kerningValue != 0) {
                        m_kerning[{left, right}] = kerningValue;
                    }
                }
            }
        }
    }

    unsigned int loadFlag = useSubpixel ? (FT_LOAD_TARGET_LCD | FT_LOAD_RENDER) : FT_LOAD_RENDER;

    // Pass 1: measure glyphs and compute total area
    int totalArea = 0;
    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(m_face, c, loadFlag),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        unsigned int width = m_face->glyph->bitmap.width;
        unsigned int height = m_face->glyph->bitmap.rows;
        FT_Pos advance = m_face->glyph->advance.x;
        Character& ch = m_characters[c];
        ch.x = m_face->glyph->bitmap_left;
        ch.top = m_face->glyph->bitmap_top;
        ch.advance = advance / 64;
        ch.width = static_cast<int>(useSubpixel ? width / 3 : width);
        ch.glyph_height = static_cast<int>(height);

        if (width > 0 && height > 0 && m_face->glyph->bitmap.buffer) {
            totalArea += static_cast<int>(useSubpixel ? width / 3 : width) * height;
        }
    }

    // Compute atlas dimensions
    int side = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(totalArea))));
    int pow2 = 1;
    while (pow2 < side) pow2 *= 2;
    if (pow2 == 0) pow2 = 1;
    int atlasWidth = pow2;
    int atlasHeight = pow2;

    std::vector<unsigned char> atlasData(useSubpixel ? atlasWidth * atlasHeight * 3 : atlasWidth * atlasHeight, 0);

    // Pass 2: render glyphs directly into atlas and compute UVs
    int currentX = 0;
    int currentY = 0;
    int rowHeight = 0;
    float invW = 1.0f / static_cast<float>(atlasWidth);
    float invH = 1.0f / static_cast<float>(atlasHeight);

    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(m_face, c, loadFlag),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        int width = static_cast<int>(m_face->glyph->bitmap.width);
        int height = static_cast<int>(m_face->glyph->bitmap.rows);
        int atlasPixelWidth = useSubpixel ? width / 3 : width;

        if (currentX + atlasPixelWidth > atlasWidth) {
            currentX = 0;
            currentY += rowHeight + 1;
            rowHeight = 0;
        }

        if (width > 0 && height > 0 && m_face->glyph->bitmap.buffer) {
            if (useSubpixel) {
                blit_bitmap_subpixel(
                    m_face->glyph->bitmap.buffer, m_face->glyph->bitmap.pitch,
                    atlasData.data(), atlasWidth * 3,
                    currentX, currentY, width, height
                );
            } else {
                blit_bitmap(
                    m_face->glyph->bitmap.buffer, m_face->glyph->bitmap.pitch,
                    atlasData.data(), atlasWidth,
                    currentX, currentY, width, height
                );
            }
        }

        if (height > rowHeight) rowHeight = height;

        Character& ch = m_characters[c];
        ch.uv_top_left = Vector2f(static_cast<float>(currentX) * invW, static_cast<float>(currentY + height) * invH);
        ch.uv_bottom_right = Vector2f(static_cast<float>(currentX + atlasPixelWidth) * invW, static_cast<float>(currentY) * invH);

        currentX += atlasPixelWidth + 1;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    m_atlas.create(atlasWidth, atlasHeight, atlasData.data(), useSubpixel ? 3 : 1, useSubpixel ? false : true);

    if (useSubpixel) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_atlas.getID()));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    }
}

}
