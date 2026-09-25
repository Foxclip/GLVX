#include "glvx/mouse.h"
#include "glvx/window.h"

namespace glvx {

bool Mouse::m_button_states[Mouse::num_buttons] = {};

bool Mouse::isButtonPressed(Button button) {
    int idx = static_cast<int>(button);
    if (idx < 0 || idx >= num_buttons) {
        return false;
    }
    return m_button_states[idx];
}

Vector2i Mouse::getPosition(const Window& window) {
    double x, y;
    glfwGetCursorPos(window.getWindowHandle(), &x, &y);
    return Vector2i(static_cast<int>(x), static_cast<int>(y));
}

void Mouse::setPosition(const Window& window, const Vector2i& position) {
    glfwSetCursorPos(window.getWindowHandle(), position.x, position.y);
}

void Mouse::setButtonState(Button button, bool pressed) {
    int idx = static_cast<int>(button);
    if (idx >= 0 && idx < num_buttons) {
        m_button_states[idx] = pressed;
    }
}

void Mouse::reset() {
    for (int i = 0; i < num_buttons; ++i) {
        m_button_states[i] = false;
    }
}

}
