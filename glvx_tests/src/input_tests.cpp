#include "glvx_tests/input_tests.h"
#include "glvx/keyboard.h"

InputTestsModule::InputTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    addTest("mouse_move", [&](test::Test& test) { mouseMoveTest(test); });
    addTest("mouse_button_pressed", [&](test::Test& test) { mouseButtonPressedTest(test); });
    addTest("mouse_button_released", [&](test::Test& test) { mouseButtonReleasedTest(test); });
    addTest("mouse_wheel_scrolled", [&](test::Test& test) { mouseWheelScrolledTest(test); });
    addTest("key_pressed", [&](test::Test& test) { keyPressedTest(test); });
    addTest("key_released", [&](test::Test& test) { keyReleasedTest(test); });
    addTest("key_pressed_with_modifier", [&](test::Test& test) { keyPressedWithModifierTest(test); });
    addTest("text_entered", [&](test::Test& test) { textEnteredTest(test); });
    addTest("keyboard_state", [&](test::Test& test) { keyboardStateTest(test); });
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

void InputTestsModule::keyPressedTest(test::Test& test) {
    window.clearEventQueue();

    Window::keyCallbackGLFW(window.getWindowHandle(), GLFW_KEY_A, 0, GLFW_PRESS, 0);

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::KeyPressed);
    T_CHECK(event.key.code == Key::A);
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::keyReleasedTest(test::Test& test) {
    window.clearEventQueue();

    Window::keyCallbackGLFW(window.getWindowHandle(), GLFW_KEY_A, 0, GLFW_RELEASE, 0);

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::KeyReleased);
    T_CHECK(event.key.code == Key::A);
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::keyPressedWithModifierTest(test::Test& test) {
    window.clearEventQueue();

    Window::keyCallbackGLFW(window.getWindowHandle(), GLFW_KEY_A, 0, GLFW_PRESS, GLFW_MOD_SHIFT);

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::KeyPressed);
    T_CHECK(event.key.code == Key::A);
    T_CHECK(static_cast<int>(event.key.modifier) & static_cast<int>(Modifier::Shift));
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::textEnteredTest(test::Test& test) {
    window.clearEventQueue();

    Window::charCallbackGLFW(window.getWindowHandle(), 'H');

    Event event;
    T_CHECK(window.pollEvent(event));
    T_CHECK(event.type == EventType::TextEntered);
    T_CHECK(event.text.unicode == U'H');
    T_CHECK(!window.pollEvent(event));
}

void InputTestsModule::keyboardStateTest(test::Test& test) {
    Window::keyCallbackGLFW(window.getWindowHandle(), GLFW_KEY_A, 0, GLFW_PRESS, 0);
    T_CHECK(Keyboard::isKeyPressed(Key::A));

    Window::keyCallbackGLFW(window.getWindowHandle(), GLFW_KEY_A, 0, GLFW_RELEASE, 0);
    T_CHECK(!Keyboard::isKeyPressed(Key::A));

    Keyboard::reset();
}
