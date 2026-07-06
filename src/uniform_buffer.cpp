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
    if (perObjectID) {
        GL_CALL(glDeleteBuffers(1, &perObjectID));
    }
}

void UniformBuffer::createCamera() {
    GL_CALL(glGenBuffers(1, &cameraID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, cameraID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, 256, nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::createPerObject() {
    GL_CALL(glGenBuffers(1, &perObjectID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, perObjectID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, 256, nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updateCamera(const Matrix4& view, const Matrix4& projection) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, cameraID));
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, view.getData()));
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, projection.getData()));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::updatePerObject(const Matrix4& model, const Color& color, bool hasTexture) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, perObjectID));

    float data[21] = {};
    std::memcpy(data, model.getData(), 64);
    data[16] = color.r;
    data[17] = color.g;
    data[18] = color.b;
    data[19] = color.a;
    data[20] = hasTexture ? 1.0f : 0.0f;

    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), data));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::bindCamera() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraID));
}

void UniformBuffer::bindPerObject() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, perObjectID));
}

}
