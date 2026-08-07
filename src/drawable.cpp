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
    return m_color;
}

void Drawable::setColor(const Color& color) {
    m_color = color;
}

Shader* Drawable::getShader() const {
    return m_shader;
}

void Drawable::setShader(Shader* shader) {
    m_shader = shader;
}

AbstractTexture* Drawable::getTexture() const {
    return m_texture;
}

void Drawable::setTexture(AbstractTexture* texture) {
    m_texture = texture;
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

    if (renderShader->isUsingUBO()) {
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
    renderBase(m_shader, m_texture, m_color, getTransform(), view, projection, states);
}

}
