#include "glvis/text.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/vertex.h"
#include "glvis/vertex_buffer.h"
#include <glad/glad.h>
#include <vector>
#include <cassert>

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

        // Height calculation: from highest point (top of tallest char) to lowest (bottom of deepest descender)
        // bitmap_top is measured from baseline (positive going up)
        // texture height extends below the bitmap_top position
        float char_top = static_cast<float>(ch.y);
        float char_bottom = char_top - static_cast<float>(ch.texture.getHeight());

        text_height = std::max(text_height, char_top - char_bottom);

        current_x += static_cast<float>(ch.advance);
    }

    setSize(text_width, text_height);

    float x = 0.0f;
    float y = 0.0f;

    for (char c : string) {
        const Character& ch = font->getCharacter(c);

        // Calculate position
        // bitmap_left is measured from the current cursor position
        // bitmap_top is measured from the baseline (positive going up in FreeType)
        // In OpenGL, +Y is up, but we need to flip for text rendering
        float xpos = x + ch.x;
        float ypos = y - ch.y;

        float w = static_cast<float>(ch.texture.getWidth());
        float h = static_cast<float>(ch.texture.getHeight());

        // TODO: blit character textures onto text_texture
    }
}

}
