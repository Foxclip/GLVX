#pragma once

#include "glvis/vector.h"
#include "glvis/matrix.h"
#include "glvis/angle.h"
#include "glvis/transformable.h"

namespace glvis {

const float VIEW_ZOOM_FACTOR = 1.2f;

class View : public Transformable {
public:
    float getZoom() const;
    void setZoom(float zoom);
    Matrix4 getViewMatrix(float width, float height, bool filp_y = false) const;
    Matrix4 getInvViewMatrix(float width, float height, bool filp_y = false) const;
    Matrix4 getProjectionMatrix(float width, float height) const;
};

}
