#pragma once

#include "glvis/rectangle.h"
#include "glvis/font.h"
#include "glvis/vertex_buffer.h"
#include "glvis/vector.h"
#include "glvis/rect.h"
#include <string>
#include <vector>

namespace glvis {

class Text : public Rectangle {
public:
    Text() = default;
    Text(Font* font, const std::string& string);
    Font* getFont() const;
    void setFont(Font* font);
    const std::string& getString() const;
    void setString(const std::string& string);
    float getMaxWidth() const;
    void setMaxWidth(float max_width);
    const VertexBuffer& getVertexBuffer() const override;
    Shader* getDefaultShader() const override;

private:
    Font* font = nullptr;
    std::string string;
    float max_width = 0.0f;
    VertexBuffer vertex_buffer;
    Rect text_bounds;

    std::vector<std::string> breakLines() const;
    float measureWidth(const std::string& text) const;
    Rect calculateVisualBounds() const;

};

}
