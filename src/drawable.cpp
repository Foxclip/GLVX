#include "glvis/drawable.h"
#include <glad/glad.h>
#include "glvis/shader.h"
#include "glvis/abstract_texture.h"
#include "glvis/vertex_buffer.h"
#include "glvis/drawable.h"

namespace glvis {

glm::mat4 Drawable::getModelMatrix() const {
    return glm::mat4(1.0f);
}

Color Drawable::getColor() const {
    return color;
}

void Drawable::setColor(const Color& color) {
    this->color = color;
}

void Drawable::renderBase(Shader* shader, AbstractTexture* texture, const glm::mat4& view, const glm::mat4& projection) const {
    shader->setVec4("color", glm::vec4(color.r, color.g, color.b, color.a));
    shader->setInt("tex", 0);
    if (texture) {
        shader->setBool("hasTexture", true);
        texture->bind();
    } else {
        shader->setBool("hasTexture", false);
    }
    const VertexBuffer& vertexBuffer = getVertexBuffer();
    vertexBuffer.render(view, projection);
}

}
