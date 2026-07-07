#pragma once

#include <glad/glad.h>
#include "glvis/matrix.h"
#include "glvis/color.h"

namespace glvis {

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void createCameraUBO();
    void createObjectUBO();

    void updateCameraUBO(const Matrix4& view, const Matrix4& projection);
    void updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture);

    void bindCameraUBO() const;
    void bindObjectUBO() const;

    unsigned int getCameraID() const { return cameraID; }
    unsigned int getObjectID() const { return objectID; }

private:
    unsigned int cameraID = 0;
    unsigned int objectID = 0;
};

}
