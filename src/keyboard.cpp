#include "glvis/keyboard.h"

namespace glvis {

bool Keyboard::key_states[Keyboard::NumKeys] = {};
int Keyboard::key_state_refs = 0;

bool Keyboard::isKeyPressed(key::Key key) {
    int idx = static_cast<int>(key);
    if (idx < 0 || idx >= NumKeys) return false;
    return key_states[idx];
}

bool Keyboard::isModifierActive(key::Modifier modifier) {
    switch (modifier) {
        case key::Modifier::Control:
            return isKeyPressed(key::Key::LControl) || isKeyPressed(key::Key::RControl);
        case key::Modifier::Shift:
            return isKeyPressed(key::Key::LShift) || isKeyPressed(key::Key::RShift);
        case key::Modifier::Alt:
            return isKeyPressed(key::Key::LAlt) || isKeyPressed(key::Key::RAlt);
        case key::Modifier::System:
            return isKeyPressed(key::Key::LSystem) || isKeyPressed(key::Key::RSystem);
        default:
            return false;
    }
}

void Keyboard::setKeyState(key::Key key, bool pressed) {
    int idx = static_cast<int>(key);
    if (idx >= 0 && idx < NumKeys) {
        key_states[idx] = pressed;
    }
}

void Keyboard::reset() {
    for (int i = 0; i < NumKeys; ++i) {
        key_states[i] = false;
    }
}

}
