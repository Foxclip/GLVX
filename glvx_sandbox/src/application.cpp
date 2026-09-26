#include "application.h"
#include <cmath>
#include <numbers>

static int transparentAlpha(int index) {
    int alpha = static_cast<int>(256 / pow(2, index));
    if (index == 0) {
        alpha = 255;
    }
    return alpha;
}

void Application::init() {
    m_window.create(800, 600, "GLVX sandbox");
    m_font_normal.openFromFile("fonts/LiberationSans-Regular.ttf");
    m_font_subpixel.openFromFile("fonts/LiberationSans-Regular.ttf", true);
    m_start_time = std::chrono::steady_clock::now();
    setupShapes();
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

    m_window.display();
}
