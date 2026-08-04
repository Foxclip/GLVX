#pragma once

#include "glvx/vertex_array.h"
#include "glvx/transformable.h"

namespace glvx {

class Shape : public VertexArray, public Transformable {
public:
    Transform getTransform() const override;

protected:
    using VertexArray::VertexArray;  // Inherit constructors from VertexArray

};

}
