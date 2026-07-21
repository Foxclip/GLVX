#pragma once

#include <glad/glad.h>
#include "glvis/matrix.h"
#include "glvis/color.h"

namespace glvis {

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

    void setVP(const Matrix4& view, const Matrix4& projection);
    void updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture, bool premultiplyOutput);

    void bindObjectUBO() const;

    unsigned int getObjectID() const { return object_id; }

private:
    unsigned int object_id = 0;
    Matrix4 cached_vp;
};

}
