#include "glvis_tests/input_tests.h"

InputTestsModule::InputTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    addTest("mouse_move", [&](test::Test& test) { mouseMoveTest(test); });
    addTest("mouse_button_pressed", [&](test::Test& test) { mouseButtonPressedTest(test); });
    addTest("mouse_button_released", [&](test::Test& test) { mouseButtonReleasedTest(test); });
    addTest("mouse_wheel_scrolled", [&](test::Test& test) { mouseWheelScrolledTest(test); });
}

void InputTestsModule::mouseMoveTest(test::Test& test) {
    window.clearEventQueue();

    Window::mouseMoveCallbackGLFW(window.getWindowHandle(), 50, 50);

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::MouseMoved);
    T_COMPARE(event.mouseMove.x, 50);
    T_COMPARE(event.mouseMove.y, 50);
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::mouseButtonPressedTest(test::Test& test) {
    window.clearEventQueue();

    Window::mouseButtonCallbackGLFW(
        window.getWindowHandle(), static_cast<int>(Mouse::Button::Right), GLFW_PRESS, 0
    );

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::MouseButtonPressed);
    T_CHECK(event.mouseButton.button == Mouse::Button::Right);
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::mouseButtonReleasedTest(test::Test& test) {
    window.clearEventQueue();

    Window::mouseButtonCallbackGLFW(
        window.getWindowHandle(), static_cast<int>(Mouse::Button::Left), GLFW_RELEASE, 0
    );

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::MouseButtonReleased);
    T_CHECK(event.mouseButton.button == Mouse::Button::Left);
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::mouseWheelScrolledTest(test::Test& test) {
    window.clearEventQueue();

    Window::scrollCallbackGLFW(window.getWindowHandle(), 0, 3);

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::MouseWheelScrolled);
    T_COMPARE(event.mouseWheel.delta, 3.0f);
    T_CHECK(!window.pollEvent(event));
}
