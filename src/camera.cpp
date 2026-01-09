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

}
