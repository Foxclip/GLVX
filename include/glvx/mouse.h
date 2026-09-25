#pragma once

#include <glvx/vector.h>

namespace glvx {

class Window;

class Mouse {
public:
    enum class Button {
        Left = 0,
        Right = 1,
        Middle = 2,
        XButton1 = 3,
        XButton2 = 4,
        Count = 5
    };

    static bool isButtonPressed(Button button);
    static Vector2i getPosition(const Window& window);
    static void setPosition(const Window& window, const Vector2i& position);
    static void reset();

private:
    friend class Window;

    static void setButtonState(Button button, bool pressed);

    static constexpr int num_buttons = static_cast<int>(Button::Count);
    static bool m_button_states[num_buttons];
};

}
