#include "glvis/text.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
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

void Text::setString(const std::string& string) {
    this->string = string;

    text_bounds = calculateVisualBounds();
    Vector2f text_size = text_bounds.size;
    setSize(text_size);

    if (string.empty()) {
        vertex_buffer = VertexBuffer();
        return;
    }

    std::vector<Vertex> vertices;
    float current_x = 0.0f;

    for (size_t i = 0; i < string.size(); i++) {
        const Character& ch = font->getCharacter(string[i]);
        if (ch.width <= 0) {
            current_x += static_cast<float>(ch.advance);
            if (i + 1 < string.size()) {
                current_x += static_cast<float>(font->getKerning(string[i], string[i + 1]));
            }
            continue;
        }

        float char_x = current_x + static_cast<float>(ch.x);
        float char_y = static_cast<float>(font->getCharacterSize());
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
        if (i + 1 < string.size()) {
            current_x += static_cast<float>(font->getKerning(string[i], string[i + 1]));
        }
    }

    if (!vertex_buffer.getVAO()) {
        vertex_buffer.create(static_cast<std::size_t>(vertices.size()));
    }
    vertex_buffer.update(vertices);
}

void Text::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    if (string.empty()) return;

    Shader* renderShader = states.shader ? states.shader : (getShader() ? getShader() : common::defaultShader);
    renderShader->use();

    Matrix4 model = Transformable::getModelMatrix();
    Matrix4 combined = states.transform * model;

    renderShader->setVec4("color", Vector4(color.r, color.g, color.b, color.a));
    renderShader->setMat4("model", combined);
    renderShader->setMat4("view", view);
    renderShader->setMat4("projection", projection);
    renderShader->setInt("tex", 0);
    renderShader->setBool("hasTexture", true);

    font->getAtlas().bind();
    vertex_buffer.render();
}

Rect Text::calculateVisualBounds() const {
    Rect result;
    float current_x = 0.0f;

    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);
        float effective_char_width = (ch.width > 0) ? static_cast<float>(ch.width) : static_cast<float>(ch.advance);

        Rect char_rect;
        int font_size = font->getCharacterSize();
        char_rect.position.x = current_x + static_cast<float>(ch.x);
        char_rect.position.y = static_cast<float>(font_size - ch.height);
        char_rect.size.x = effective_char_width;
        char_rect.size.y = static_cast<float>(ch.glyph_height);

        if (i == 0) {
            result = char_rect;
        } else {
            result.extend(char_rect);
        }

        current_x += static_cast<float>(ch.advance);
        if (i + 1 < string.size()) {
            current_x += static_cast<float>(font->getKerning(string[i], string[i + 1]));
        }
    }

    return result;
}

}
