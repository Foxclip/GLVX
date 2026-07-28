#pragma once

#include "glvis/transform.h"

namespace glvis {

class AbstractTexture;
class Shader;

class RenderStates {
public:
    RenderStates();

    Transform transform;
    AbstractTexture* texture;
    Shader* shader;
    bool texture_is_premultiplied;
};

} // namespace glvis
