#pragma once

#include "glvx/key.h"

namespace glvx {

class Keyboard {
public:
    static bool isKeyPressed(Key key);
    static bool isModifierActive(Modifier modifier);
    static void reset();

private:
    friend class Window;

    static void setKeyState(Key key, bool pressed);

    static constexpr int NumKeys = 350;
    static bool m_key_states[NumKeys];
};

}
