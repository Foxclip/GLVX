#include "glvis_tests/basic_tests.h"

BasicTestsModule::BasicTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) { {
        auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    }
}

void BasicTestsModule::clearTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("clear");

    // Clear the window with red
    window.clear(Color::Red);
    window.display();
    Image image = window.readPixels();
    Vector2f window_center = window.getCenter();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(window_center)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(WINDOW_SIZE - Vector2i(1, 1)), Color::Red, &Color::toString);

    // Resize the window
    const Vector2i new_window_size = RESIZED_WINDOW_SIZE;
    window.setSize(new_window_size);

    // Clear the window with green
    window.clear(Color::Green);
    window.display();
    image = window.readPixels();
    Vector2f new_window_center = window.getCenter();
    T_COMPARE(image.getPixel(0, 0), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(new_window_center)), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(new_window_size - Vector2i(1, 1)), Color::Green, &Color::toString);
}
