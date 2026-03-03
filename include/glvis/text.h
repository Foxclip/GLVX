#pragma once

#include "glvis/rectangle.h"
#include "glvis/font.h"
#include "glvis/render_texture.h"

namespace glvis {

class Text : public Rectangle {
public:
    Text() = default;
    Text(Font* font, const std::string& string);
    Font* getFont() const;
    void setFont(Font* font);
    const std::string& getString() const;
    void setString(const std::string& string);

private:
    Font* font = nullptr;
    std::string string;
    RenderTexture text_texture;

};

}
