#pragma once

#include "glvx/blend_mode.h"
#include "glvx/transform.h"

namespace glvx {

class AbstractTexture;
class Shader;

class RenderStates {
public:
    RenderStates();

    Transform transform;
    AbstractTexture* texture;
    Shader* shader;
    bool texture_is_premultiplied;
    BlendMode blend_mode;
};

} // namespace glvx
