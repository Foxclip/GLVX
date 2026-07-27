#pragma once

#include "glvis/vector.h"
#include "glvis/matrix.h"
#include "glvis/float_rect.h"
#include "glvis/angle.h"

namespace glvis {

class Transform {
public:
    Transform();
    explicit Transform(const Matrix4& matrix);

    Transform& translate(float x, float y);
    Transform& translate(const Vector2f& vector);
    Transform& rotate(const Angle& angle, const Vector2f& center);
    Transform& scale(float factor_x, float factor_y, float center_x, float center_y);
    Transform& scale(float factor_x, float factor_y);
    Transform& scale(const Vector2f& factor, const Vector2f& center);
    Transform& scale(const Vector2f& factor);
    Transform& combine(const Transform& transform);
    Vector2f transformPoint(float x, float y) const;
    Vector2f transformPoint(const Vector2f& point) const;
    FloatRect transformRect(const FloatRect& rect) const;
    Transform getInverse() const;
    Matrix4 toMatrix4() const;
    const float* getData() const;
    Transform& operator*=(const Transform& transform);
    Vector2f operator*(const Vector2f& point) const;
    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;

private:
    glm::mat4 m_matrix;
};

Transform operator*(const Transform& left, const Transform& right);

}
