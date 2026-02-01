#pragma once

#include "glvis/vertex_array.h"
#include "glvis/transformable.h"

namespace glvis {

class Shape : public VertexArray, public Transformable {
public:
    Matrix4 getModelMatrix() const override;

protected:
    using VertexArray::VertexArray;  // Inherit constructors from VertexArray

};

}
