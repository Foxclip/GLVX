#include "application.h"
#include <cmath>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static int transparentAlpha(int index) {
    int alpha = static_cast<int>(256 / pow(2, index));
    if (index == 0) {
        alpha = 255;
    }
    return alpha;
}

bool Application::loadCursorIcon(
    glvx::Cursor::Type type,
    std::vector<unsigned char>& out_pixels,
    int& out_width,
    int& out_height
) {
    out_pixels.clear();
    out_width = 0;
    out_height = 0;

#ifdef _WIN32
    // Loads the same Win32 system cursor resource the library uses for that
    // type (see src/cursor.cpp and glfw's win32_window.c), then copies its
    // color bitmap to RGBA, top-down.
    // In the Windows SDK the IDC_* macros expand to MAKEINTRESOURCE (LPSTR),
    // so cast back to the numeric id, as src/cursor.cpp does.
    DWORD resource_id = 0;
    switch (type) {
        case glvx::Cursor::Type::Arrow:                    resource_id = (DWORD)(uintptr_t)IDC_ARROW; break;
        case glvx::Cursor::Type::ArrowWait:                resource_id = (DWORD)(uintptr_t)IDC_APPSTARTING; break;
        case glvx::Cursor::Type::Wait:                     resource_id = (DWORD)(uintptr_t)IDC_WAIT; break;
        case glvx::Cursor::Type::Text:                     resource_id = (DWORD)(uintptr_t)IDC_IBEAM; break;
        case glvx::Cursor::Type::Hand:                     resource_id = (DWORD)(uintptr_t)IDC_HAND; break;
        case glvx::Cursor::Type::SizeHorizontal:           resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeVertical:             resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeTopLeftBottomRight:   resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomLeftTopRight:   resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeLeft:                 resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeRight:                resource_id = (DWORD)(uintptr_t)IDC_SIZEWE; break;
        case glvx::Cursor::Type::SizeTop:                  resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeBottom:               resource_id = (DWORD)(uintptr_t)IDC_SIZENS; break;
        case glvx::Cursor::Type::SizeTopLeft:              resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomRight:          resource_id = (DWORD)(uintptr_t)IDC_SIZENWSE; break;
        case glvx::Cursor::Type::SizeBottomLeft:           resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeTopRight:             resource_id = (DWORD)(uintptr_t)IDC_SIZENESW; break;
        case glvx::Cursor::Type::SizeAll:                  resource_id = (DWORD)(uintptr_t)IDC_SIZEALL; break;
        case glvx::Cursor::Type::Cross:                    resource_id = (DWORD)(uintptr_t)IDC_CROSS; break;
        case glvx::Cursor::Type::Help:                     resource_id = (DWORD)(uintptr_t)IDC_HELP; break;
        case glvx::Cursor::Type::NotAllowed:               resource_id = (DWORD)(uintptr_t)IDC_NO; break;
    }

    HCURSOR h_cursor = (HCURSOR)LoadImageW(
        NULL, MAKEINTRESOURCEW(resource_id), IMAGE_CURSOR,
        0, 0, LR_DEFAULTSIZE | LR_SHARED
    );
    if (!h_cursor) {
        return false;
    }

    ICONINFO icon_info = {};
    if (!GetIconInfo(h_cursor, &icon_info)) {
        return false;
    }

    if (icon_info.hbmColor == NULL) {
        // monochrome cursors have no color bitmap
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    BITMAP bitmap = {};
    if (!GetObjectW(icon_info.hbmColor, sizeof(BITMAP), &bitmap)) {
        DeleteObject(icon_info.hbmColor);
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    const int width = bitmap.bmWidth;
    const int height = bitmap.bmHeight;
    std::vector<unsigned char> pixels(
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4
    );

    HDC dc = GetDC(NULL);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = -height; // top-down
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    int retrieved = GetDIBits(
        dc, icon_info.hbmColor, 0, height,
        pixels.data(), &bitmap_info, DIB_RGB_COLORS
    );
    ReleaseDC(NULL, dc);

    DeleteObject(icon_info.hbmColor);
    DeleteObject(icon_info.hbmMask);

    if (retrieved <= 0) {
        return false;
    }

    // GetDIBits yields B,G,R,A memory bytes; we want R,G,B,A
    for (int i = 0; i < width * height; i++) {
        std::swap(
            pixels[static_cast<std::size_t>(i) * 4],
            pixels[static_cast<std::size_t>(i) * 4 + 2]
        );
    }

    out_pixels = std::move(pixels);
    out_width = width;
    out_height = height;
    return true;
#else
    (void)type;
    return false;
#endif
}

std::string Application::cursorDisplayName(glvx::Cursor::Type type) {
    const char* raw_name = "Unknown";
    switch (type) {
        case glvx::Cursor::Type::Arrow:                    raw_name = "Arrow"; break;
        case glvx::Cursor::Type::ArrowWait:                raw_name = "ArrowWait"; break;
        case glvx::Cursor::Type::Wait:                     raw_name = "Wait"; break;
        case glvx::Cursor::Type::Text:                     raw_name = "Text"; break;
        case glvx::Cursor::Type::Hand:                     raw_name = "Hand"; break;
        case glvx::Cursor::Type::SizeHorizontal:           raw_name = "SizeHorizontal"; break;
        case glvx::Cursor::Type::SizeVertical:             raw_name = "SizeVertical"; break;
        case glvx::Cursor::Type::SizeTopLeftBottomRight:   raw_name = "SizeTopLeftBottomRight"; break;
        case glvx::Cursor::Type::SizeBottomLeftTopRight:   raw_name = "SizeBottomLeftTopRight"; break;
        case glvx::Cursor::Type::SizeLeft:                 raw_name = "SizeLeft"; break;
        case glvx::Cursor::Type::SizeRight:                raw_name = "SizeRight"; break;
        case glvx::Cursor::Type::SizeTop:                  raw_name = "SizeTop"; break;
        case glvx::Cursor::Type::SizeBottom:               raw_name = "SizeBottom"; break;
        case glvx::Cursor::Type::SizeTopLeft:              raw_name = "SizeTopLeft"; break;
        case glvx::Cursor::Type::SizeBottomRight:          raw_name = "SizeBottomRight"; break;
        case glvx::Cursor::Type::SizeBottomLeft:           raw_name = "SizeBottomLeft"; break;
        case glvx::Cursor::Type::SizeTopRight:             raw_name = "SizeTopRight"; break;
        case glvx::Cursor::Type::SizeAll:                  raw_name = "SizeAll"; break;
        case glvx::Cursor::Type::Cross:                    raw_name = "Cross"; break;
        case glvx::Cursor::Type::Help:                     raw_name = "Help"; break;
        case glvx::Cursor::Type::NotAllowed:               raw_name = "NotAllowed"; break;
    }

    // Insert a space before each capital except the first so the Text
    // line-breaking (which splits on spaces) can wrap the long names.
    std::string name;
    for (std::size_t i = 0; raw_name[i] != '\0'; i++) {
        if (i > 0 && raw_name[i] >= 'A' && raw_name[i] <= 'Z') {
            name += ' ';
        }
        name += raw_name[i];
    }
    return name;
}

void Application::init() {
    m_window.create(800, 600, "GLVX sandbox");
    m_font_normal.openFromFile("fonts/LiberationSans-Regular.ttf");
    m_font_subpixel.openFromFile("fonts/LiberationSans-Regular.ttf", true);
    m_start_time = std::chrono::steady_clock::now();
    setupShapes();
    setupCursorRow();
}

void Application::setupShapes() {
    m_rectangle_red.setColor(glvx::Color::Red);
    m_rectangle_green.setColor(glvx::Color::Green);
    m_rectangle_blue.setColor(glvx::Color::Blue);
    m_rectangle_red.setPosition(10.0f, 10.0f);
    m_rectangle_green.setPosition(40.0f, 10.0f);
    m_rectangle_blue.setPosition(70.0f, 10.0f);

    m_circle.setColor(glvx::Color::Yellow);
    m_circle.setPosition(10.0f, 40.0f);

    const float hexagon_radius = 10.0f;
    float pi_f = static_cast<float>(std::numbers::pi);
    float point_count_f = static_cast<float>(m_hexagon.getPointCount());
    for (std::size_t i = 0; i < m_hexagon.getPointCount(); ++i) {
        float i_f = static_cast<float>(i);
        const float angle = 2.0f * pi_f * i_f / point_count_f;
        m_hexagon.setPoint(i, glvx::Vector2f(
            hexagon_radius * std::cos(angle),
            hexagon_radius * std::sin(angle)
        ));
    }
    m_hexagon.setColor(glvx::Color(0, 255, 255));
    m_hexagon.setPosition(50.0f, 50.0f);

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        int alpha = transparentAlpha(i);
        m_transparent_rectangles[i].setColor(glvx::Color(255, 0, 0, alpha));
        m_transparent_rectangles[i].setSize(glvx::Vector2f(20.0f, 20.0f));
        m_transparent_rectangles[i].setPosition(
            10.0f + i * 30.0f,
            70.0f
        );
    }

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        int alpha = transparentAlpha(i);
        float x = 10.0f + i * 40.0f;
        m_rgb_group_rectangles[i][0].setColor(glvx::Color(255, 0, 0, alpha));
        m_rgb_group_rectangles[i][1].setColor(glvx::Color(0, 255, 0, alpha));
        m_rgb_group_rectangles[i][2].setColor(glvx::Color(0, 0, 255, alpha));
        for (int c = 0; c < 3; c++) {
            m_rgb_group_rectangles[i][c].setSize(glvx::Vector2f(20.0f, 20.0f));
        }
        m_rgb_group_rectangles[i][0].setPosition(x, 100.0f);
        m_rgb_group_rectangles[i][1].setPosition(x + 10.0f, 100.0f);
        m_rgb_group_rectangles[i][2].setPosition(x + 5.0f, 110.0f);
    }

    m_text_normal.setFont(&m_font_normal);
    m_text_normal.setCharacterSize(10);
    m_text_normal.setString("The quick brown fox jumps over the lazy dog.");
    m_text_normal.setPosition(10.0f, 140.0f);
    m_text_subpixel.setFont(&m_font_subpixel);
    m_text_subpixel.setCharacterSize(10);
    m_text_subpixel.setString("The quick brown fox jumps over the lazy dog.");
    m_text_subpixel.setPosition(10.0f, 160.0f);

    m_arrow.setPoint(0, glvx::Vector2f(12.0f, 0.0f));
    m_arrow.setPoint(1, glvx::Vector2f(-8.0f, -7.0f));
    m_arrow.setPoint(2, glvx::Vector2f(-8.0f, 7.0f));
    m_arrow.setColor(glvx::Color::White);
    m_arrow.setOrigin(0.0f, 0.0f);
    m_arrow.setPosition(10.0f, 190.0f);

    m_mouse_arrow.setPoint(0, glvx::Vector2f(12.0f, 0.0f));
    m_mouse_arrow.setPoint(1, glvx::Vector2f(-8.0f, -7.0f));
    m_mouse_arrow.setPoint(2, glvx::Vector2f(-8.0f, 7.0f));
    m_mouse_arrow.setColor(glvx::Color::Yellow);
    m_mouse_arrow.setOrigin(0.0f, 0.0f);
    m_mouse_arrow.setPosition(40.0f, 190.0f);

    m_button_background.setColor(glvx::Color(70, 130, 180));
    m_button_background.setPosition(10.0f, 220.0f);
    m_button_label.setFont(&m_font_normal);
    m_button_label.setCharacterSize(14);
    setButtonLabel();
}

void Application::setupCursorRow() {
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        const glvx::Cursor::Type type = static_cast<glvx::Cursor::Type>(i);
        m_cursor_types[i].loadFromSystem(type);

        std::vector<unsigned char> pixels;
        int icon_width = 0;
        int icon_height = 0;
        if (loadCursorIcon(type, pixels, icon_width, icon_height)) {
            m_cursor_icons[i].create(icon_width, icon_height, pixels.data(), 4);
        }

        const float tile_x = CURSOR_ROW_X + i * (CURSOR_TILE_W + CURSOR_TILE_GAP);
        m_cursor_tiles[i].setSize(CURSOR_TILE_W, CURSOR_TILE_H);
        m_cursor_tiles[i].setPosition(tile_x, CURSOR_ROW_Y);
        m_cursor_tiles[i].setColor(glvx::Color(40, 40, 40));

        m_cursor_icon_rects[i].setSize(CURSOR_ICON_BOX, CURSOR_ICON_BOX);
        m_cursor_icon_rects[i].setPosition(
            tile_x + (CURSOR_TILE_W - CURSOR_ICON_BOX) / 2.0f,
            CURSOR_ROW_Y + 4.0f
        );
        if (m_cursor_icons[i].getID() != 0) {
            m_cursor_icon_rects[i].setTexture(&m_cursor_icons[i]);
        }
        m_cursor_icon_rects[i].setColor(glvx::Color::White);

        m_cursor_labels[i].setFont(&m_font_subpixel);
        m_cursor_labels[i].setCharacterSize(7);
        m_cursor_labels[i].setString(cursorDisplayName(type));
        m_cursor_labels[i].setMaxWidth(CURSOR_TILE_W - 2.0f);
        m_cursor_labels[i].setColor(glvx::Color::White);
        m_cursor_labels[i].setOrigin(m_cursor_labels[i].getWidth() / 2.0f, 0.0f);
        m_cursor_labels[i].setPosition(
            tile_x + CURSOR_TILE_W / 2.0f,
            CURSOR_ROW_Y + 30.0f
        );
    }

    m_arrow_cursor.loadFromSystem(glvx::Cursor::Type::Arrow);
    m_window.setMouseCursor(m_arrow_cursor);
}

void Application::run() {
    while (m_window.isOpen()) {
        handleEvents();
        render();
    }
}

void Application::handleEvents() {
    glvx::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == glvx::EventType::Closed) {
            m_window.close();
        }
        if (event.type == glvx::EventType::Resized) {
            m_view.setPosition(m_window.getCenter());
        }
        if (event.type == glvx::EventType::MouseButtonPressed) {
            if (event.mouseButton.button == glvx::Mouse::Button::Left) {
                const glvx::Vector2f click_world =
                    m_window.screenToWorld(event.mouseButton.x, event.mouseButton.y);
                if (isMouseOverButton(click_world)) {
                    m_button_press_count++;
                    setButtonLabel();
                }
            }
        }
    }
}

void Application::updateArrow() {
    const auto now = std::chrono::steady_clock::now();
    const float elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<float>>(now - m_start_time).count();
    const float rotation = std::fmod(elapsed_seconds, ARROW_PERIOD_SECONDS) / ARROW_PERIOD_SECONDS;
    m_arrow.setRotation(glvx::Angle::fromRadians(rotation * 2.0f * static_cast<float>(std::numbers::pi)));
}

void Application::updateMouseArrow() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    const glvx::Vector2f arrow_pos = m_mouse_arrow.getPosition();
    const float dx = mouse_world.x - arrow_pos.x;
    const float dy = mouse_world.y - arrow_pos.y;
    m_mouse_arrow.setRotation(glvx::Angle::fromRadians(-std::atan2(dy, dx)));
}

bool Application::isMouseOverButton(const glvx::Vector2f& point_world) const {
    const glvx::Vector2f position = m_button_background.getPosition();
    const glvx::Vector2f size = m_button_background.getSize();
    return point_world.x >= position.x &&
           point_world.x <= position.x + size.x &&
           point_world.y >= position.y &&
           point_world.y <= position.y + size.y;
}

void Application::setButtonLabel() {
    m_button_label.setString("Clicks: " + std::to_string(m_button_press_count));
    const glvx::Vector2f position = m_button_background.getPosition();
    m_button_label.setOrigin(
        m_button_label.getWidth() / 2.0f,
        m_button_label.getHeight() / 2.0f + 4.0f
    );
    m_button_label.setPosition(
        position.x + m_button_background.getWidth() / 2.0f,
        position.y + m_button_background.getHeight() / 2.0f
    );
}

void Application::updateButton() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    const bool hovered = isMouseOverButton(mouse_world);
    const bool pressed = hovered && glvx::Mouse::isButtonPressed(glvx::Mouse::Button::Left);

    glvx::Color background_color;
    if (pressed) {
        background_color = glvx::Color(30, 60, 110);
    }
    else if (hovered) {
        background_color = glvx::Color(100, 165, 220);
    }
    else {
        background_color = glvx::Color(70, 130, 180);
    }
    m_button_background.setColor(background_color);

    if (pressed) {
        m_button_label.setColor(glvx::Color(220, 220, 220));
    }
    else {
        m_button_label.setColor(glvx::Color::White);
    }
}

void Application::updateCursorRow() {
    const glvx::Vector2f mouse_world = m_window.screenToWorld(glvx::Mouse::getPosition(m_window));
    int hovered = -1;
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        const glvx::Vector2f position = m_cursor_tiles[i].getPosition();
        const glvx::Vector2f size = m_cursor_tiles[i].getSize();
        if (mouse_world.x >= position.x &&
            mouse_world.x <= position.x + size.x &&
            mouse_world.y >= position.y &&
            mouse_world.y <= position.y + size.y) {
            hovered = i;
            break;
        }
    }

    if (hovered != m_current_cursor_index) {
        if (hovered == -1) {
            m_window.setMouseCursor(m_arrow_cursor);
        } else {
            m_window.setMouseCursor(m_cursor_types[hovered]);
        }
        m_current_cursor_index = hovered;
        for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
            if (i == hovered) {
                m_cursor_tiles[i].setColor(glvx::Color(80, 90, 120));
            } else {
                m_cursor_tiles[i].setColor(glvx::Color(40, 40, 40));
            }
        }
    }
}

void Application::render() {
    m_view.setPosition(m_window.getCenter());
    m_window.setView(m_view);
    m_window.clear(glvx::Color::Black);

    m_window.draw(m_rectangle_red);
    m_window.draw(m_rectangle_green);
    m_window.draw(m_rectangle_blue);

    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        m_window.draw(m_transparent_rectangles[i]);
    }
    for (int i = 0; i < NUM_TRANSPARENT_RECTANGLES; i++) {
        m_window.draw(m_rgb_group_rectangles[i][0]);
        m_window.draw(m_rgb_group_rectangles[i][1]);
        m_window.draw(m_rgb_group_rectangles[i][2]);
    }

    m_window.draw(m_circle);
    m_window.draw(m_hexagon);

    m_window.draw(m_text_normal);
    m_window.draw(m_text_subpixel);

    updateArrow();
    m_window.draw(m_arrow);

    updateMouseArrow();
    m_window.draw(m_mouse_arrow);

    updateButton();
    m_window.draw(m_button_background);
    m_window.draw(m_button_label);

    updateCursorRow();
    for (int i = 0; i < NUM_CURSOR_TYPES; i++) {
        m_window.draw(m_cursor_tiles[i]);
        if (m_cursor_icons[i].getID() != 0) {
            m_window.draw(m_cursor_icon_rects[i]);
        }
        m_window.draw(m_cursor_labels[i]);
    }

    m_window.display();
}
