#include "glvis/text.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/vertex.h"
#include "glvis/vertex_buffer.h"
#include "glvis/render_states.h"
#include "glvis/rect.h"
#include <glad/glad.h>
#include <vector>
#include <cassert>
#include <cmath>

namespace glvis {

Text::Text(Font* font, const std::string& string) : Rectangle(0.0f, 0.0f) {
    setFont(font);
    setString(string);
    setTexture(&text_texture);
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
    float widthf = std::ceil(text_size.x);
    float heightf = std::ceil(text_size.y);
    int width = static_cast<int>(widthf);
    int height = static_cast<int>(heightf);

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

    // Set up shader uniforms - use identity matrices, vertices will be in NDC
    simpleShader.setMat4("model", Matrix4());
    simpleShader.setMat4("view", Matrix4());
    simpleShader.setMat4("projection", Matrix4());
    simpleShader.setVec4("color", Vector4(255, 255, 255, 255));
    simpleShader.setBool("hasTexture", true);
    simpleShader.setInt("tex", 0);  // Explicitly set texture unit

    // Render each character
    float current_x = 0.0f;
    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);

        if (ch.texture.getWidth() <= 0 || ch.texture.getHeight() <= 0) {
            current_x += static_cast<float>(ch.advance);
            continue;
        }

        float char_width = static_cast<float>(ch.texture.getWidth());
        float char_height = static_cast<float>(ch.texture.getHeight());

        // Position relative to baseline
        float x = current_x + static_cast<float>(ch.x);
        float y = static_cast<float>(ch.height);

        // Character bounds in pixel coordinates
        float char_left = x;
        float char_right = x + char_width;
        float char_bottom = y - char_height;
        float char_top = y;

        // Convert pixel coordinates to NDC [-1, 1]
        float left = (char_left / widthf) * 2.0f - 1.0f;
        float right = (char_right / widthf) * 2.0f - 1.0f;
        float bottom = (char_bottom / heightf) * 2.0f - 1.0f;
        float top = (char_top / heightf) * 2.0f - 1.0f;

        std::vector<Vertex> vertices = {
            Vertex(Vector2f(left, top), Color::White, Vector2f(0.0f, 1.0f)),
            Vertex(Vector2f(left, bottom), Color::White, Vector2f(0.0f, 0.0f)),
            Vertex(Vector2f(right, top), Color::White, Vector2f(1.0f, 1.0f)),
            Vertex(Vector2f(right, top), Color::White, Vector2f(1.0f, 1.0f)),
            Vertex(Vector2f(left, bottom), Color::White, Vector2f(0.0f, 0.0f)),
            Vertex(Vector2f(right, bottom), Color::White, Vector2f(1.0f, 0.0f))
        };

        VertexBuffer vb(PrimitiveType::Triangles, Usage::DynamicDraw);
        vb.create(6);
        vb.update(vertices);

        // Bind character texture and render
        ch.texture.bind();
        vb.render();

        current_x += static_cast<float>(ch.advance);
    }

    // Restore previous framebuffer
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, currentFBO));
}

const RenderTexture& Text::getTexture() {
    return text_texture;
}

Vector2f Text::calculateSize() const {
    Rect text_rect;
    float current_x = 0.0f;

    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);
        float effective_char_width = 0.0f;
        float texture_width = static_cast<float>(ch.texture.getWidth());
        if (texture_width > 0 && i < string.size() - 1) {
            effective_char_width = texture_width;
        } else {
            effective_char_width = static_cast<float>(ch.advance);
        }

        Rect char_rect;
        char_rect.position.x = current_x;
        char_rect.position.y = -static_cast<float>(ch.height);
        char_rect.size.x = effective_char_width;
        char_rect.size.y = static_cast<float>(ch.texture.getHeight());

        text_rect.extend(char_rect);
        current_x += static_cast<float>(ch.advance);
    }

    return Vector2f(text_rect.size.x, text_rect.size.y);
}

}
