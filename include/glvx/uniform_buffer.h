#pragma once

#include <glad/glad.h>
#include "glvx/matrix.h"
#include "glvx/color.h"

namespace glvx {

struct ObjectUBO {
    float vp[16];
    float model[16];
    float color[4];
    float hasTexture;
    float premultiplyOutput;
};

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void createObjectUBO();
    void updateObjectUBO(
        const Matrix4& model,
        const Color& color,
        bool hasTexture,
        bool premultiplyOutput,
        const Matrix4& view,
        const Matrix4& projection
    );
    void bindObjectUBO() const;
    unsigned int getObjectID() const { return m_object_id; }

private:
    unsigned int m_object_id = 0;
};

}
