#pragma once

#include "glvis/drawable.h"
#include "glvis/transformable.h"

namespace glvis {

class Shader;

class Shape : public Drawable, public Transformable {
public:
    Matrix4 getModelMatrix() const override;

protected:


private:

};

}
