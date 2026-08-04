#pragma once

#include "glvx/vector.h"

namespace glvx {

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
