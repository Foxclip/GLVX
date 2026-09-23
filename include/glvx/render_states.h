#pragma once

#include "glvx/blend_mode.h"
#include "glvx/transform.h"

namespace glvx {

class AbstractTexture;
class Shader;

struct RenderStates {
    Transform transform;
    AbstractTexture* texture = nullptr;
    Shader* shader = nullptr;
    bool premultiply_output = false;
    BlendMode blend_mode;
};

} // namespace glvx
