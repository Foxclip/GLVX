#pragma once

#include "glvis/rectangle.h"
#include "glvis/font.h"
#include "glvis/vertex_buffer.h"
#include "glvis/vector.h"
#include "glvis/rect.h"

namespace glvis {

class Text : public Rectangle {
public:
    Text() = default;
    Text(Font* font, const std::string& string);
    Font* getFont() const;
    void setFont(Font* font);
    const std::string& getString() const;
    void setString(const std::string& string);
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;

private:
    Font* font = nullptr;
    std::string string;
    VertexBuffer vertex_buffer;
    Rect text_bounds;

    Rect calculateVisualBounds() const;

};

}
