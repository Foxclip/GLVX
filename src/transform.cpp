#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "glvis/transform.h"
#include "glvis/utils.h"

namespace glvis {

Transform::Transform() : m_matrix(glm::mat4(1.0f)) { }

Transform::Transform(const Matrix4& matrix) : m_matrix(to_glmMat4(matrix)) { }

Transform& Transform::translate(float x, float y) {
    m_matrix = m_matrix * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    return *this;
}

Transform& Transform::translate(const Vector2f& vector) {
    m_matrix = m_matrix * glm::translate(glm::mat4(1.0f), glm::vec3(vector.x, vector.y, 0.0f));
    return *this;
}

Transform& Transform::rotate(float angle, const Vector2f& center) {
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::translate(rotation, glm::vec3(center.x, center.y, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    rotation = glm::translate(rotation, glm::vec3(-center.x, -center.y, 0.0f));
    m_matrix = m_matrix * rotation;
    return *this;
}

Transform& Transform::scale(float factor_x, float factor_y, float center_x, float center_y) {
    glm::mat4 scaling = glm::mat4(1.0f);
    scaling = glm::translate(scaling, glm::vec3(center_x, center_y, 0.0f));
    scaling = glm::scale(scaling, glm::vec3(factor_x, factor_y, 1.0f));
    scaling = glm::translate(scaling, glm::vec3(-center_x, -center_y, 0.0f));
    m_matrix = m_matrix * scaling;
    return *this;
}

Transform& Transform::scale(float factor_x, float factor_y) {
    return scale(factor_x, factor_y, 0.0f, 0.0f);
}

Transform& Transform::scale(const Vector2f& factor, const Vector2f& center) {
    return scale(factor.x, factor.y, center.x, center.y);
}

Transform& Transform::scale(const Vector2f& factor) {
    return scale(factor.x, factor.y);
}

Transform& Transform::combine(const Transform& transform) {
    m_matrix = m_matrix * transform.m_matrix;
    return *this;
}

Vector2f Transform::transformPoint(float x, float y) const {
    glm::vec4 result = m_matrix * glm::vec4(x, y, 0.0f, 1.0f);
    return Vector2f(result.x, result.y);
}

Vector2f Transform::transformPoint(const Vector2f& point) const {
    return transformPoint(point.x, point.y);
}

Rect Transform::transformRect(const Rect& rect) const {
    Vector2f p0 = transformPoint(rect.position.x, rect.position.y);
    Vector2f p1 = transformPoint(rect.position.x + rect.size.x, rect.position.y);
    Vector2f p2 = transformPoint(rect.position.x, rect.position.y + rect.size.y);
    Vector2f p3 = transformPoint(rect.position.x + rect.size.x, rect.position.y + rect.size.y);

    float min_x = std::min({ p0.x, p1.x, p2.x, p3.x });
    float min_y = std::min({ p0.y, p1.y, p2.y, p3.y });
    float max_x = std::max({ p0.x, p1.x, p2.x, p3.x });
    float max_y = std::max({ p0.y, p1.y, p2.y, p3.y });

    return Rect(min_x, min_y, max_x - min_x, max_y - min_y);
}

Transform Transform::getInverse() const {
    float determinant = glm::determinant(m_matrix);
    if (std::abs(determinant) < 1e-6f) {
        return Transform();
    }
    Transform result;
    result.m_matrix = glm::inverse(m_matrix);
    return result;
}

Matrix4 Transform::toMatrix4() const {
    return from_glmMat4(m_matrix);
}

Transform& Transform::operator*=(const Transform& transform) {
    return combine(transform);
}

Vector2f Transform::operator*(const Vector2f& point) const {
    return transformPoint(point);
}

bool Transform::operator==(const Transform& other) const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (std::abs(m_matrix[i][j] - other.m_matrix[i][j]) > 1e-6f) {
                return false;
            }
        }
    }
    return true;
}

bool Transform::operator!=(const Transform& other) const {
    return !(*this == other);
}

Transform operator*(const Transform& left, const Transform& right) {
    Transform result = left;
    result.combine(right);
    return result;
}

}
