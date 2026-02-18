#include "glvis/view.h"
#include "glvis/vector.h"
#include "glvis/utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glvis {

float View::getZoom() const {
    return getScale().x;
}

void View::setZoom(float zoom) {
    setScale(Vector2f(zoom, zoom));
}

Matrix4 View::getViewMatrix(float width, float height) const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(width / 2, height / 2, 0.0f));
    view = glm::rotate(view, -getRotation().asRadians(), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::scale(view, glm::vec3(getZoom(), -getZoom(), 1.0f));
    view = glm::translate(view, glm::vec3(-getPosition().x, -getPosition().y, 0.0f));
    return from_glmMat4(view);
}

Matrix4 View::getInvViewMatrix(float width, float height) const {
    glm::mat4 invView = glm::mat4(1.0f);
    invView = glm::translate(invView, glm::vec3(getPosition().x, getPosition().y, 0.0f));
    invView = glm::scale(invView, glm::vec3(1.0f / getZoom(), -1.0f / getZoom(), 1.0f));
    invView = glm::rotate(invView, getRotation().asRadians(), glm::vec3(0.0f, 0.0f, 1.0f));
    invView = glm::translate(invView, glm::vec3(-width / 2, -height / 2, 0.0f));
    return from_glmMat4(invView);
}

Matrix4 View::getProjectionMatrix(float width, float height) const {
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    return from_glmMat4(projection);
}

}
