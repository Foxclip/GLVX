#pragma once

#include <glad/glad.h>
#include "glvis/matrix.h"
#include "glvis/color.h"

namespace glvis {

struct CameraUBO {
    float view[16];
    float projection[16];
};

struct ObjectUBO {
    float model[16];
    float color[4];
    float hasTexture;
    float premultiplyOutput;
};

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void createCameraUBO();
    void createObjectUBO();

    void updateCameraUBO(const Matrix4& view, const Matrix4& projection);
    void updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture, bool premultiplyOutput);

    void bindCameraUBO() const;
    void bindObjectUBO() const;

    unsigned int getCameraID() const { return camera_id; }
    unsigned int getObjectID() const { return object_id; }

private:
    unsigned int camera_id = 0;
    unsigned int object_id = 0;
    Matrix4 cached_view;
    Matrix4 cached_projection;
    bool first_camera_UBO_update = true;
};

}
