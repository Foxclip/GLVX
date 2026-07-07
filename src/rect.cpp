#include "glvis/rect.h"
#include <algorithm>

namespace glvis {

Rect::Rect() : position(0.0f, 0.0f), size(0.0f, 0.0f) {}

Rect::Rect(float x, float y, float width, float height) : position(x, y), size(width, height) {}

Rect::Rect(const Vector2f& position, const Vector2f& size) : position(position), size(size) {}

void Rect::extend(const Rect& other) {
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

}
