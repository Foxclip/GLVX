#pragma once

#include <vector>
#include "glvis/glvis_common.h"
#include "glvis/vertex.h"
#include <glm/glm.hpp>

namespace glvis {
    
    class VertexBuffer {
    public:
        VertexBuffer();
        ~VertexBuffer();
        bool create(std::size_t vertexCount);
        std::size_t getVertexCount() const;
        bool update(const std::vector<Vertex>& newVertices);
        bool resize(std::size_t newSize);
        void append(const Vertex& vertex);
        void clear();
        PrimitiveType getPrimitiveType() const;
        void setPrimitiveType(PrimitiveType type);
        unsigned int getVAO() const;
        void render(const glm::mat4& view, const glm::mat4& projection) const;
        Vertex& operator[](std::size_t index);
        const Vertex& operator[](unsigned int index) const;

    private:
        void recreateBuffer(const std::vector<Vertex>& data);
        PrimitiveType type = PrimitiveType::Triangles;
        std::vector<Vertex> vertices;
        unsigned int VAO = 0;
        unsigned int VBO = 0;

    };

}
