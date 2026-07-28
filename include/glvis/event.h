#pragma once

#include "glvis/key.h"
#include "glvis/mouse.h"
#include <cstdint>

namespace glvis {

enum class EventType {
    None = 0,
    Closed = 1,
    Resized = 2,
    Moved = 3,
    FocusLost = 4,
    FocusGained = 5,
    TextEntered = 6,
    KeyPressed = 7,
    KeyReleased = 8,
    MouseMoved = 9,
    MouseButtonPressed = 10,
    MouseButtonReleased = 11,
    MouseWheelScrolled = 12
};

struct Event {
    EventType type = EventType::None;

    struct SizeEvent {
        unsigned int width = 0;
        unsigned int height = 0;
    };

    struct MoveEvent {
        int x = 0;
        int y = 0;
    };

    struct KeyEvent {
        Key code = Key::Unknown;
        Modifier modifier = Modifier::None;
        bool altGr = false;
    };

    struct TextEvent {
        char32_t unicode = 0;
    };

    struct MouseMoveEvent {
        int x = 0;
        int y = 0;
    };

    struct MouseButtonEvent {
        Mouse::Button button = Mouse::Button::Left;
        int x = 0;
        int y = 0;
    };

    struct MouseWheelEvent {
        float delta = 0.0f;
        int x = 0;
        int y = 0;
    };

    SizeEvent size;
    MoveEvent pos;
    KeyEvent key;
    TextEvent text;
    MouseMoveEvent mouseMove;
    MouseButtonEvent mouseButton;
    MouseWheelEvent mouseWheel;
};

}
