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
    renderShader->use();
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
    vertexBuffer.render();
}

}
