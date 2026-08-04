#include "glvx/mouse.h"
#include "glvx/window.h"

namespace glvx {

std::unordered_map<GLFWwindow*, Mouse::MouseState> Mouse::window_states;

bool Mouse::isButtonPressed(const Window& window, Button button) {
    auto it = window_states.find(window.getWindowHandle());
    if (it == window_states.end()) {
        return false;
    }
    size_t idx = static_cast<size_t>(button);
    if (idx >= static_cast<size_t>(Button::Count)) {
        return false;
    }
    return it->second.button_states[idx];
}

Vector2i Mouse::getPosition(const Window& window) {
    double x, y;
    glfwGetCursorPos(window.getWindowHandle(), &x, &y);
    return Vector2i(static_cast<int>(x), static_cast<int>(y));
}

void Mouse::setPosition(const Window& window, const Vector2i& position) {
    glfwSetCursorPos(window.getWindowHandle(), position.x, position.y);
}

void Mouse::setButtonState(GLFWwindow* handle, Button button, bool pressed) {
    auto it = window_states.find(handle);
    if (it == window_states.end()) {
        return;
    }
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(Button::Count)) {
        it->second.button_states[idx] = pressed;
    }
}

void Mouse::reset() {
    window_states.clear();
}

}
