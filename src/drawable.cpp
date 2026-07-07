#include "glvis/drawable.h"
#include <glad/glad.h>
#include "glvis/shader.h"
#include "glvis/abstract_texture.h"
#include "glvis/vertex_buffer.h"
#include "glvis/uniform_buffer.h"
#include "glvis/glvis_common.h"
#include "glvis/utils.h"
#include <cassert>

namespace glvis {

Matrix4 Drawable::getModelMatrix() const {
    return Matrix4(); // Identity
}

Color Drawable::getColor() const {
    return color;
}

void Drawable::setColor(const Color& color) {
    this->color = color;
}

Shader* Drawable::getShader() const {
    return shader;
}

void Drawable::setShader(Shader* shader) {
    this->shader = shader;
}

AbstractTexture* Drawable::getTexture() const {
    return texture;
}

void Drawable::setTexture(AbstractTexture* texture) {
    this->texture = texture;
}

void Drawable::renderBase(
    Shader* shader,
    const AbstractTexture* texture,
    const Color& color,
    const Matrix4& model,
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    const VertexBuffer& vertexBuffer = getVertexBuffer();
    if (vertexBuffer.getVertexCount() == 0) return;
    Shader* renderShader = states.shader ? states.shader : shader;
    assert(renderShader);
    const AbstractTexture* renderTexture = states.texture ? states.texture : texture;
    Matrix4 combinedModel = states.transform * getModelMatrix();
    renderShader->use();

    if (renderShader->useUBO) {
        common::uniformBuffer->updateCameraUBO(view, projection);
        common::uniformBuffer->updateObjectUBO(combinedModel, color, renderTexture != nullptr);
        common::uniformBuffer->bindCameraUBO();
        common::uniformBuffer->bindObjectUBO();
        renderShader->setInt("tex", 0);
        if (renderTexture) {
            renderTexture->bind();
        }
    } else {
        renderShader->setVec4("color", Vector4(color.r, color.g, color.b, color.a));
        renderShader->setMat4("model", combinedModel);
        renderShader->setMat4("view", view);
        renderShader->setMat4("projection", projection);
        renderShader->setInt("tex", 0);
        if (renderTexture) {
            renderShader->setBool("hasTexture", true);
            renderTexture->bind();
        } else {
            renderShader->setBool("hasTexture", false);
        }
    }

    vertexBuffer.render();
}

void Drawable::render(
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    renderBase(shader, texture, color, getModelMatrix(), view, projection, states);
}

}
