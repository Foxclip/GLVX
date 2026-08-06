#pragma once

#include "glvx/transform.h"

namespace glvx {

class AbstractTexture;
class Shader;

class RenderStates {
public:
    RenderStates();

    Transform m_transform;
    AbstractTexture* m_texture;
    Shader* m_shader;
    bool m_texture_is_premultiplied;
};

} // namespace glvx
