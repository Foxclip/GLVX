#include "glvx/angle.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glvx {

Angle::Angle() { }

float Angle::asDegrees() const {
    return glm::degrees(m_radians);
}

float Angle::asRadians() const {
    return m_radians;
}

Angle Angle::fromDegrees(float degrees) {
    Angle angle;
    angle.m_radians = glm::radians(degrees);
    return angle;
}

Angle Angle::fromRadians(float radians) {
    Angle angle;
    angle.m_radians = radians;
    return angle;
}

Angle Angle::operator-() const {
    Angle result;
    result.m_radians = -m_radians;
    return result;
}

}
