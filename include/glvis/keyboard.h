#pragma once

#include "glvis/key.h"

namespace glvis {

class Keyboard {
public:
    static bool isKeyPressed(Key key);
    static bool isModifierActive(Modifier modifier);
    static void setKeyState(Key key, bool pressed);
    static void reset();

private:
    static constexpr int NumKeys = 350;
    static bool key_states[NumKeys];
};

}
