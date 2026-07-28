#pragma once

#include "glvis/key.h"

namespace glvis {

class Keyboard {
public:
    static bool isKeyPressed(key::Key key);
    static bool isModifierActive(key::Modifier modifier);

    static void setKeyState(key::Key key, bool pressed);
    static void reset();

private:
    static constexpr int NumKeys = 350;
    static bool key_states[NumKeys];
    static int key_state_refs;
};

}
