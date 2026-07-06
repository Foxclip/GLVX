#include "glvis/text.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/shaders/subpixel.h"
#include "glvis/vertex.h"
#include "glvis/vertex_buffer.h"
#include "glvis/render_states.h"
#include "glvis/glvis_common.h"
#include <glad/glad.h>
#include <vector>
#include <cassert>
#include <cmath>

namespace glvis {

Text::Text(Font* font, const std::string& string) : Rectangle(0.0f, 0.0f) {
    setFont(font);
    setString(string);
}

Font* Text::getFont() const {
    return font;
}

void Text::setFont(Font* font) {
    this->font = font;
    vertex_buffer = VertexBuffer();
}

const std::string& Text::getString() const {
    return string;
}

float Text::getMaxWidth() const {
    return max_width;
}

void Text::setMaxWidth(float max_width) {
    this->max_width = max_width;
    setString(string);
}

void Text::setString(const std::string& string) {
    this->string = string;

    text_bounds = calculateVisualBounds();
    Vector2f text_size = text_bounds.size;
    setSize(text_size);

    if (string.empty()) {
        vertex_buffer = VertexBuffer();
        return;
    }

    std::vector<std::string> lines = breakLines();

    std::vector<Vertex> vertices;
    float line_height = static_cast<float>(font->getLineHeight());

    for (size_t line_idx = 0; line_idx < lines.size(); line_idx++) {
        const std::string& line = lines[line_idx];
        float current_x = 0.0f;
        float current_y = static_cast<float>(line_idx) * line_height;

        for (size_t i = 0; i < line.size(); i++) {
            const Character& ch = font->getCharacter(line[i]);
            if (ch.width <= 0) {
                current_x += static_cast<float>(ch.advance);
                if (i + 1 < line.size()) {
                    current_x += static_cast<float>(font->getKerning(line[i], line[i + 1]));
                }
                continue;
            }

            float char_x = current_x + static_cast<float>(ch.x);
            float part_below_baseline = static_cast<float>(ch.glyph_height - ch.top);
            float char_y = static_cast<float>(font->getCharacterSize() + part_below_baseline) + current_y;
            float char_w = static_cast<float>(ch.width);
            float char_h = static_cast<float>(ch.glyph_height);

            float uvTopL = ch.uv_top_left.x;
            float uvTopR = ch.uv_bottom_right.x;
            float uvBotL = ch.uv_bottom_right.y;
            float uvBotR = ch.uv_top_left.y;

            vertices.push_back(Vertex(Vector2f(char_x, char_y), Color::White, Vector2f(uvTopL, uvBotR)));
            vertices.push_back(Vertex(Vector2f(char_x, char_y - char_h), Color::White, Vector2f(uvTopL, uvBotL)));
            vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y), Color::White, Vector2f(uvTopR, uvBotR)));
            vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y), Color::White, Vector2f(uvTopR, uvBotR)));
            vertices.push_back(Vertex(Vector2f(char_x, char_y - char_h), Color::White, Vector2f(uvTopL, uvBotL)));
            vertices.push_back(Vertex(Vector2f(char_x + char_w, char_y - char_h), Color::White, Vector2f(uvTopR, uvBotL)));

            current_x += static_cast<float>(ch.advance);
            if (i + 1 < line.size()) {
                current_x += static_cast<float>(font->getKerning(line[i], line[i + 1]));
            }
        }
    }

    if (!vertex_buffer.getVAO()) {
        vertex_buffer.create(static_cast<std::size_t>(vertices.size()));
    }
    vertex_buffer.update(vertices);
}

void Text::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    renderBase(getShader(), &font->getAtlas(), color, getModelMatrix(), view, projection, states);
}

const VertexBuffer& Text::getVertexBuffer() const {
    return vertex_buffer;
}

Shader* Text::getDefaultShader() const {
    return font->isSubpixel() ? common::subpixelShader : common::defaultShader;
}

std::vector<std::string> Text::breakLines() const {
    assert(font);

    std::vector<std::string> result;

    size_t start = 0;
    while (start < string.size()) {
        size_t end = string.find('\n', start);
        size_t paragraph_end = (end == std::string::npos) ? string.size() : end;

        std::string paragraph = string.substr(start, paragraph_end - start);

        if (max_width <= 0.0f) {
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

                    if (measureWidth(candidate) <= max_width) {
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

        start = (end == std::string::npos) ? string.size() : end + 1;
    }

    return result;
}

float Text::measureWidth(const std::string& text) const {
    assert(font);
    if (text.empty()) {
        return 0.0f;
    }

    float width = 0.0f;

    for (size_t i = 0; i < text.size(); i++) {
        const Character& ch = font->getCharacter(text[i]);
        width += static_cast<float>(ch.advance);
        if (i + 1 < text.size()) {
            width += static_cast<float>(font->getKerning(text[i], text[i + 1]));
        }
    }

    return width;
}

Rect Text::calculateVisualBounds() const {
    std::vector<std::string> lines = breakLines();

    if (lines.empty()) {
        return Rect();
    }

    Rect result;
    float line_height = static_cast<float>(font->getLineHeight());
    bool first = true;

    for (size_t line_idx = 0; line_idx < lines.size(); line_idx++) {
        const std::string& line = lines[line_idx];
        float current_x = 0.0f;
        float line_y_offset = static_cast<float>(line_idx) * line_height;

        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            const Character& ch = font->getCharacter(c);
            float effective_char_width = (ch.width > 0) ? static_cast<float>(ch.width) : static_cast<float>(ch.advance);

            Rect char_rect;
            int font_size = font->getCharacterSize();
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
                current_x += static_cast<float>(font->getKerning(line[i], line[i + 1]));
            }
        }
    }

    return result;
}

}
