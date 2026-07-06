#pragma once

#include <glad/glad.h>
#include "glvis/matrix.h"
#include "glvis/color.h"

namespace glvis {

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void createCamera();
    void createPerObject();

    void updateCamera(const Matrix4& view, const Matrix4& projection);
    void updatePerObject(const Matrix4& model, const Color& color, bool hasTexture);

    void bindCamera() const;
    void bindPerObject() const;

    unsigned int getCameraID() const { return cameraID; }
    unsigned int getPerObjectID() const { return perObjectID; }

private:
    unsigned int cameraID = 0;
    unsigned int perObjectID = 0;
};

}
