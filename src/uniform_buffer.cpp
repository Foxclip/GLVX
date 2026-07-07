#include "glvis/uniform_buffer.h"
#include "glvis/glvis_common.h"
#include <cstring>

namespace glvis {

UniformBuffer::UniformBuffer() { }

UniformBuffer::~UniformBuffer() {
    if (cameraID) {
        GL_CALL(glDeleteBuffers(1, &cameraID));
    }
    if (objectID) {
        GL_CALL(glDeleteBuffers(1, &objectID));
    }
}

void UniformBuffer::createCameraUBO() {
    GL_CALL(glGenBuffers(1, &cameraID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, cameraID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::createObjectUBO() {
    GL_CALL(glGenBuffers(1, &objectID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, objectID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectUBO), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updateCameraUBO(const Matrix4& view, const Matrix4& projection) {
    if (firstCameraUBOUpdate || cachedView != view || cachedProjection != projection) {
        cachedView = view;
        cachedProjection = projection;
        firstCameraUBOUpdate = false;

        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, cameraID));

        CameraUBO ubo;
        std::memcpy(ubo.view, view.getData(), sizeof(ubo.view));
        std::memcpy(ubo.projection, projection.getData(), sizeof(ubo.projection));

        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo), &ubo));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
}

void UniformBuffer::updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, objectID));

    ObjectUBO ubo = {};
    std::memcpy(ubo.model, model.getData(), sizeof(ubo.model));
    ubo.color[0] = static_cast<float>(color.r);
    ubo.color[1] = static_cast<float>(color.g);
    ubo.color[2] = static_cast<float>(color.b);
    ubo.color[3] = static_cast<float>(color.a);
    ubo.hasTexture = hasTexture ? 1.0f : 0.0f;

    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo), &ubo));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::bindCameraUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraID));
}

void UniformBuffer::bindObjectUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, objectID));
}

}
