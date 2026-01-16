#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "vertex.h"
#include "vertex_buffer.h"
#include "drawable.h"

namespace glvis {

class Shader;
class AbstractTexture;

class VertexArray : public Drawable {
public:
    VertexArray();
    VertexArray(PrimitiveType type, std::size_t vertexCount = 0);
    std::size_t getVertexCount() const;
    PrimitiveType getPrimitiveType() const;
    glm::vec2 getBoundsMin() const;
    glm::vec2 getBoundsMax() const;
    Vertex& operator[](std::size_t index);
    const Vertex& operator[](unsigned int index) const;
    void clear();
    void resize(unsigned int newSize);
    void append(const Vertex& vertex);
    void setPrimitiveType(PrimitiveType type);
    void setShader(Shader* shader);
    void setTexture(AbstractTexture* texture);
    void render(const glm::mat4& view, const glm::mat4& projection) const override;
    const VertexBuffer& getVertexBuffer() const override;
    glm::mat4 getModelMatrix() const override;

private:
    VertexBuffer vertexBuffer;
    Shader* shader = nullptr;
    AbstractTexture* texture = nullptr;
};

}
