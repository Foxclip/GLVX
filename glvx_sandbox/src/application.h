#pragma once

#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/circle.h"
#include "glvx/convex_shape.h"
#include "glvx/view.h"
#include "glvx/color.h"
#include "glvx/text.h"

class Application {
public:
    void init();
    void run();

private:
    void setupShapes();
    void handleEvents();
    void render();

    glvx::Window m_window;
    glvx::View m_view;
    glvx::Rectangle m_rectangle_red{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_green{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_blue{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_red_transparent{20.0f, 20.0f};
    glvx::Circle m_circle{10.0f};
    glvx::ConvexShape m_hexagon{6};
    glvx::Font m_font_normal;
    glvx::Font m_font_subpixel;
    glvx::Text m_text_normal;
    glvx::Text m_text_subpixel;
};
