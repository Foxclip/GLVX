#include "glvis/angle.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glvis {

    Angle::Angle() { }

    float Angle::asDegrees() const {
        return glm::degrees(radians);
    }

    float Angle::asRadians() const {
        return radians;
    }

    Angle degrees(float degrees) {
        Angle angle;
        angle.radians = glm::radians(degrees);
        return angle;
    }

    Angle radians(float radians) {
        Angle angle;
        angle.radians = radians;
        return angle;
    }
}
