#include "glvx/keyboard.h"

namespace glvx {

bool Keyboard::m_key_states[Keyboard::NumKeys] = {};

bool Keyboard::isKeyPressed(Key key) {
    int idx = static_cast<int>(key);
    if (idx < 0 || idx >= NumKeys) {
        return false;
    }
    return m_key_states[idx];
}

bool Keyboard::isModifierActive(Modifier modifier) {
    switch (modifier) {
        case Modifier::Control: return isKeyPressed(Key::LControl) || isKeyPressed(Key::RControl);
        case Modifier::Shift:   return isKeyPressed(Key::LShift) || isKeyPressed(Key::RShift);
        case Modifier::Alt:     return isKeyPressed(Key::LAlt) || isKeyPressed(Key::RAlt);
        case Modifier::System:  return isKeyPressed(Key::LSystem) || isKeyPressed(Key::RSystem);
        default:                return false;
    }
}

void Keyboard::setKeyState(Key key, bool pressed) {
    int idx = static_cast<int>(key);
    if (idx >= 0 && idx < NumKeys) {
        m_key_states[idx] = pressed;
    }
}

void Keyboard::reset() {
    for (int i = 0; i < NumKeys; ++i) {
        m_key_states[i] = false;
    }
}

}
