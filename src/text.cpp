#include "glvx/text.h"
#include "glvx/shader.h"
#include "glvx/shaders/simple.h"
#include "glvx/shaders/subpixel.h"
#include "glvx/vertex.h"
#include "glvx/vertex_buffer.h"
#include "glvx/render_states.h"
#include "glvx/glvx_common.h"
#include <glad/glad.h>
#include <vector>
#include <cassert>
#include <cmath>

namespace glvx {

Text::Text(Font* font, const std::string& string) : Shape(PrimitiveType::Triangles, 0) {
    setFont(font);
    setString(string);
}

Font* Text::getFont() const {
    return m_font;
}

void Text::setFont(Font* font) {
    m_font = font;
    if (font) {
        setTexture(const_cast<Texture*>(&font->getAtlas()));
        if (font->isSubpixel()) {
            setShader(common::subpixelShader);
        }
    } else {
        setTexture(nullptr);
        setShader(nullptr);
    }
}

const std::string& Text::getString() const {
    return m_string;
}

float Text::getMaxWidth() const {
    return m_max_width;
}

float Text::getWidth() const {
    return m_width;
}

float Text::getHeight() const {
    return m_height;
}

Vector2f Text::getSize() const {
    return Vector2f(m_width, m_height);
}

void Text::setMaxWidth(float max_width) {
    m_max_width = max_width;
    setString(m_string);
}

void Text::setString(const std::string& string) {
    m_string = string;

    if (string.empty()) {
        m_vertices.clear();
        m_vertex_buffer.create(0);
        m_width = 0.0f;
        m_height = 0.0f;
        return;
    }

    Vector2f text_size = calculateVisualBounds().size;

    std::vector<std::string> lines = breakLines();

    m_vertices.clear();
    float line_height = static_cast<float>(m_font->getLineHeight());

    for (size_t line_idx = 0; line_idx < lines.size(); line_idx++) {
        const std::string& line = lines[line_idx];
        float current_x = 0.0f;
        float current_y = static_cast<float>(line_idx) * line_height;

        for (size_t i = 0; i < line.size(); i++) {
            const Character& ch = m_font->getCharacter(line[i]);
            if (ch.width <= 0) {
                current_x += static_cast<float>(ch.advance);
                if (i + 1 < line.size()) {
                current_x += static_cast<float>(m_font->getKerning(line[i], line[i + 1]));
                }
                continue;
            }

            float char_x = current_x + static_cast<float>(ch.x);
            float part_below_baseline = static_cast<float>(ch.glyph_height - ch.top);
            float char_y = static_cast<float>(m_font->getCharacterSize() + part_below_baseline) + current_y;
            float char_w = static_cast<float>(ch.width);
            float char_h = static_cast<float>(ch.glyph_height);

            float uvTopL = ch.uv_top_left.x;
            float uvTopR = ch.uv_bottom_right.x;
            float uvBotL = ch.uv_bottom_right.y;
            float uvBotR = ch.uv_top_left.y;

            m_vertices.push_back(Vertex(Vector2f(char_x, char_y), Color::White, Vector2f(uvTopL, uvBotR)));
            m_vertices.push_back(Vertex(Vector2f(char_x, char_y - char_h), Color::White, Vector2f(uvTopL, uvBotL)));
            m_vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y), Color::White, Vector2f(uvTopR, uvBotR)));
            m_vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y), Color::White, Vector2f(uvTopR, uvBotR)));
            m_vertices.push_back(Vertex(Vector2f(char_x, char_y - char_h), Color::White, Vector2f(uvTopL, uvBotL)));
            m_vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y - char_h), Color::White, Vector2f(uvTopR, uvBotL)));

            current_x += static_cast<float>(ch.advance);
            if (i + 1 < line.size()) {
                current_x += static_cast<float>(m_font->getKerning(line[i], line[i + 1]));
            }
        }
    }

    m_width = text_size.x;
    m_height = text_size.y;
    m_vertex_buffer.update(m_vertices);
}

void Text::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    renderBase(m_shader, m_texture, m_color, getTransform(), view, projection, states);
}

std::vector<std::string> Text::breakLines() const {
    assert(m_font);

    std::vector<std::string> result;

    size_t start = 0;
    while (start < m_string.size()) {
        size_t end = m_string.find('\n', start);
        size_t paragraph_end = (end == std::string::npos) ? m_string.size() : end;

        std::string paragraph = m_string.substr(start, paragraph_end - start);

        if (m_max_width <= 0.0f) {
            result.push_back(paragraph);
        } else {
            if (paragraph.empty()) {
                result.push_back("");
            } else {
                std::vector<std::string> words;
                size_t word_start = 0;
                while (word_start < paragraph.size()) {
                    size_t space_pos = paragraph.find(' ', word_start);
                    if (space_pos == std::string::npos) {
                        words.push_back(paragraph.substr(word_start));
                        break;
                    } else {
                        words.push_back(paragraph.substr(word_start, space_pos - word_start));
                        word_start = space_pos + 1;
                    }
                }

                std::string current_line;
                for (size_t w = 0; w < words.size(); w++) {
                    std::string candidate;
                    if (current_line.empty()) {
                        candidate = words[w];
                    } else {
                        candidate = current_line + " " + words[w];
                    }

                    if (measureWidth(candidate) <= m_max_width) {
                        current_line = candidate;
                    } else {
                        if (!current_line.empty()) {
                            result.push_back(current_line);
                        }
                        current_line = words[w];
                    }
                }
                if (!current_line.empty()) {
                    result.push_back(current_line);
                }
            }
        }

        start = (end == std::string::npos) ? m_string.size() : end + 1;
    }

    return result;
}

float Text::measureWidth(const std::string& text) const {
    assert(m_font);
    if (text.empty()) {
        return 0.0f;
    }

    float width = 0.0f;

    for (size_t i = 0; i < text.size(); i++) {
        const Character& ch = m_font->getCharacter(text[i]);
        width += static_cast<float>(ch.advance);
        if (i + 1 < text.size()) {
            width += static_cast<float>(m_font->getKerning(text[i], text[i + 1]));
        }
    }

    return width;
}

FloatRect Text::calculateVisualBounds() const {
    std::vector<std::string> lines = breakLines();

    if (lines.empty()) {
        return FloatRect();
    }

    FloatRect result;
    float line_height = static_cast<float>(m_font->getLineHeight());
    bool first = true;

    for (size_t line_idx = 0; line_idx < lines.size(); line_idx++) {
        const std::string& line = lines[line_idx];
        float current_x = 0.0f;
        float line_y_offset = static_cast<float>(line_idx) * line_height;

        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            const Character& ch = m_font->getCharacter(c);
            float effective_char_width = (ch.width > 0) ? static_cast<float>(ch.width) : static_cast<float>(ch.advance);

            FloatRect char_rect;
            int font_size = m_font->getCharacterSize();
            char_rect.position.x = current_x + static_cast<float>(ch.x);
            float part_below_baseline = static_cast<float>(ch.glyph_height - ch.top);
            char_rect.position.y = static_cast<float>(font_size - ch.top + part_below_baseline) + line_y_offset;
            char_rect.size.x = effective_char_width;
            char_rect.size.y = static_cast<float>(ch.glyph_height);

            if (first) {
                result = char_rect;
                first = false;
            } else {
                result.extend(char_rect);
            }

            current_x += static_cast<float>(ch.advance);
            if (i + 1 < line.size()) {
                current_x += static_cast<float>(m_font->getKerning(line[i], line[i + 1]));
            }
        }
    }

    return result;
}

}
