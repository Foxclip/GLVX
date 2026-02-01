#include "glvis/drawable.h"
#include <glad/glad.h>
#include "glvis/shader.h"
#include "glvis/abstract_texture.h"
#include "glvis/vertex_buffer.h"
#include "glvis/utils.h"

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

void Drawable::renderBase(
    Shader* shader,
    AbstractTexture* texture,
    const Color& color,
    const Matrix4& model,
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    const VertexBuffer& vertexBuffer = getVertexBuffer();
    Shader* renderShader = states.shader ? states.shader : shader;
    if (renderShader == nullptr) throw std::runtime_error("Shader not set");
    AbstractTexture* renderTexture = states.texture ? states.texture : texture;
    Matrix4 combinedModel = states.transform * getModelMatrix();
    Drawable::renderInternal(
        vertexBuffer, renderShader, renderTexture, color, combinedModel, view, projection
    );
}

void Drawable::renderInternal(
    const VertexBuffer& vertexBuffer,
    Shader* shader,
    AbstractTexture* texture,
    const Color& color,
    const Matrix4& model,
    const Matrix4& view,
    const Matrix4& projection
) {
    shader->use();
    shader->setVec4("color", Vector4(color.r, color.g, color.b, color.a));
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setInt("tex", 0);
    if (texture) {
        shader->setBool("hasTexture", true);
        texture->bind();
    } else {
        shader->setBool("hasTexture", false);
    }
    vertexBuffer.render();
}

}
