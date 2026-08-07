#pragma once

#include "glvx/rectangle.h"
#include "glvx/font.h"
#include "glvx/vertex_buffer.h"
#include "glvx/vector.h"
#include "glvx/float_rect.h"
#include <string>
#include <vector>

namespace glvx {

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
private:
    Font* m_font = nullptr;
    std::string m_string;
    float m_max_width = 0.0f;
    VertexBuffer m_vertex_buffer;
    FloatRect m_text_bounds;

    std::vector<std::string> breakLines() const;
    float measureWidth(const std::string& text) const;
    FloatRect calculateVisualBounds() const;

};

}
