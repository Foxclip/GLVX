#pragma once

#include "glvis/vector.h"

namespace glvis {

struct Rect {
public:
    Vector2f position;
    Vector2f size;

    Rect();
    Rect(float x, float y, float width, float height);
    Rect(const Vector2f& position, const Vector2f& size);
    void extend(const Rect& other);
};

}
