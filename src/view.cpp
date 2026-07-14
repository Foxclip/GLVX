#include "glvis/view.h"
#include "glvis/vector.h"
#include "glvis/utils.h"
#include "glvis/matrix.h"

namespace glvis {

float View::getZoom() const {
    return getScale().x;
}

void View::setZoom(float zoom) {
    setScale(Vector2f(zoom, zoom));
}

Matrix4 View::getViewMatrix(float width, float height, bool filp_y) const {
    Matrix4 view;
    float flip = filp_y ? -1.0f : 1.0f;
    view = Matrix4::translate(view, Vector3(width / 2, height / 2, 0.0f));
    view = Matrix4::rotate(view, -getRotation().asRadians(), Vector3(0.0f, 0.0f, 1.0f));
    view = Matrix4::scale(view, Vector3(getZoom(), flip * getZoom(), 1.0f));
    view = Matrix4::translate(view, Vector3(-getPosition().x, -getPosition().y, 0.0f));
    return view;
}

Matrix4 View::getInvViewMatrix(float width, float height, bool filp_y) const {
    Matrix4 invView;
    float flip = filp_y ? -1.0f : 1.0f;
    invView = Matrix4::translate(invView, Vector3(getPosition().x, getPosition().y, 0.0f));
    invView = Matrix4::scale(invView, Vector3(1.0f / getZoom(), flip / getZoom(), 1.0f));
    invView = Matrix4::rotate(invView, getRotation().asRadians(), Vector3(0.0f, 0.0f, 1.0f));
    invView = Matrix4::translate(invView, Vector3(-width / 2, -height / 2, 0.0f));
    return invView;
}

Matrix4 View::getProjectionMatrix(float width, float height) const {
    return Matrix4::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

}
