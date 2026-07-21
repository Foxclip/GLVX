#include "glvis/uniform_buffer.h"
#include "glvis/glvis_common.h"
#include <cstring>

namespace glvis {

UniformBuffer::UniformBuffer() { }

UniformBuffer::~UniformBuffer() {
    if (camera_id) {
        GL_CALL(glDeleteBuffers(1, &camera_id));
    }
    if (object_id) {
        GL_CALL(glDeleteBuffers(1, &object_id));
    }
}

void UniformBuffer::createCameraUBO() {
    GL_CALL(glGenBuffers(1, &camera_id));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, camera_id));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::createObjectUBO() {
    GL_CALL(glGenBuffers(1, &object_id));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, object_id));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectUBO), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updateCameraUBO(const Matrix4& view, const Matrix4& projection) {
    if (first_camera_UBO_update || cached_view != view || cached_projection != projection) {
        cached_view = view;
        cached_projection = projection;
        first_camera_UBO_update = false;

        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, camera_id));

        CameraUBO ubo;
        std::memcpy(ubo.view, view.getData(), sizeof(ubo.view));
        std::memcpy(ubo.projection, projection.getData(), sizeof(ubo.projection));

        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo), &ubo));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
}

void UniformBuffer::updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture, bool premultiplyOutput) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, object_id));

    ObjectUBO ubo = {};
    std::memcpy(ubo.model, model.getData(), sizeof(ubo.model));
    ubo.color[0] = static_cast<float>(color.r);
    ubo.color[1] = static_cast<float>(color.g);
    ubo.color[2] = static_cast<float>(color.b);
    ubo.color[3] = static_cast<float>(color.a);
    ubo.hasTexture = hasTexture ? 1.0f : 0.0f;
    ubo.premultiplyOutput = premultiplyOutput ? 1.0f : 0.0f;

    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo), &ubo));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::bindCameraUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_id));
}

void UniformBuffer::bindObjectUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, object_id));
}

}
