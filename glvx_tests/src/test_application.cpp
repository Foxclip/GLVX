#include "glvx_tests/test_application.h"

TestApplication app;

TestApplication::TestApplication() : m_window(), m_rectangle(100, 100) { }

void TestApplication::init() {
    m_window.create(TEST_APP_WINDOW_WIDTH, TEST_APP_WINDOW_HEIGHT, "glvx test application");
    m_view.setPosition(m_window.getCenter());
    m_rectangle.setColor(glvx::Color(255, 0, 0));
    m_rectangle.setSize(static_cast<Vector2f>(TEST_APP_RECT_SIZE));
}

void TestApplication::advance() {
    process_input();
    render();
}

glvx::Image TestApplication::readPixels() {
    return m_window.readPixels();
}

void TestApplication::toggleRectangle() {
    m_draw_rectangle = !m_draw_rectangle;
}

Vector2f TestApplication::getRectanglePosition() const {
    return m_rectangle.getPosition();
}

Window& TestApplication::getWindow() {
    return m_window;
}

void TestApplication::process_input() {
    Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == glvx::EventType::Closed) {
            m_window.close();
        }
        if (event.type == glvx::EventType::KeyPressed) {
            if (event.key.code == Key::Up) {
                m_rectangle.move(0, -TEST_APP_MOVE_STEP);
            }
            if (event.key.code == Key::Down) {
                m_rectangle.move(0, TEST_APP_MOVE_STEP);
            }
            if (event.key.code == Key::Left) {
                m_rectangle.move(-TEST_APP_MOVE_STEP, 0);
            }
            if (event.key.code == Key::Right) {
                m_rectangle.move(TEST_APP_MOVE_STEP, 0);
            }
        }
    }
}

void TestApplication::render() {
    m_window.setView(m_view);
    m_window.clear(Color(TEST_APP_CLEAR_COLOR));
    if (m_draw_rectangle) {
        m_window.draw(m_rectangle);
    }
    m_window.display();
}
