#include "glvis/rectangle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "glvis/glvis_common.h"
#include "glvis/texture.h"

namespace glvis {

    Rectangle::Rectangle(float width, float height) {
        this->width = width;
        this->height = height;
        this->shader = common::defaultShader;

        // Create vertices for rectangle (2 triangles, 6 vertices total)
        std::vector<Vertex> vertices(6);
        // First triangle (0, height), (0, 0), (width, height)
        vertices[0].position = Vector2(0.0f, height);
        vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[0].texCoords = Vector2(0.0f, 1.0f);
        
        vertices[1].position = Vector2(0.0f, 0.0f);
        vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[1].texCoords = Vector2(0.0f, 0.0f);
        
        vertices[2].position = Vector2(width, height);
        vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[2].texCoords = Vector2(1.0f, 1.0f);
        
        // Second triangle (width, height), (0, 0), (width, 0)
        vertices[3].position = Vector2(width, height);
        vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[3].texCoords = Vector2(1.0f, 1.0f);
        
        vertices[4].position = Vector2(0.0f, 0.0f);
        vertices[4].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[4].texCoords = Vector2(0.0f, 0.0f);
        
        vertices[5].position = Vector2(width, 0.0f);
        vertices[5].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[5].texCoords = Vector2(1.0f, 0.0f);

        // Initialize vertex buffer with 6 vertices
        vertexBuffer.create(6);
        vertexBuffer.update(vertices);
        vertexBuffer.setPrimitiveType(PrimitiveType::Triangles);
    }

    Rectangle::~Rectangle() {
        // VertexBuffer destructor will handle cleanup
    }

    float Rectangle::getWidth() const {
        return width;
    }

    float Rectangle::getHeight() const {
        return height;
    }

    Vector2 Rectangle::getSize() const {
        return Vector2(width, height);
    }

    void Rectangle::setTexture(AbstractTexture* texture) {
        this->texture = texture;
    }

    void Rectangle::render(const glm::mat4& view, const glm::mat4& projection) const {
        START_TRY
            if (shader == nullptr) throw std::runtime_error("Shader not set");
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f));
            modelMatrix = glm::rotate(modelMatrix, rotation.asRadians(), glm::vec3(0.0f, 0.0f, -1.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale.x, scale.y, 1.0f));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-origin.x, -origin.y, 0.0f));
            shader->use();
            // No GL_CALL needed here as it's a method call
            shader->setMat4("model", modelMatrix);
            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            // Commented out the color override to use vertex colors
            // shader->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            if (texture) {
                shader->setInt("tex", 0);
                shader->setBool("hasTexture", true);
                texture->bind();
            } else {
                shader->setInt("tex", -1);
                shader->setBool("hasTexture", false);
            }
            vertexBuffer.render(view, projection);
        END_TRY
    }
}
