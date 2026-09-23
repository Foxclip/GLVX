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

Font::Font(const std::filesystem::path& filename, bool use_subpixel) {
    openFromFile(filename, use_subpixel);
}

Font::~Font() {
    if (m_face) {
        FT_Done_Face(m_face);
    }
}

void Font::openFromFile(const std::filesystem::path& filename, bool use_subpixel) {
    m_use_subpixel = use_subpixel;
    if (!m_is_library_initialized) {
        FREETYPE_CALL(FT_Init_FreeType(&m_library), []() { return "Failed to initialize FreeType library"; });
        m_is_library_initialized = true;
    }
    FT_Done_Face(m_face);
    m_face = nullptr;
    m_sizes.clear();

    FREETYPE_CALL(FT_New_Face(m_library, filename.string().c_str(), 0, &m_face), []() { return "Failed to load font file"; });
}

bool Font::isSubpixel() const {
    return m_use_subpixel;
}

Character& Font::getCharacter(unsigned int character_size, unsigned char c) {
    return loadPage(character_size).m_characters[c];
}

const Texture& Font::getAtlas(unsigned int character_size) {
    return loadPage(character_size).m_atlas;
}

int Font::getKerning(unsigned int character_size, unsigned char left, unsigned char right) {
    SizePage& page = loadMetadata(character_size);
    auto it = page.m_kerning.find({left, right});
    if (it != page.m_kerning.end()) {
        return it->second;
    }
    return 0;
}

int Font::getLineHeight(unsigned int character_size) {
    return loadMetadata(character_size).m_line_height;
}

int Font::getBaselineY(unsigned int character_size) {
    return loadMetadata(character_size).m_ascender;
}

Font::SizePage& Font::loadPage(unsigned int character_size) {
    SizePage& page = loadMetadata(character_size);
    if (page.m_rasterized) {
        return page;
    }

    try {
        rasterizePage(page);
    } catch (...) {
        m_sizes.erase(character_size);
        throw;
    }
    return page;
}

Font::SizePage& Font::loadMetadata(unsigned int character_size) {
    auto it = m_sizes.find(character_size);
    if (it != m_sizes.end()) {
        return it->second;
    }

    SizePage& page = m_sizes.try_emplace(character_size).first->second;
    try {
        FREETYPE_CALL(FT_Set_Pixel_Sizes(m_face, 0, character_size), []() { return "Failed to set font size"; });

        // Load kerning data
        if (FT_HAS_KERNING(m_face)) {
            FT_Vector kern_vec;
            for (unsigned char left = 32; left < 126; left++) {
                for (unsigned char right = 33; right < 127; right++) {
                    FT_UInt left_glyph = FT_Get_Char_Index(m_face, left);
                    FT_UInt right_glyph = FT_Get_Char_Index(m_face, right);
                    if (left_glyph && right_glyph) {
                        FREETYPE_CALL(
                            FT_Get_Kerning(m_face, left_glyph, right_glyph, FT_KERNING_DEFAULT, &kern_vec),
                            [&]() {
                                return "Failed to get kerning for characters: " + std::to_string(left) + ", " + std::to_string(right);
                            }
                        );
                        int kerning_value = kern_vec.x / 64;
                        if (kerning_value != 0) {
                            page.m_kerning[{left, right}] = kerning_value;
                        }
                    }
                }
            }
        }

        page.m_line_height = m_face->size->metrics.height / 64;
        page.m_ascender = m_face->ascender / 64;
    } catch (...) {
        m_sizes.erase(character_size);
        throw;
    }
    return page;
}

void Font::rasterizePage(SizePage& page) {
    unsigned int load_flag = m_use_subpixel ? (FT_LOAD_TARGET_LCD | FT_LOAD_RENDER) : FT_LOAD_RENDER;

    struct RasterizedGlyph {
        std::vector<unsigned char> m_data;
        int m_width = 0;
        int m_height = 0;
        int m_pitch = 0;
    };
    std::vector<RasterizedGlyph> bitmaps(95);

    // Pass 1: rasterize each glyph once, measure it and cache its bitmap
    int total_area = 0;
    for (unsigned char c = 32; c < 127; c++) {
        FREETYPE_CALL(
            FT_Load_Char(m_face, c, load_flag),
            [&]() {
                return "Failed to load character: " + std::to_string(c) + " (" + std::string(1, c) + ")";
            }
        );
        unsigned int width = m_face->glyph->bitmap.width;
        unsigned int height = m_face->glyph->bitmap.rows;
        FT_Pos advance = m_face->glyph->advance.x;
        Character& ch = page.m_characters[c];
        ch.x = m_face->glyph->bitmap_left;
        ch.top = m_face->glyph->bitmap_top;
        ch.advance = advance / 64;
        ch.width = static_cast<int>(m_use_subpixel ? width / 3 : width);
        ch.glyph_height = static_cast<int>(height);

        if (width > 0 && height > 0 && m_face->glyph->bitmap.buffer) {
            total_area += static_cast<int>(m_use_subpixel ? width / 3 : width) * static_cast<int>(height);
            RasterizedGlyph& bmp = bitmaps[c - 32];
            bmp.m_width = static_cast<int>(width);
            bmp.m_height = static_cast<int>(height);
            bmp.m_pitch = m_face->glyph->bitmap.pitch;
            bmp.m_data.assign(
                m_face->glyph->bitmap.buffer,
                m_face->glyph->bitmap.buffer + static_cast<size_t>(bmp.m_pitch) * height
            );
        }
    }

    // Compute atlas dimensions
    int side = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(total_area))));
    int pow2 = 1;
    while (pow2 < side) {
        pow2 *= 2;
    }
    if (pow2 == 0) {
        pow2 = 1;
    }
    int atlas_width = pow2;
    int atlas_height = pow2;

    std::vector<unsigned char> atlas_data(m_use_subpixel ? atlas_width * atlas_height * 3 : atlas_width * atlas_height, 0);

    // Pass 2: place the cached bitmaps into the atlas and compute UVs
    int current_x = 0;
    int current_y = 0;
    int row_height = 0;
    float inv_w = 1.0f / static_cast<float>(atlas_width);
    float inv_h = 1.0f / static_cast<float>(atlas_height);

    for (unsigned char c = 32; c < 127; c++) {
        const RasterizedGlyph& bmp = bitmaps[c - 32];
        int width = bmp.m_width;
        int height = bmp.m_height;
        int atlas_pixel_width = m_use_subpixel ? width / 3 : width;

        if (current_x + atlas_pixel_width > atlas_width) {
            current_x = 0;
            current_y += row_height + 1;
            row_height = 0;
        }

        if (width > 0 && height > 0) {
            if (m_use_subpixel) {
                blit_bitmap_subpixel(
                    bmp.m_data.data(), bmp.m_pitch,
                    atlas_data.data(), atlas_width * 3,
                    current_x, current_y, width, height
                );
            } else {
                blit_bitmap(
                    bmp.m_data.data(), bmp.m_pitch,
                    atlas_data.data(), atlas_width,
                    current_x, current_y, width, height
                );
            }
        }

        if (height > row_height) {
            row_height = height;
        }

        Character& ch = page.m_characters[c];
        ch.uv_top_left = Vector2f(static_cast<float>(current_x) * inv_w, static_cast<float>(current_y + height) * inv_h);
        ch.uv_bottom_right = Vector2f(static_cast<float>(current_x + atlas_pixel_width) * inv_w, static_cast<float>(current_y) * inv_h);

        current_x += atlas_pixel_width + 1;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    page.m_atlas.create(atlas_width, atlas_height, atlas_data.data(), m_use_subpixel ? 3 : 1, m_use_subpixel ? false : true);

    if (m_use_subpixel) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, page.m_atlas.getID()));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    }
    page.m_rasterized = true;
}

}
