#include "glvx_tests/window_tests.h"
#include "glvx/rectangle.h"

WindowTestsModule::WindowTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto window_resize_test = addTest("window_resize", [&](test::Test& test) { windowResizeTest(test); });
    auto window_recreate_after_close_test = addTest("window_recreate_after_close", [&](test::Test& test) { windowRecreateAfterCloseTest(test); });
    auto window_close_idempotent_test = addTest("window_close_idempotent", { window_recreate_after_close_test }, [&](test::Test& test) { windowCloseIdempotentTest(test); });
    auto window_create_preserves_state_after_recreate_test = addTest("window_create_preserves_state_after_recreate", { window_close_idempotent_test }, [&](test::Test& test) { windowCreatePreservesStateAfterRecreateTest(test); });
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

void WindowTestsModule::windowRecreateAfterCloseTest(test::Test& test) {
    window.close();
    window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "recreated");
    T_ASSERT(window.isOpen() == true);
    T_ASSERT(window.getWidth() == WINDOW_SIZE.x);
    T_ASSERT(window.getHeight() == WINDOW_SIZE.y);
}

void WindowTestsModule::windowCloseIdempotentTest(test::Test& test) {
    window.close();
    window.close();
}

void WindowTestsModule::windowCreatePreservesStateAfterRecreateTest(test::Test& test) {
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
