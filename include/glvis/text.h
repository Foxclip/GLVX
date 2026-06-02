#pragma once

#include "glvis/rectangle.h"
#include "glvis/font.h"
#include "glvis/render_texture.h"
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
    const RenderTexture& getTexture();
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;

private:
    Font* font = nullptr;
    std::string string;
    RenderTexture text_texture;
    Rect text_bounds;

    Rect calculateVisualBounds() const;

};

}
