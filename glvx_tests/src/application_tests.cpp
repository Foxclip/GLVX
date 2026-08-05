#include "glvx_tests/application_tests.h"
#include "glvx_tests/test_application.h"
#include <GLFW/glfw3.h>

ApplicationTestsModule::ApplicationTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto init_test = addTest("init", [&](test::Test& test) { initTest(test); });
        auto clear_test = addTest("clear", { init_test }, [&](test::Test& test) { clearTest(test); });
        auto rectangle_test = addTest("rectangle", { clear_test }, [&](test::Test& test) { rectangleTest(test); });
        auto move_rectangle_test = addTest("move_rectangle", { rectangle_test }, [&](test::Test& test) { moveRectangleTest(test); });
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

void ApplicationTestsModule::rectangleTest(test::Test& test) {
    app.toggleRectangle();
    app.advance();

    Image image = app.readPixels();
    Vector2i rectangle_bottom_right = TEST_APP_RECT_SIZE - Vector2i(1, 1);
    Vector2i rectangle_outside_bottom_right = rectangle_bottom_right + Vector2i(1, 1);
    T_COMPARE(image.getPixel(0, 0), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_bottom_right), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_outside_bottom_right), TEST_APP_CLEAR_COLOR, &Color::toString);
}

void ApplicationTestsModule::moveRectangleTest(test::Test& test) {
    Window::keyCallbackGLFW(app.getWindow().getWindowHandle(), GLFW_KEY_RIGHT, 0, GLFW_PRESS, 0);
    Window::keyCallbackGLFW(app.getWindow().getWindowHandle(), GLFW_KEY_DOWN, 0, GLFW_PRESS, 0);
    app.advance();

    Image image = app.readPixels();
    Vector2i new_rectangle_pos(static_cast<int>(TEST_APP_MOVE_STEP), static_cast<int>(TEST_APP_MOVE_STEP));
    Vector2i rectangle_bottom_right = new_rectangle_pos + TEST_APP_RECT_SIZE - Vector2i(1, 1);
    Vector2i rectangle_outside_bottom_right = rectangle_bottom_right + Vector2i(1, 1);
    T_COMPARE(image.getPixel(0, 0), TEST_APP_CLEAR_COLOR, &Color::toString);
    T_COMPARE(image.getPixel(new_rectangle_pos), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_bottom_right), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_outside_bottom_right), TEST_APP_CLEAR_COLOR, &Color::toString);
}
