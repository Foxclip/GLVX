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

    text_bounds = calculateVisualBounds();
    Vector2f text_size = text_bounds.size;
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
        Rect char_bounds;
        // if x position of the first character is non-zero, it will give the text non-zero x position,
        // so we need to subtract it
        char_bounds.position.x = current_x + static_cast<float>(ch.x) - text_bounds.position.x;
        char_bounds.position.y = static_cast<float>(ch.height);
        char_bounds.size.x = static_cast<float>(ch.texture.getWidth());
        char_bounds.size.y = static_cast<float>(ch.texture.getHeight());

        if (char_bounds.size.x <= 0 || char_bounds.size.y <= 0) {
            current_x += static_cast<float>(ch.advance);
            continue;
        }

        // Convert pixel coordinates to NDC [-1, 1]
        float char_left = char_bounds.position.x;
        float char_right = char_left + char_bounds.size.x;
        float char_bottom = char_bounds.position.y - char_bounds.size.y;
        float char_top = char_bounds.position.y;
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

void Text::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    if (string.empty()) {
        return;
    }
    RenderStates shifted = states;
    shifted.transform = Matrix4::translate(
        shifted.transform, Vector3(text_bounds.position.x, text_bounds.position.y, 0.0f)
    );
    Rectangle::render(view, projection, shifted);
}

Rect Text::calculateVisualBounds() const {
    Rect result;
    float current_x = 0.0f;

    for (size_t i = 0; i < string.size(); i++) {
        char c = string[i];
        const Character& ch = font->getCharacter(c);
        float effective_char_width = 0.0f;
        float texture_width = static_cast<float>(ch.texture.getWidth());
        if (texture_width > 0) {
            effective_char_width = texture_width;
        } else {
            effective_char_width = static_cast<float>(ch.advance);
        }

        Rect char_rect;
        int font_size = font->getCharacterSize();
        char_rect.position.x = current_x + static_cast<float>(ch.x);
        char_rect.position.y = static_cast<float>(font_size - ch.height);
        char_rect.size.x = effective_char_width;
        char_rect.size.y = static_cast<float>(ch.texture.getHeight());

        if (i == 0) {
            result = char_rect;
        } else {
            result.extend(char_rect);
        }
        current_x += static_cast<float>(ch.advance);
    }

    return result;
}

}
