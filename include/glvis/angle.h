#pragma once

namespace glvis {

class Angle {
public:
    Angle();
    static Angle fromDegrees(float degrees);
    static Angle fromRadians(float radians);
    float asDegrees() const;
    float asRadians() const;
    Angle operator-() const;

private:
    float m_radians = 0.0f;

};

}
