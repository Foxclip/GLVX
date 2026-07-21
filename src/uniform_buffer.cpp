#include "glvis/uniform_buffer.h"
#include "glvis/glvis_common.h"
#include <cstring>

namespace glvis {

UniformBuffer::UniformBuffer() { }

UniformBuffer::~UniformBuffer() {
    if (object_id) {
        GL_CALL(glDeleteBuffers(1, &object_id));
    }
}

void UniformBuffer::createObjectUBO() {
    GL_CALL(glGenBuffers(1, &object_id));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, object_id));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectUBO), nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::setVP(const Matrix4& view, const Matrix4& projection) {
    Matrix4 vp = projection * view;
    if (cached_vp != vp) {
        cached_vp = vp;
    }
}

void UniformBuffer::updateObjectUBO(const Matrix4& model, const Color& color, bool hasTexture, bool premultiplyOutput) {
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, object_id));

    ObjectUBO ubo = {};
    std::memcpy(ubo.vp, cached_vp.getData(), sizeof(ubo.vp));
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

void UniformBuffer::bindObjectUBO() const {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, object_id));
}

}