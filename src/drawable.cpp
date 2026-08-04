#include "glvx/drawable.h"
#include "glvx/shader.h"
#include "glvx/abstract_texture.h"
#include "glvx/vertex_buffer.h"
#include "glvx/uniform_buffer.h"
#include "glvx/glvx_common.h"
#include "glvx/utils.h"
#include <cassert>

namespace glvx {

Transform Drawable::getTransform() const {
    return Transform();
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
    const Transform& model,
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    const VertexBuffer& vertexBuffer = getVertexBuffer();
    if (vertexBuffer.getVertexCount() == 0) return;
    Shader* renderShader = states.shader ? states.shader : shader ? shader : common::defaultShader;
    assert(renderShader);
    const AbstractTexture* renderTexture = states.texture ? states.texture : texture;
    Matrix4 combinedModel = (states.transform * model).toMatrix4();
    bool textureIsPremultiplied = renderTexture != nullptr && renderTexture->isRenderTexture();
    renderShader->use();

    if (renderShader->useUBO) {
        common::uniformBuffer->updateObjectUBO(
            combinedModel, color, renderTexture != nullptr, !textureIsPremultiplied, view, projection
        );
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
    renderBase(shader, texture, color, getTransform(), view, projection, states);
}

}
