#include "glvis/shape.h"

namespace glvis {

Matrix4 Shape::getModelMatrix() const {
    return Transformable::getModelMatrix();
}

void Shape::setShader(Shader* shader) {
    this->shader = shader;
}

}
