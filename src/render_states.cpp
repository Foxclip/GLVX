#include "glvx/render_states.h"

namespace glvx {

RenderStates::RenderStates()
    : texture(nullptr), shader(nullptr), texture_is_premultiplied(false) {
    // transform is default-constructed to identity via Transform()
}

} // namespace glvx
