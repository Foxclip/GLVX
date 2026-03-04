#include "glvis/text.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/vertex.h"
#include "glvis/vertex_buffer.h"
#include "glvis/render_states.h"
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
}

const std::string& Text::getString() const {
    return string;
}

void Text::setString(const std::string& string) {
    this->string = string;

    Vector2f text_size = calculateSize();
    setSize(text_size);

    // Blit character textures onto text_texture
    int width = static_cast<int>(std::ceil(text_size.x));
    int height = static_cast<int>(std::ceil(text_size.y));

    if (width <= 0 || height <= 0) {
        return;
    }

    text_texture.create(width, height);

    // Save current framebuffer
    GLint currentFBO;
    GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO));

    // Bind our framebuffer
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, text_texture.getFBO()));
    GL_CALL(glViewport(0, 0, width, height));

    // Clear the texture with transparent color
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    // Enable blending for transparency
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // Create shader for rendering
    Shader simpleShader(shaders::simple_vert, shaders::simple_frag);
    simpleShader.use();

    // Render each character
    float current_x = 0.0f;
    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);

        if (ch.texture.getWidth() == 0 || ch.texture.getHeight() == 0) {
            // Skip characters with no bitmap
            current_x += static_cast<float>(ch.advance);
            continue;
        }

        float char_width = static_cast<float>(ch.texture.getWidth());
        float char_height = static_cast<float>(ch.texture.getHeight());

        // Position: X = current_x + bitmap_left, Y = bitmap_top (from top of text area)
        float pos_x = current_x + static_cast<float>(ch.x);
        float pos_y = static_cast<float>(ch.y);

        // Create a quad for this character
        // Note: Y is flipped because OpenGL texture coordinates have (0,0) at bottom-left
        // but we want to render with Y going up from the top
        std::vector<Vertex> vertices = {
            Vertex(Vector2f(pos_x, pos_y), Color::White, Vector2f(0.0f, 1.0f)),
            Vertex(Vector2f(pos_x, pos_y - char_height), Color::White, Vector2f(0.0f, 0.0f)),
            Vertex(Vector2f(pos_x + char_width, pos_y), Color::White, Vector2f(1.0f, 1.0f)),
            Vertex(Vector2f(pos_x + char_width, pos_y), Color::White, Vector2f(1.0f, 1.0f)),
            Vertex(Vector2f(pos_x, pos_y - char_height), Color::White, Vector2f(0.0f, 0.0f)),
            Vertex(Vector2f(pos_x + char_width, pos_y - char_height), Color::White, Vector2f(1.0f, 0.0f))
        };

        // Create temporary vertex buffer for this character
        VertexBuffer vb(PrimitiveType::Triangles, Usage::DynamicDraw);
        vb.create(6);
        vb.update(vertices);

        // Set up shader uniforms
        simpleShader.setMat4("model", Matrix4());
        simpleShader.setMat4("view", Matrix4());
        simpleShader.setMat4("projection", Matrix4());
        simpleShader.setVec4("color", Vector4(255, 255, 255, 255));
        simpleShader.setBool("hasTexture", true);

        // Bind texture and render
        ch.texture.bind();
        vb.render();

        current_x += static_cast<float>(ch.advance);
    }

    // Restore previous framebuffer
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, currentFBO));
}

Vector2f Text::calculateSize() const {
    float text_width = 0.0f;
    float text_height = 0.0f;
    float current_x = 0.0f;

    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);

        float char_left = static_cast<float>(ch.x);
        float char_width = static_cast<float>(ch.texture.getWidth());
        float char_right = char_left + char_width;

        text_width = std::max(text_width, current_x + static_cast<float>(ch.advance));

        float char_top = static_cast<float>(ch.y);
        float char_bottom = char_top - static_cast<float>(ch.texture.getHeight());

        text_height = std::max(text_height, char_top - char_bottom);

        current_x += static_cast<float>(ch.advance);
    }

    return Vector2f(text_width, text_height);
}

}
