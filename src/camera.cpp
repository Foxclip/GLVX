#include "glvis/camera.h"

namespace glvis {

const glm::vec2& Camera::getPosition() const {
    return pos;
}

float Camera::getZoom() const {
    return zoom;
}

void Camera::setPosition(const glm::vec2& pos) {
    this->pos = pos;
}

void Camera::setZoom(float zoom) {
    this->zoom = zoom;
}

glm::mat4 Camera::getViewMatrix(float width, float height) const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(width / 2, height / 2, 0.0f));
    view = glm::scale(view, glm::vec3(zoom, -zoom, 1.0f));
    view = glm::translate(view, glm::vec3(-pos.x, -pos.y, 0.0f));
    return view;
}

glm::mat4 Camera::getInvViewMatrix(float width, float height) const {
    glm::mat4 invView = glm::mat4(1.0f);
    invView = glm::translate(invView, glm::vec3(pos.x, pos.y, 0.0f));
    invView = glm::scale(invView, glm::vec3(1.0f / zoom, -1.0f / zoom, 1.0f));
    invView = glm::translate(invView, glm::vec3(-width / 2, -height / 2, 0.0f));
    return invView;
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const {
    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
    return projection;
}

}
