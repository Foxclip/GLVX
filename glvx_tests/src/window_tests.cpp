#include "glvx_tests/window_tests.h"
#include "glvx/rectangle.h"

WindowTestsModule::WindowTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto window_resize_test = addTest("window_resize", [&](test::Test& test) { windowResizeTest(test); });
    auto window_recreate_test = addTest("window_recreate", [&](test::Test& test) { windowRecreateTest(test); });
    auto window_double_close_test = addTest("window_double_close", { window_recreate_test }, [&](test::Test& test) { windowDoubleCloseTest(test); });
    auto window_draw_after_recreate_test = addTest("window_draw_after_recreate", { window_double_close_test }, [&](test::Test& test) { windowDrawAfterRecreateTest(test); });
}

void WindowTestsModule::windowResizeTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("window resize");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Draw a 10x10 red rectangle
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image initialImage = window.readPixels();

    // Resize window
    window.setSize(RESIZED_WINDOW_SIZE);
    window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image resizedImage = window.readPixels();

    // Resize back to original size
    window.setSize(WINDOW_SIZE);
    window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image finalImage = window.readPixels();

    // Compare initial and final images pixel by pixel
    T_WRAP_CONTAINER(compareImages(test, finalImage, initialImage));
}

void WindowTestsModule::windowRecreateTest(test::Test& test) {
    window.close();
    window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "recreated");
    T_CHECK(window.isOpen() == true);
    T_CHECK(window.getWidth() == WINDOW_SIZE.x);
    T_CHECK(window.getHeight() == WINDOW_SIZE.y);
}

void WindowTestsModule::windowDoubleCloseTest(test::Test& test) {
    window.close();
    window.close();
}

void WindowTestsModule::windowDrawAfterRecreateTest(test::Test& test) {
    window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "post-cycle");
    window.clear(Color::Black);
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(0, 0), Vector2i(9, 9), Color::Red));
}

void WindowTestsModule::afterRunModule() {
    if (!window.isOpen()) {
        window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "glvx tests");
    }
}
