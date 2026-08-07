#pragma once

#include "glvx/shape.h"
#include "glvx/font.h"
#include "glvx/vector.h"
#include "glvx/float_rect.h"
#include <string>

namespace glvx {

class Text : public Shape {
public:
    Text() = default;
    Text(Font* font, const std::string& string);
    Font* getFont() const;
    void setFont(Font* font);
    const std::string& getString() const;
    void setString(const std::string& string);
    float getMaxWidth() const;
    void setMaxWidth(float max_width);
    float getWidth() const;
    float getHeight() const;
    Vector2f getSize() const;

protected:
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;

private:
    Font* m_font = nullptr;
    std::string m_string;
    float m_max_width = 0.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;

    std::vector<std::string> breakLines() const;
    float measureWidth(const std::string& text) const;
    FloatRect calculateVisualBounds() const;

};

}
