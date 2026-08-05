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
    Vector2i rectangle_corner = TEST_APP_RECT_SIZE - Vector2i(1, 1);
    Vector2i rectangle_outside = rectangle_corner + Vector2i(1, 1);
    T_COMPARE(image.getPixel(0, 0), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_corner), glvx::Color(255, 0, 0), &Color::toString);
    T_COMPARE(image.getPixel(rectangle_outside), TEST_APP_CLEAR_COLOR, &Color::toString);
}

void ApplicationTestsModule::moveRectangleTest(test::Test& test) {
    Vector2f initial_pos = app.getRectanglePosition();

    Window::keyCallbackGLFW(app.getWindow().getWindowHandle(), GLFW_KEY_UP, 0, GLFW_PRESS, 0);
    app.advance();

    Vector2f new_pos = app.getRectanglePosition();
    T_COMPARE(new_pos.x, initial_pos.x);
    T_COMPARE(new_pos.y, initial_pos.y - TEST_APP_MOVE_STEP);
}
