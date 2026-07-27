#pragma once

#include "glvis/vector.h"

namespace glvis {

struct FloatRect {
public:
    Vector2f position;
    Vector2f size;

    FloatRect();
    FloatRect(float x, float y, float width, float height);
    FloatRect(const Vector2f& position, const Vector2f& size);
    void extend(const FloatRect& other);
};

}
