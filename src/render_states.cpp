#include "glvis/render_states.h"

namespace glvis {

RenderStates::RenderStates()
    : texture(nullptr), shader(nullptr), texture_is_premultiplied(false) {
    // transform is default-constructed to identity via Matrix4()
}

} // namespace glvis
