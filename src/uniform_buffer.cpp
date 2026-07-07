#include "glvis/uniform_buffer.h"
#include "glvis/glvis_common.h"
#include <cstring>

namespace glvis {

UniformBuffer::UniformBuffer() {
}

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
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix4), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::createObjectUBO() {
    GL_CALL(glGenBuffers(1, &objectID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, objectID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) + 5 * sizeof(float), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updateCameraUBO(const Matrix4& view, const Matrix4& projection) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, cameraID));
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), view.getData()));
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4), sizeof(Matrix4), projection.getData()));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, objectID));

    float data[sizeof(Matrix4) / sizeof(float) + 5] = {};
    std::memcpy(data, model.getData(), sizeof(Matrix4));
    size_t offset = sizeof(Matrix4) / sizeof(float);
    data[offset + 0] = color.r;
    data[offset + 1] = color.g;
    data[offset + 2] = color.b;
    data[offset + 3] = color.a;
    data[offset + 4] = hasTexture ? 1.0f : 0.0f;

    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), data));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::bindCameraUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraID));
}

void UniformBuffer::bindObjectUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, objectID));
}

}
