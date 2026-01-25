#pragma once

#include "glvis/vector.h"
#include "glvis/matrix.h"
#include "glvis/angle.h"

namespace glvis {

const float CAMERA_ZOOM_FACTOR = 1.2f;

class Camera {
    public:
    const Vector2& getPosition() const;
    float getZoom() const;
    const Angle& getRotation() const;
    void setPosition(const Vector2& pos);
    void setZoom(float zoom);
    void setRotation(const Angle& rotation);
    Matrix4 getViewMatrix(float width, float height) const;
    Matrix4 getInvViewMatrix(float width, float height) const;
    Matrix4 getProjectionMatrix(float width, float height) const;

    private:
    Vector2 pos = Vector2(0.0f, 0.0f);
    float zoom = 1.0f;
    Angle rotation = radians(0.0f);
};

}
