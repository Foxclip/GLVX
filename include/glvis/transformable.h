#pragma once

#include "glvis/vector.h"
#include "glvis/angle.h"
#include "glvis/transform.h"

namespace glvis {

class Transformable {
public:
    float getX() const;
    float getY() const;
    const Vector2f& getPosition() const;
    const Angle& getRotation() const;
    const Vector2f& getScale() const;
    const Vector2f& getOrigin() const;
    Transform getTransform() const;
    void setPosition(float x, float y);
    void setPosition(const Vector2f& position);
    void setRotation(const Angle& rotation);
    void setScale(float x, float y);
    void setScale(const Vector2f& scale);
    void setOrigin(float x, float y);
    void setOrigin(const Vector2f& origin);
    void move(float dx, float dy);
    void move(const Vector2f& offset);

protected:
    Vector2f position;
    Angle rotation;
    Vector2f scale = Vector2f(1.0f, 1.0f);
    Vector2f origin = Vector2f(0.0f, 0.0f);
};

}
