#include "application.h"
#include <cmath>
#include <numbers>

void Application::init() {
    m_window.create(800, 600, "GLVX sandbox");
    setupShapes();
}

void Application::setupShapes() {
    m_rectangle_red.setColor(glvx::Color::Red);
    m_rectangle_green.setColor(glvx::Color::Green);
    m_rectangle_blue.setColor(glvx::Color::Blue);
    m_rectangle_red.setPosition(10.0f, 10.0f);
    m_rectangle_green.setPosition(40.0f, 10.0f);
    m_rectangle_blue.setPosition(70.0f, 10.0f);

    m_rectangle_red_transparent.setColor(glvx::Color(255, 0, 0, 128));
    m_rectangle_red_transparent.setPosition(10.0f, 40.0f);

    m_circle.setColor(glvx::Color::Yellow);
    m_circle.setPosition(10.0f, 70.0f);

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
    m_hexagon.setPosition(50.0f, 80.0f);
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

void Application::render() {
    m_view.setPosition(m_window.getCenter());
    m_window.setView(m_view);
    m_window.clear(glvx::Color::Black);

    m_window.draw(m_rectangle_red);
    m_window.draw(m_rectangle_green);
    m_window.draw(m_rectangle_blue);

    m_window.draw(m_rectangle_red_transparent);

    m_window.draw(m_circle);
    m_window.draw(m_hexagon);

    m_window.display();
}
