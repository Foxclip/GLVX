#include "glvis/camera.h"
#include "glvis/vector.h"
#include "glvis/utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glvis {

const Vector2& Camera::getPosition() const {
    return pos;
}

float Camera::getZoom() const {
    return zoom;
}

void Camera::setPosition(const Vector2& pos) {
    this->pos = pos;
}

void Camera::setZoom(float zoom) {
    this->zoom = zoom;
}

const Angle& Camera::getRotation() const {
    return rotation;
}

void Camera::setRotation(const Angle& rotation) {
    this->rotation = rotation;
}

Matrix4 Camera::getViewMatrix(float width, float height) const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(width / 2, height / 2, 0.0f));
    view = glm::rotate(view, rotation.asRadians(), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::scale(view, glm::vec3(zoom, -zoom, 1.0f));
    view = glm::translate(view, glm::vec3(-pos.x, -pos.y, 0.0f));
    return from_glmMat4(view);
}

Matrix4 Camera::getInvViewMatrix(float width, float height) const {
    glm::mat4 invView = glm::mat4(1.0f);
    invView = glm::translate(invView, glm::vec3(pos.x, pos.y, 0.0f));
    invView = glm::scale(invView, glm::vec3(1.0f / zoom, -1.0f / zoom, 1.0f));
    invView = glm::rotate(invView, -rotation.asRadians(), glm::vec3(0.0f, 0.0f, 1.0f));
    invView = glm::translate(invView, glm::vec3(-width / 2, -height / 2, 0.0f));
    return from_glmMat4(invView);
}

Matrix4 Camera::getProjectionMatrix(float width, float height) const {
    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
    return from_glmMat4(projection);
}

}
