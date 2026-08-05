#include "glvx_tests/application_tests.h"

TestApplication app;

ApplicationTestsModule::ApplicationTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto init_test = addTest("init", [&](test::Test& test) { initTest(test); });
        auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    }
}

void ApplicationTestsModule::initTest(test::Test& test) {
    app.init();
}

void ApplicationTestsModule::clearTest(test::Test& test) {
    app.advance();

    Image image = app.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        Vector2i(0, 0),
        Vector2i(TEST_APP_WINDOW_WIDTH, TEST_APP_WINDOW_HEIGHT),
        TEST_APP_CLEAR_COLOR
    ));
}

TestApplication::TestApplication() : m_window(), m_rectangle(100, 100) { }

void TestApplication::init() {
    m_window.create(TEST_APP_WINDOW_WIDTH, TEST_APP_WINDOW_HEIGHT, "glvx test application");
    m_view.setPosition(m_window.getCenter());
    m_rectangle.setColor(glvx::Color(255, 0, 0));
}

void TestApplication::advance() {
    process_input();
    render();
}

glvx::Image TestApplication::readPixels() {
    return m_window.readPixels();
}

void TestApplication::process_input() {
    glvx::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == glvx::EventType::Closed) {
            m_window.close();
        }
    }
}

void TestApplication::render() {
    m_window.setView(m_view);
    m_window.clear(glvx::Color(64, 128, 255));
    // m_window.draw(m_rectangle);
    m_window.display();
}
