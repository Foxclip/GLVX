#pragma once

#include "glvis/matrix.h"

namespace glvis {

class AbstractTexture;
class Shader;

class RenderStates {
public:
    RenderStates();

    Matrix4 transform;
    AbstractTexture* texture;
    Shader* shader;
};

} // namespace glvis
