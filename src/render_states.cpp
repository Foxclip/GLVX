#include "glvx/render_states.h"

namespace glvx {

RenderStates::RenderStates()
    : m_texture(nullptr), m_shader(nullptr), m_texture_is_premultiplied(false) {
    // transform is default-constructed to identity via Transform()
}

} // namespace glvx
