#include "glvx/float_rect.h"
#include <algorithm>

namespace glvx {

FloatRect::FloatRect() : position(0.0f, 0.0f), size(0.0f, 0.0f) {}

FloatRect::FloatRect(float x, float y, float width, float height) : position(x, y), size(width, height) {}

FloatRect::FloatRect(const Vector2f& position, const Vector2f& size) : position(position), size(size) {}

void FloatRect::extend(const FloatRect& other) {
    float right = position.x + size.x;
    float other_right = other.position.x + other.size.x;
    float bottom = position.y + size.y;
    float other_bottom = other.position.y + other.size.y;
    if (other.position.x < position.x) {
        float diff = position.x - other.position.x;
        position.x = other.position.x;
        size.x += diff;
    }
    if (other.position.y < position.y) {
        float diff = position.y - other.position.y;
        position.y = other.position.y;
        size.y += diff;
    }
    if (other_right > right) {
        size.x += other_right - right;
    }
    if (other_bottom > bottom) {
        size.y += other_bottom - bottom;
    }
}

bool FloatRect::intersects(const FloatRect& other) const {
    FloatRect intersection;
    intersects(other, intersection);
    return intersection.size.x > 0.0f && intersection.size.y > 0.0f;
}

void FloatRect::intersects(const FloatRect& other, FloatRect& intersection) const {
    float left = std::max(position.x, other.position.x);
    float top = std::max(position.y, other.position.y);
    float right = std::min(position.x + size.x, other.position.x + other.size.x);
    float bottom = std::min(position.y + size.y, other.position.y + other.size.y);
    if (right > left && bottom > top) {
        intersection.position = Vector2f(left, top);
        intersection.size = Vector2f(right - left, bottom - top);
    }
    else {
        intersection = FloatRect();
    }
}

}
