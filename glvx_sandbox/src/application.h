#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/circle.h"
#include "glvx/convex_shape.h"
#include "glvx/mouse.h"
#include "glvx/view.h"
#include "glvx/color.h"
#include "glvx/text.h"
#include "glvx/texture.h"

class Application {
public:
    void init();
    void run();

private:
    static const int NUM_TRANSPARENT_RECTANGLES = 10;
    static const int NUM_CURSOR_TYPES = 21;
    inline static const float ARROW_PERIOD_SECONDS = 5.0f;
    inline static const float CURSOR_TILE_W = 34.0f;
    inline static const float CURSOR_TILE_H = 62.0f;
    inline static const float CURSOR_TILE_GAP = 2.0f;
    inline static const float CURSOR_ROW_X = 10.0f;
    inline static const float CURSOR_ROW_Y = 260.0f;
    inline static const float CURSOR_ICON_BOX = 24.0f;

    glvx::Window m_window;
    glvx::View m_view;
    glvx::Rectangle m_rectangle_red{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_green{20.0f, 20.0f};
    glvx::Rectangle m_rectangle_blue{20.0f, 20.0f};
    glvx::Circle m_circle{10.0f};
    glvx::ConvexShape m_hexagon{6};
    glvx::Rectangle m_transparent_rectangles[NUM_TRANSPARENT_RECTANGLES];
    glvx::Rectangle m_rgb_group_rectangles[NUM_TRANSPARENT_RECTANGLES][3];
    glvx::Font m_font_normal;
    glvx::Font m_font_subpixel;
    glvx::Text m_text_normal;
    glvx::Text m_text_subpixel;
    std::chrono::steady_clock::time_point m_start_time;
    glvx::ConvexShape m_arrow{3};
    glvx::ConvexShape m_mouse_arrow{3};
    glvx::Rectangle m_button_background{120.0f, 30.0f};
    glvx::Text m_button_label;
    int m_button_press_count = 0;

    glvx::Cursor m_cursor_types[NUM_CURSOR_TYPES];
    glvx::Cursor m_arrow_cursor;
    glvx::Texture m_cursor_icons[NUM_CURSOR_TYPES];
    glvx::Rectangle m_cursor_tiles[NUM_CURSOR_TYPES];
    glvx::Rectangle m_cursor_icon_rects[NUM_CURSOR_TYPES];
    glvx::Text m_cursor_labels[NUM_CURSOR_TYPES];
    int m_current_cursor_index = -1;

    void setupShapes();
    void setupCursorRow();
    void handleEvents();
    void updateArrow();
    void updateMouseArrow();
    void updateButton();
    void updateCursorRow();
    bool isMouseOverButton(const glvx::Vector2f& point_world) const;
    void setButtonLabel();
    static bool loadCursorIcon(
        glvx::Cursor::Type type,
        std::vector<unsigned char>& out_pixels,
        int& out_width,
        int& out_height
    );
    static std::string cursorDisplayName(glvx::Cursor::Type type);
    void render();
};
