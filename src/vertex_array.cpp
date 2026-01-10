#include "glvis/vertex_array.h"
#include <algorithm>
#include <limits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/shader.h"
#include "glvis/glvis_common.h"

namespace glvis {

    VertexArray::VertexArray() { 
        shader = common::defaultShader;
    }

    VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount) {
        vertexBuffer.create(vertexCount);
        vertexBuffer.setPrimitiveType(type);
        shader = common::defaultShader;
    }

    std::size_t VertexArray::getVertexCount() const {
        return vertexBuffer.getVertexCount();
    }

    PrimitiveType VertexArray::getPrimitiveType() const {
        return vertexBuffer.getPrimitiveType();
    }

    Vertex& VertexArray::operator[](std::size_t index) {
        return vertexBuffer[index];
    }

    const Vertex& VertexArray::operator[](unsigned int index) const {
        return vertexBuffer[index];
    }

    void VertexArray::clear() {
        vertexBuffer.clear();
    }

    void VertexArray::resize(unsigned int newSize) {
        vertexBuffer.resize(newSize);
    }

    void VertexArray::append(const Vertex& vertex) {
        vertexBuffer.append(vertex);
    }

    void VertexArray::setPrimitiveType(PrimitiveType type) {
        vertexBuffer.setPrimitiveType(type);
    }

    void VertexArray::setShader(Shader* shader) {
        this->shader = shader;
    }

    void VertexArray::render(const glm::mat4& view, const glm::mat4& projection) const {
        if (shader == nullptr) return;
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        shader->use();
        shader->setMat4("model", modelMatrix);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setInt("tex", 0);
        shader->setBool("hasTexture", false);
        vertexBuffer.render(view, projection);
    }

}
